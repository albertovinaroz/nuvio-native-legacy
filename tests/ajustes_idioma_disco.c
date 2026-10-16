// #129: IDIOMA DE LEGENDA E AUDIO ESCOLHIDO NA TV SOBREVIVE AO ARRANQUE.
//
// O relato: "escolhi ingles para legenda e audio, saio dos ajustes e nao
// fica salvo; o filme seguinte nao liga a legenda em ingles". O ajustes.txt
// recebia "legendaIdioma 3" certinho — quem perdia era a LEITURA.
// ajustes_dir le o arquivo passando cada valor por limita(), que para as duas
// linhas de idioma consulta nValores() -> nLingua. E nLingua so era
// preenchido por rotulosDeIdioma(), chamado DEPOIS do laco de leitura: no
// arranque a lista tinha "1 valor", todo indice >= 1 era recusado como fora
// da faixa, e a legenda voltava a "Da conta" e o audio ao "Original".
//
// Este teste grava o arquivo como gravar() gravaria, le com ajustes_dir num
// processo que nunca abriu a tela de Ajustes (nLingua zerado, igual ao
// arranque) e confere o indice E o que chega em linguas.c, que e o que o
// player consulta (ling_legenda/ling_audio).
#include "../src/ajustes.c"
#include <assert.h>
#include <unistd.h>

// Volta o modulo ao estado de um processo que acabou de abrir: e o que separa
// um caso do seguinte (o arquivo e a unica coisa que atravessa um arranque).
static void reiniciarIdioma(void) {
  valor[AJ_IDIOMA] = 0;
  idiomaEfetivo = IDIOMA_EN;
  idiomaPosArranque = 0;
  idiomaUltimaFonte = -1;
  idiomaFonteGravada = IDA_PADRAO;
  sistemaLoc[0] = contaTmdbLing[0] = contaLegLing[0] = 0;
}
static void escrever(const char *caminho, const char *texto) {
  FILE *f = fopen(caminho, "w");
  assert(f);
  fputs(texto, f);
  fclose(f);
}
// O numero de uma linha "chave N" do arquivo, ou -1 se nao existe.
static int linhaDoArquivo(const char *caminho, const char *chave) {
  char linha[96], k[64];
  int v, achou = -1;
  FILE *f = fopen(caminho, "r");
  assert(f);
  while (fgets(linha, sizeof linha, f))
    if (sscanf(linha, "%63s %d", k, &v) == 2 && !strcmp(k, chave)) achou = v;
  fclose(f);
  return achou;
}

int main(void) {
  char dir[] = "/tmp/nuvio-aj-idioma-XXXXXX";
  char caminho[700];
  FILE *f;
  int en = -1, es = -1, i;
  assert(mkdtemp(dir));
  for (i = 0; i < ling_opcao_n(); i++) {
    if (!strcmp(ling_opcao_codigo(i), "en")) en = i;
    if (!strcmp(ling_opcao_codigo(i), "es")) es = i;
  }
  assert(en > 1 && es > 1);
  assert(nLingua == 0);   // o estado do arranque: a tela nunca abriu

  snprintf(caminho, sizeof caminho, "%s/ajustes.txt", dir);
  f = fopen(caminho, "w");
  assert(f);
  // No disco vai o indice de sempre; na memoria, a posicao na tela (fork).
  fprintf(f, "legendaIdioma %d\naudioIdioma %d\n", ling_opcao_para_disco(en), ling_opcao_para_disco(es));
  fclose(f);

  ajustes_dir(dir);
  assert(valor[AJ_LEG_LINGUA] == en);
  assert(valor[AJ_AUD_LINGUA] == es);
  assert(!strcmp(ling_legenda(), "en"));
  assert(!strcmp(ling_audio(), "es"));
  // Fork: as variantes moram depois do "~" no disco e ao lado da familia na tela.
  assert(ling_opcao_para_disco(LING_OPC_ORIGINAL) == 30);
  assert(!strcmp(ling_opcao_codigo(es + 1), "es-419"));
  assert(ling_opcao_para_disco(es + 1) == 31);
  for (i = 0; i < ling_opcao_n(); i++) assert(ling_opcao_de_disco(ling_opcao_para_disco(i)) == i);

  // A conta com outro idioma nao desfaz a escolha desta TV (linguas.c: a
  // local ganha). E o blob da linha "[ajustes] idiomas da conta" do relato.
  ajustes_aplicar_blob("{\"features\":{\"player_settings\":{"
    "\"subtitle_preferred_language\":{\"type\":\"string\",\"value\":\"pt-br\"},"
    "\"preferred_audio_language\":{\"type\":\"string\",\"value\":\"\"}}}}");
  assert(valor[AJ_LEG_LINGUA] == en);
  assert(valor[AJ_AUD_LINGUA] == es);
  assert(!strcmp(ling_legenda(), "en"));
  assert(!strcmp(ling_audio(), "es"));

  // ABRIR A TELA DE AJUSTES nao zera a escolha (#129, a causa de campo):
  // conferirPadroes comparava o idioma contra o n=2 da tabela e voltava tudo
  // a "Da conta" — "padrao fora da lista em 4 (\"Idioma do áudio\")".
  ajustes_iniciar();
  assert(valor[AJ_LEG_LINGUA] == en);
  assert(valor[AJ_AUD_LINGUA] == es);
  ajustes_iniciar();
  assert(valor[AJ_LEG_LINGUA] == en);

  // O que ficou em disco continua sendo a escolha, para o proximo arranque.
  { char linha[96]; int achouLeg = 0;
    f = fopen(caminho, "r");
    assert(f);
    while (fgets(linha, sizeof linha, f)) {
      int v;
      if (sscanf(linha, "legendaIdioma %d", &v) == 1) { assert(v == ling_opcao_para_disco(en)); achouLeg = 1; }
    }
    fclose(f);
    assert(achouLeg); }

  // ---- IDIOMA AUTOMATICO DA INTERFACE -------------------------------------
  // Cada caso parte de um arquivo escrito a mao, como o de uma versao anterior
  // ou o de uma TV nova, e le com ajustes_dir num estado limpo.
  unlink(caminho);

  // (1) TV nova: nenhum arquivo -> automatico, e sem conta nem TV, ingles.
  reiniciarIdioma();
  ajustes_dir(dir);
  assert(valor[AJ_IDIOMA] == 0 && ajustes_idioma() == IDIOMA_EN);

  // (2) ARQUIVO DE ANTES DA MARCA: "idioma 0" (ou 1) sem idiomaAutoLocal e
  // escolha manual, nao importa o que a conta diga depois.
  escrever(caminho, "idioma 0\n");
  reiniciarIdioma();
  ajustes_dir(dir);
  assert(valor[AJ_IDIOMA] == 1 && ajustes_idioma() == IDIOMA_PT);
  ajustes_aplicar_blob("{\"features\":{\"tmdb_settings\":{"
    "\"tmdb_language\":{\"type\":\"string\",\"value\":\"ro-RO\"}}}}");
  assert(ajustes_idioma() == IDIOMA_PT);          // a conta nao desfaz a escolha
  assert(valor[AJ_IDIOMA] == 1);
  escrever(caminho, "idioma 1\n");
  reiniciarIdioma();
  ajustes_dir(dir);
  assert(valor[AJ_IDIOMA] == 2 && ajustes_idioma() == IDIOMA_EN);
  escrever(caminho, "idioma 7\nidiomaAutoLocal 0\n");
  reiniciarIdioma();
  ajustes_dir(dir);
  assert(valor[AJ_IDIOMA] == 8 && ajustes_idioma() == IDIOMA_ES);   // marca 0 = manual

  // (3) AUTOMATICO: o gravado e o ultimo resolvido (a TV abre nele), e a
  // conta corrige quando chega. Ordem: tmdb_language, depois legenda.
  escrever(caminho, "idioma 2\nidiomaAutoLocal 1\nidiomaFonteLocal 3\n");
  reiniciarIdioma();
  ajustes_dir(dir);
  assert(valor[AJ_IDIOMA] == 0 && ajustes_idioma() == IDIOMA_RO);
  ajustes_aplicar_blob("{\"features\":{\"tmdb_settings\":{"
    "\"tmdb_language\":{\"type\":\"string\",\"value\":\"pt-BR\"}}}}");
  assert(ajustes_idioma() == IDIOMA_PT);
  assert(valor[AJ_IDIOMA] == 0);                  // continua automatico
  assert(!strcmp(contaTmdbLing, "pt-BR"));
  // Em disco: idioma em vigor, marca de automatico ligada e a fonte (tmdb = 0).
  assert(linhaDoArquivo(caminho, "idioma") == IDIOMA_PT);
  assert(linhaDoArquivo(caminho, "idiomaAutoLocal") == 1);
  assert(linhaDoArquivo(caminho, "idiomaFonteLocal") == IDA_TMDB);
  // O mesmo blob de novo nao muda nada (sem laco de re-resolucao).
  ajustes_aplicar_blob("{\"features\":{\"tmdb_settings\":{"
    "\"tmdb_language\":{\"type\":\"string\",\"value\":\"pt-BR\"}}}}");
  assert(ajustes_idioma() == IDIOMA_PT);
  // Sem tmdb_language (nulo), o idioma de legenda decide.
  ajustes_aplicar_blob("{\"features\":{\"tmdb_settings\":{"
    "\"tmdb_language\":{\"type\":\"string\",\"value\":null}},"
    "\"player_settings\":{\"subtitle_preferred_language\":"
    "{\"type\":\"string\",\"value\":\"ukr\"}}}}");
  assert(ajustes_idioma() == IDIOMA_UK);
  assert(linhaDoArquivo(caminho, "idiomaFonteLocal") == IDA_LEGENDA);
  // Sem nenhum dos dois, cai no locale da TV e depois no ingles.
  strcpy(sistemaLoc, "fr-FR");
  ajustes_aplicar_blob("{\"features\":{\"player_settings\":{"
    "\"subtitle_preferred_language\":{\"type\":\"string\",\"value\":\"none\"}}}}");
  assert(ajustes_idioma() == IDIOMA_FR);
  assert(linhaDoArquivo(caminho, "idiomaFonteLocal") == IDA_SISTEMA);
  sistemaLoc[0] = 0;
  ajustes_aplicar_blob("{\"features\":{\"player_settings\":{"
    "\"subtitle_preferred_language\":{\"type\":\"string\",\"value\":\"\"}}}}");
  assert(ajustes_idioma() == IDIOMA_EN);

  // (4) Que veio da CONTA no arranque anterior: no proximo, ajustes_dir + o
  // ultimo resolvido, e o locale da TV nao o derruba antes de o blob chegar.
  escrever(caminho, "idioma 3\nidiomaAutoLocal 1\nidiomaFonteLocal 1\n");
  reiniciarIdioma();
  ajustes_dir(dir);
  setenv("NUVIO_LOCALE", "en-US", 1);
  ajustes_idioma_auto_iniciar(NULL);
  assert(ajustes_idioma() == IDIOMA_UK);          // da legenda da conta, guardado
  assert(idiomaFonteGravada == IDA_LEGENDA);
  // Ja o que veio da TV e refeito no arranque (a TV pode ter mudado de lingua).
  escrever(caminho, "idioma 3\nidiomaAutoLocal 1\nidiomaFonteLocal 2\n");
  reiniciarIdioma();
  ajustes_dir(dir);
  setenv("NUVIO_LOCALE", "de-DE", 1);
  ajustes_idioma_auto_iniciar(NULL);
  assert(ajustes_idioma() == IDIOMA_DE);

  // (5) A ESCOLHA MANUAL GANHA E DESLIGA O AUTOMATICO; "Automático" religa.
  escrever(caminho, "idioma 2\nidiomaAutoLocal 1\nidiomaFonteLocal 0\n");
  reiniciarIdioma();
  ajustes_dir(dir);
  strcpy(contaTmdbLing, "es-419");
  idiomaResolver(0);
  assert(ajustes_idioma() == IDIOMA_ES);
  valor[AJ_IDIOMA] = 1 + IDIOMA_DE;               // a pessoa escolheu Deutsch
  idiomaEscolhido();
  gravar();
  assert(ajustes_idioma() == IDIOMA_DE);
  assert(linhaDoArquivo(caminho, "idioma") == IDIOMA_DE);
  assert(linhaDoArquivo(caminho, "idiomaAutoLocal") == 0);
  ajustes_aplicar_blob("{\"features\":{\"tmdb_settings\":{"
    "\"tmdb_language\":{\"type\":\"string\",\"value\":\"fr\"}}}}");
  assert(ajustes_idioma() == IDIOMA_DE);          // a conta nao mexe mais
  reiniciarIdioma();
  ajustes_dir(dir);                               // e sobrevive ao reinicio
  assert(valor[AJ_IDIOMA] == 1 + IDIOMA_DE && ajustes_idioma() == IDIOMA_DE);
  valor[AJ_IDIOMA] = 0;                           // escolheu "Automático" de novo
  strcpy(contaTmdbLing, "fr");
  idiomaEscolhido();
  gravar();
  assert(ajustes_idioma() == IDIOMA_FR);          // resolve na hora
  assert(linhaDoArquivo(caminho, "idiomaAutoLocal") == 1);
  // A escolha manual atravessa o blob de layout da conta sem ser reescrita:
  // "idioma" e local e nunca sobe.
  assert(somenteDesteAparelho(AJ_IDIOMA));

  // (6) OS 22 DE 2026-09. O numero gravado no disco e o IDIOMA_* (os antigos
  // nao mudam de lugar), e a escolha manual de qualquer um sobrevive ao reinicio.
  escrever(caminho, "idioma 27\nidiomaAutoLocal 0\n");
  reiniciarIdioma();
  ajustes_dir(dir);
  assert(valor[AJ_IDIOMA] == 1 + IDIOMA_JA && ajustes_idioma() == IDIOMA_JA);
  escrever(caminho, "idioma 29\nidiomaAutoLocal 0\n");
  reiniciarIdioma();
  ajustes_dir(dir);
  assert(ajustes_idioma() == IDIOMA_ZHTW);
  escrever(caminho, "idioma 30\nidiomaAutoLocal 0\n");       // fora da faixa: nao inventa idioma
  reiniciarIdioma();
  ajustes_dir(dir);
  assert(ajustes_idioma() == IDIOMA_EN);
  // Automatico com a conta em portugues de Portugal, chines tradicional e norueguês.
  escrever(caminho, "");
  reiniciarIdioma();
  ajustes_dir(dir);
  strcpy(contaTmdbLing, "pt-PT");   idiomaResolver(0);   assert(ajustes_idioma() == IDIOMA_PTPT);
  strcpy(contaTmdbLing, "zh-Hant"); idiomaResolver(0);   assert(ajustes_idioma() == IDIOMA_ZHTW);
  strcpy(contaTmdbLing, "zh");      idiomaResolver(0);   assert(ajustes_idioma() == IDIOMA_ZHCN);
  strcpy(contaTmdbLing, "nb");      idiomaResolver(0);   assert(ajustes_idioma() == IDIOMA_NO);
  strcpy(contaTmdbLing, "");        strcpy(contaLegLing, "gre"); idiomaResolver(0);
  assert(ajustes_idioma() == IDIOMA_EL);
  // "Da interface" no idioma dos metadados: o codigo que o TMDB entende, um por
  // idioma (o bokmal e "nb", nao "no"; o portugues do Brasil e o europeu diferem).
  { static const char *const TMDB[IDIOMA_N] = {
      "pt-BR", "en-US", "ro-RO", "uk-UA", "ru-RU", "fr-FR", "de-DE", "es-ES", "it-IT",
      "nl-NL", "pl-PL", "tr-TR", "pt-PT", "sv-SE", "da-DK", "nb-NO", "cs-CZ", "sk-SK",
      "sl-SI", "hu-HU", "lt-LT", "bs-BA", "sr-RS", "bg-BG", "el-GR", "id-ID", "vi-VN",
      "ja-JP", "zh-CN", "zh-TW" };
    int i;
    valor[AJ_TMDB_IDIOMA] = 0;
    for (i = 0; i < IDIOMA_N; i++) {
      valor[AJ_IDIOMA] = 1 + i;
      assert(ajustes_idioma() == i);
      assert(!strcmp(ajustes_tmdb_idioma(), TMDB[i]));
    } }
  // A lista do "Idioma dos metadados" cresceu no fim: os 14 indices antigos
  // continuam apontando para os mesmos codigos.
  valor[AJ_IDIOMA] = 1 + IDIOMA_EN;
  valor[AJ_TMDB_IDIOMA] = 1;  assert(!strcmp(ajustes_tmdb_idioma(), "pt-BR"));
  valor[AJ_TMDB_IDIOMA] = 7;  assert(!strcmp(ajustes_tmdb_idioma(), "pt-PT"));
  valor[AJ_TMDB_IDIOMA] = 13; assert(!strcmp(ajustes_tmdb_idioma(), "ru-RU"));
  valor[AJ_TMDB_IDIOMA] = 31; assert(!strcmp(ajustes_tmdb_idioma(), "zh-TW"));
  valor[AJ_TMDB_IDIOMA] = 0;

  unlink(caminho);
  { char tmp[700]; snprintf(tmp, sizeof tmp, "%s/ajustes.tmp", dir); unlink(tmp); }
  rmdir(dir);
  puts("ajustes_idioma_disco: ok");
  return 0;
}
