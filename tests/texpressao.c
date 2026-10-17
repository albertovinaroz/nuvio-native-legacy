// PRESSAO DE MEMORIA DO SISTEMA no cache de texturas (Android onTrimMemory).
//
// POR QUE ESTE TESTE EXISTE: um upload de textura chegou a levar 16 s na TV
// Android com o low-memory killer ativo. A resposta e baixar o teto por um
// tempo e despejar arte FRIA — nunca a que esta na tela. Nenhuma tela mostra "o
// aviso foi ouvido"; a regra so se prova aqui, sem contexto GL (tex fica 0).
#include "../src/tex_cache.c"
#include <assert.h>
#include <stdio.h>

static void limpar(void) {
  memset(itens, 0, sizeof itens);
  nMax = 8; quadroAtual = 100; bytesUsados = 0;
  tex_despejos = 0; tex_despejos_quentes = 0;
  pressaoPct = 100; pressaoAte = 0; pressaoPlay = 100;
  orcMemTotal = 0;
}
// 1024x10: bytesTextura sem piramide, conta redonda.
static void pronto(int i, unsigned long uso, unsigned long quadro) {
  itens[i].estado = PRONTO; itens[i].uso = uso; itens[i].ultimoQuadro = quadro;
  itens[i].w = 1024; itens[i].h = 10;
  snprintf(itens[i].caminho, sizeof itens[i].caminho, "a%d", i);
  bytesUsados += bytesTextura(1024, 10);
}

int main(void) {
  long um = bytesTextura(1024, 10);
  int i;
  mtx = SDL_CreateMutex();   // tex_novo_quadro sai sem ele (cache nao iniciado)

  // 1. A tabela de niveis do Android.
  assert(tex_pressao_pct(0) == 100 && tex_pressao_pct(4) == 100);
  assert(tex_pressao_pct(5) == 80 && tex_pressao_pct(10) == 60 && tex_pressao_pct(15) == 40);
  assert(tex_pressao_pct(20) == 50 && tex_pressao_pct(40) == 50);
  assert(tex_pressao_pct(60) == 25 && tex_pressao_pct(80) == 25);
  puts("ok  tabela de niveis");

  // 2. Sem aviso, nada muda: cache cheio no teto continua no teto.
  limpar();
  orcamento = um * 8;
  for (i = 0; i < 8; i++) pronto(i, (unsigned long)i + 1, 50);   // todos frios
  podar();
  assert(tex_despejos == 0 && bytesUsados == um * 8);
  puts("ok  sem aviso nao despeja");

  // 3. Aviso CRITICO (15 = 40%): o proximo quadro despeja frios ate caber.
  limpar();
  orcamento = um * 10;
  for (i = 0; i < 8; i++) pronto(i, (unsigned long)i + 1, 50);
  tex_pressao_memoria(15);
  assert(pressaoPct == 40);
  tex_novo_quadro();
  assert(bytesUsados <= um * 4);              // 40% de 10 = 4
  assert(tex_despejos == 4);
  assert(itens[0].estado == VAZIO && itens[7].estado == PRONTO);   // LRU: sai o de menor uso
  puts("ok  aviso critico despeja frios no quadro seguinte");

  // 4. NUNCA despeja arte quente (desenhada neste quadro), nem sob aviso.
  limpar();
  orcamento = um * 10;
  for (i = 0; i < 6; i++) pronto(i, (unsigned long)i + 1, 100);  // todos quentes
  tex_pressao_memoria(80);
  tex_novo_quadro();
  assert(tex_despejos == 0 && bytesUsados == um * 6);
  puts("ok  arte quente fica, mesmo sob aviso");

  // 5. Aviso mais fraco depois de um forte nao afrouxa.
  limpar();
  orcamento = um * 10;
  tex_pressao_memoria(15);
  tex_pressao_memoria(5);
  assert(pressaoPct == 40);
  puts("ok  aviso fraco nao afrouxa o forte");

  // 6. O teto volta sozinho passado o prazo.
  limpar();
  orcamento = um * 10;
  tex_pressao_memoria(15);
  pressaoAte = SDL_GetTicks() - 1;           // prazo vencido
  assert(!pressaoAtiva() && pressaoPct == 100);
  assert(limiteBytes() == orcamento);
  puts("ok  teto volta ao normal sozinho");

  // 7. Nivel sem efeito (< 5) nao liga nada.
  limpar();
  tex_pressao_memoria(0);
  assert(pressaoPct == 100 && !pressaoAtiva());
  puts("ok  nivel baixo nao liga a pressao");

  // 8. REPRODUCAO (tex_pressao_reproducao): so Android liga (aqui o define
  //    nao existe, entao o teto e 100%% e nada muda) — a regra do C que vale
  //    em qualquer plataforma e que fechar o video devolve o teto inteiro.
  limpar();
  orcamento = um * 10;
  orcMemTotal = 2400;                       // a TCL do incidente
#ifdef NV_ANDROID
  assert(tex_pressao_play_pct() == 40);     // < 3 GB
  assert((orcMemTotal = 4096, tex_pressao_play_pct()) == 75);
  assert((orcMemTotal = 2048, tex_pressao_play_pct()) == 40);
  orcMemTotal = 2400;
  tex_pressao_reproducao(1);
  assert(pressaoPlay == 40);
  for (i = 0; i < 8; i++) pronto(i, (unsigned long)i + 1, 50);
  tex_novo_quadro();
  assert(bytesUsados <= um * 4);            // 40%% de 10
  assert(tex_despejos == 4);
  tex_pressao_reproducao(0);                // fechou o video: teto volta
  assert(pressaoPlay == 100 && limiteBytes() == orcamento);
#else
  assert(tex_pressao_play_pct() == 100);    // fora do Android nunca aperta
  tex_pressao_reproducao(1);
  assert(pressaoPlay == 100 && limiteBytes() == orcamento);
#endif
  puts("ok  reproducao aperta o teto no Android e devolve ao fechar");

  puts("PASS: texpressao");
  return 0;
}
