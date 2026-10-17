package space.nuvio.nativelegacy

import androidx.media3.common.C
import androidx.media3.datasource.DataSpec
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test
import java.io.ByteArrayInputStream
import java.io.IOException
import java.io.InputStream
import java.io.InterruptedIOException
import java.util.concurrent.atomic.AtomicReference
import java.net.HttpURLConnection
import java.net.SocketTimeoutException
import java.net.URL
import java.util.concurrent.CountDownLatch
import java.util.concurrent.TimeUnit
import java.util.concurrent.atomic.AtomicBoolean
import java.util.concurrent.atomic.AtomicInteger

class ParaleloDataSourceTest {
    private open class ConexaoFalsa(url: URL) : HttpURLConnection(url) {
        val desconectou = AtomicBoolean()
        override fun connect() = Unit
        override fun usingProxy(): Boolean = false
        override fun disconnect() { desconectou.set(true) }
        override fun getInputStream(): InputStream = ByteArrayInputStream(byteArrayOf(1))
        override fun getResponseCode(): Int = HTTP_OK
        override fun getResponseMessage(): String = "OK"
    }

    // Responde 206 honrando o Range pedido: devolve exatamente os bytes da
    // janela, como um servidor de Range de verdade.
    private open class ConexaoFaixa(url: URL, private val total: Long) : ConexaoFalsa(url) {
        private val pedidos = HashMap<String, String>()
        override fun setRequestProperty(key: String?, value: String?) {
            if (key != null && value != null) pedidos[key] = value
        }
        override fun getRequestProperty(key: String?): String? = pedidos[key]
        // Range invalido (ultimo < primeiro) e 416, como um servidor de verdade.
        override fun getResponseCode(): Int =
            if (fim() < ini()) HTTP_REQ_TOO_LONG else HTTP_PARTIAL
        override fun getContentLengthLong(): Long = fim() - ini() + 1
        override fun getHeaderField(name: String?): String? =
            if (name == "Content-Range") "bytes ${ini()}-${fim()}/$total" else super.getHeaderField(name)
        override fun getInputStream(): InputStream =
            ByteArrayInputStream(ByteArray((fim() - ini() + 1).toInt()))
        private fun ini(): Long = faixa().first
        private fun fim(): Long = faixa().second
        private fun faixa(): Pair<Long, Long> {
            val corpo = (pedidos["Range"] ?: "bytes=0-").removePrefix("bytes=")
            val ini = corpo.substringBefore('-').toLong()
            val fim = corpo.substringAfter('-', "")
            return if (fim.isEmpty()) ini to (total - 1) else ini to fim.toLong()
        }
    }

    @Test fun aberturaInicialRepeteFalhaDeConexaoComBackoffLimitado() {
        val aberturas = AtomicInteger()
        val conexoes = ArrayList<ConexaoFalsa>()
        val esperas = ArrayList<Long>()
        val fonte = ParaleloDataSource(null, emptyMap(),
            novaConexao = { url ->
                val tentativa = aberturas.incrementAndGet()
                val c = if (tentativa < 3) object : ConexaoFalsa(url) {
                    override fun getResponseCode(): Int = throw IOException("sem rota")
                } else ConexaoFalsa(url)
                conexoes += c
                c
            },
            dormir = { esperas += it })

        fonte.conectarComRetry(URL("https://example.invalid/video.mkv"), 0, 1023)
        fonte.close()

        assertEquals(3, aberturas.get())
        assertEquals(listOf(250L, 750L), esperas)
        assertTrue(conexoes.take(2).all { it.desconectou.get() })
    }

    @Test fun timeoutDeSocketTambemRecebeTresTentativas() {
        val aberturas = AtomicInteger()
        val esperas = ArrayList<Long>()
        val fonte = ParaleloDataSource(null, emptyMap(),
            novaConexao = { url ->
                object : ConexaoFalsa(url) {
                    override fun getResponseCode(): Int {
                        aberturas.incrementAndGet()
                        throw SocketTimeoutException("timeout")
                    }
                }
            },
            dormir = { esperas += it })

        try {
            fonte.conectarComRetry(URL("https://example.invalid/video.mkv"), 0, 1023)
        } catch (e: FalhaDownloadParalelo) {
            assertTrue(e.cause is SocketTimeoutException)
        }
        finally { fonte.close() }

        assertEquals(3, aberturas.get())
        assertEquals(listOf(250L, 750L), esperas)
    }

    @Test fun closeNaoDevolveCabecalhoObsoleto() {
        lateinit var fonte: ParaleloDataSource
        fonte = ParaleloDataSource(null, emptyMap(), novaConexao = { url ->
            object : ConexaoFalsa(url) {
                override fun getResponseCode(): Int { fonte.close(); return HTTP_OK }
            }
        }, dormir = { throw AssertionError("cancelamento nao deve repetir") })
        try {
            fonte.conectarComRetry(URL("https://example.invalid/video.mkv"), 0, 3)
            throw AssertionError("devolveu conexao depois de close")
        } catch (_: InterruptedIOException) {} finally { fonte.close() }
    }

    @Test fun closeCancelaLeituraUnicaMesmoSeSocketDevolverBytes() {
        val fonte = ParaleloDataSource(null, emptyMap())
        val entrada = ParaleloDataSource::class.java.getDeclaredField("unicaIn")
        entrada.isAccessible = true
        ParaleloDataSource.Estat.conexoes.incrementAndGet()
        entrada.set(fonte, object : InputStream() {
            override fun read(): Int { fonte.close(); return 1 }
        })
        try {
            fonte.read(ByteArray(1), 0, 1)
            throw AssertionError("entregou bytes depois de close")
        } catch (_: InterruptedIOException) {} finally { fonte.close() }
        assertEquals(0, ParaleloDataSource.Estat.conexoes.get())
    }

    @Test fun pedacoRepeteConnectEDesconectaAntesDoBackoff() {
        val aberturas = AtomicInteger()
        val conexoes = ArrayList<ConexaoFalsa>()
        val esperas = ArrayList<Long>()
        val fonte = ParaleloDataSource(null, emptyMap(), novaConexao = { url ->
            val tentativa = aberturas.incrementAndGet()
            val c = object : ConexaoFalsa(url) {
                override fun getResponseCode(): Int =
                    if (tentativa < 3) throw IOException("sem rota") else HTTP_PARTIAL
                override fun getInputStream(): InputStream = ByteArrayInputStream(byteArrayOf(1, 2, 3, 4))
                override fun getContentLengthLong(): Long = 4
            }
            conexoes += c
            c
        }, dormir = {
            assertTrue(conexoes.last().desconectou.get())
            assertEquals(0, ParaleloDataSource.Estat.conexoes.get())
            esperas += it
        })
        val baixar = ParaleloDataSource::class.java.getDeclaredMethod("baixar",
            URL::class.java, Long::class.javaPrimitiveType, Long::class.javaPrimitiveType)
        baixar.isAccessible = true
        try {
            val dados = baixar.invoke(fonte, URL("https://example.invalid/video.mkv"), 0L, 3L) as ByteArray
            assertEquals(listOf<Byte>(1, 2, 3, 4), dados.toList())
        } finally { fonte.close() }
        assertEquals(3, aberturas.get())
        assertEquals(listOf(250L, 750L), esperas)
        assertTrue(conexoes.all { it.desconectou.get() })
        assertEquals(0, ParaleloDataSource.Estat.conexoes.get())
    }

    @Test fun janelaPequenaUsaConexaoUnicaSemPool() {
        // Buraco pequeno do seek-cache: 2 bytes num arquivo de 24 GB. Sobra da
        // regressao do paralelo: o reopen em offset-2 com 0 MB restante.
        val aberturas = AtomicInteger()
        val fonte = ParaleloDataSource(null, emptyMap(), novaConexao = { url ->
            aberturas.incrementAndGet()
            ConexaoFaixa(url, 24891962527L)
        }, dormir = { throw AssertionError("janela pequena nao deve esperar retry") })
        val exec = ParaleloDataSource::class.java.getDeclaredField("exec")
        exec.isAccessible = true
        val pos = 613230488L
        val len = fonte.open(DataSpec.Builder()
            .setUri(android.net.Uri.parse("https://example.invalid/video.mkv"))
            .setPosition(pos)
            .setLength(2L)
            .build())
        assertEquals(2L, len)
        assertEquals(1, aberturas.get())      // uma conexao, nenhum reopen
        assertEquals(null, exec.get(fonte))   // sem pool
        val buf = ByteArray(2)
        assertEquals(2, fonte.read(buf, 0, 2))
        assertEquals(0, buf[0].toInt() and 0xFF)
        assertEquals(C.RESULT_END_OF_INPUT.toLong(), fonte.read(buf, 0, 2).toLong())
        fonte.close()
    }

    @Test fun janelaMediaMaiorQueASondaServeAJanelaInteira() {
        // 3 MB pedidos: acima da sonda (1 MB), abaixo do limiar do pool. A
        // conexao unica nao pode herdar o teto da sonda e acabar aos 1 MB.
        val fonte = ParaleloDataSource(null, emptyMap(), novaConexao = { url ->
            ConexaoFaixa(url, 24891962527L)
        }, dormir = { throw AssertionError("conexao viva nao deve esperar retry") })
        val janela = 3L shl 20
        val len = fonte.open(DataSpec.Builder()
            .setUri(android.net.Uri.parse("https://example.invalid/video.mkv"))
            .setPosition(1000L)
            .setLength(janela)
            .build())
        assertEquals(janela, len)
        val buf = ByteArray(1 shl 16)
        var lidos = 0L
        while (true) {
            val n = fonte.read(buf, 0, buf.size)
            if (n == C.RESULT_END_OF_INPUT) break
            lidos += n
        }
        fonte.close()
        assertEquals(janela, lidos)
    }

    @Test fun buracoGrandeNaoBaixaTudoNaSonda() {
        // Seek para tras sobre 200 MB sem cache: a sonda fica em PRIMEIRO, o
        // resto vai pelo pool, nunca um ByteArray de 200 MB na thread de quem abriu.
        val conexoes = java.util.Collections.synchronizedList(ArrayList<ConexaoFaixa>())
        val fonte = ParaleloDataSource(null, emptyMap(), novaConexao = { url ->
            ConexaoFaixa(url, 24891962527L).also { conexoes += it }
        }, dormir = { throw AssertionError("conexao viva nao deve esperar retry") })
        try {
            val len = fonte.open(DataSpec.Builder()
                .setUri(android.net.Uri.parse("https://example.invalid/video.mkv"))
                .setPosition(0L)
                .setLength(200L shl 20)
                .build())
            assertEquals(200L shl 20, len)
            assertEquals("bytes=0-${(1 shl 20) - 1}", conexoes[0].getRequestProperty("Range"))
        } finally {
            fonte.close()
        }
    }

    @Test fun aberturaComumUsaParalelo() {
        // Abertura progressiva comum (length UNSET, arquivo de 24 GB): tem que
        // criar o pool. A janela e o resto do arquivo, nao o pedaco de sonda.
        val fonte = ParaleloDataSource(null, emptyMap(), novaConexao = { url ->
            ConexaoFaixa(url, 24891962527L)
        }, dormir = { throw AssertionError("conexao viva nao deve esperar retry") })
        val exec = ParaleloDataSource::class.java.getDeclaredField("exec")
        exec.isAccessible = true
        var pool: java.util.concurrent.ExecutorService? = null
        try {
            val len = fonte.open(DataSpec.Builder()
                .setUri(android.net.Uri.parse("https://example.invalid/video.mkv"))
                .setPosition(0L)
                .setLength(C.LENGTH_UNSET.toLong())
                .build())
            assertEquals(24891962527L, len)
            pool = exec.get(fonte) as java.util.concurrent.ExecutorService
            assertTrue("abertura comum deve criar o pool", pool != null)
            val buf = ByteArray(16)
            assertEquals(16, fonte.read(buf, 0, 16))
        } finally {
            fonte.close()
            // Os fios do pool contam +1/-1 em Estat.conexoes: sem espera-los,
            // o saldo vaza para o assert do teste seguinte.
            pool?.let { assertTrue(it.awaitTermination(10, TimeUnit.SECONDS)) }
            assertEquals(0, ParaleloDataSource.Estat.conexoes.get())
        }
    }

    @Test fun janelaVaziaAbreSemConectar() {
        // DataSpec de 0 byte: abre vazio, sem Range invalido (pos-1) nem 416.
        val aberturas = AtomicInteger()
        val fonte = ParaleloDataSource(null, emptyMap(), novaConexao = { url ->
            aberturas.incrementAndGet()
            ConexaoFaixa(url, 24891962527L)
        }, dormir = { throw AssertionError("janela vazia nao deve esperar retry") })
        val len = fonte.open(DataSpec.Builder()
            .setUri(android.net.Uri.parse("https://example.invalid/video.mkv"))
            .setPosition(613230448L)
            .setLength(1L)
            .build().also { d ->
                // O Builder do media3 recusa length=0, mas o campo e public: abrir()
                // tem que abrir vazio em vez de pedir o Range invalido (pos-1).
                val campo = DataSpec::class.java.getDeclaredField("length")
                campo.isAccessible = true
                campo.set(d, 0L)
            })
        assertEquals(0L, len)
        assertEquals(0, aberturas.get())
        assertEquals(C.RESULT_END_OF_INPUT.toLong(), fonte.read(ByteArray(2), 0, 2).toLong())
        fonte.close()
    }

    @Test fun aberturaInicialRepeteSemEsperarNaPrimeiraFalha() {
        val aberturas = AtomicInteger()
        val esperas = ArrayList<Long>()
        val fonte = ParaleloDataSource(null, emptyMap(),
            novaConexao = { url ->
                val tentativa = aberturas.incrementAndGet()
                if (tentativa == 1) object : ConexaoFalsa(url) {
                    override fun getResponseCode(): Int = throw IOException("sem rota")
                } else ConexaoFalsa(url)
            },
            dormir = { esperas += it })
        fonte.conectarComRetry(URL("https://example.invalid/video.mkv"), 0, 1023, 0L)
        fonte.close()
        assertEquals(2, aberturas.get())
        assertEquals(emptyList<Long>(), esperas)
    }

    @Test fun closeDesconectaAberturaEmVoo() {
        val entrou = CountDownLatch(1)
        val saiu = CountDownLatch(1)
        val conexao = object : ConexaoFalsa(URL("https://example.invalid/video.mkv")) {
            override fun getResponseCode(): Int {
                entrou.countDown()
                while (!desconectou.get()) Thread.sleep(5)
                throw IOException("desconectada")
            }
        }
        val aberturas = AtomicInteger()
        val fonte = ParaleloDataSource(null, emptyMap(),
            novaConexao = { aberturas.incrementAndGet(); conexao },
            dormir = { throw AssertionError("cancelamento nao deve repetir") })
        val erro = AtomicReference<IOException?>()
        val t = Thread {
            try { fonte.conectarComRetry(conexao.url, 0, 1023) }
            catch (e: IOException) { erro.set(e) }
            finally { saiu.countDown() }
        }
        t.start()
        assertTrue(entrou.await(2, TimeUnit.SECONDS))

        fonte.close()

        assertTrue(saiu.await(2, TimeUnit.SECONDS))
        assertTrue(conexao.desconectou.get())
        assertEquals(1, aberturas.get())
        assertTrue("close deve sair como InterruptedIOException, recebeu ${erro.get()}", erro.get() is InterruptedIOException)
        // O finally libera `saiu` imediatamente antes de Thread.run retornar.
        t.join(200)
        assertFalse(t.isAlive)
    }
}
