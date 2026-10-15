// Idiomas de audio e de legenda da fonte (as bandeiras da folha): o parser de
// verdade sobre os formatos que os addons mandam. O primeiro caso e o do app
// Nuvio no iOS ("Languages 🇪🇸 • 🇬🇧" / "Subtitles: 🇪🇸 • 🇬🇧").
#include "streams.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int falhas;
static void junta(char lista[][6], int n, char *dst, size_t tam) {
  size_t k = 0;
  dst[0] = 0;
  for (int i = 0; i < n && k < tam; i++)
    k += (size_t)snprintf(dst + k, tam - k, "%s%s", i ? "," : "", lista[i]);
}
static void caso(Stream *s, const char *audio, const char *legenda, const char *nome) {
  char a[64], l[64];
  junta(s->idiomasAudio, s->nIdiomasAudio, a, sizeof a);
  junta(s->idiomasLegenda, s->nIdiomasLegenda, l, sizeof l);
  if (strcmp(a, audio) || strcmp(l, legenda)) {
    printf("FAIL %s: audio=[%s] (quer [%s]) legenda=[%s] (quer [%s])\n", nome, a, audio, l, legenda);
    falhas++;
  } else printf("ok   %s\n", nome);
}
#define ES "\xF0\x9F\x87\xAA\xF0\x9F\x87\xB8"
#define GB "\xF0\x9F\x87\xAC\xF0\x9F\x87\xA7"
#define RU "\xF0\x9F\x87\xB7\xF0\x9F\x87\xBA"
#define MX "\xF0\x9F\x87\xB2\xF0\x9F\x87\xBD"
#define PONTO "\xE2\x80\xA2"
#define RAIO "\xE2\x9A\xA1"
#define FALA "\xF0\x9F\x92\xAC"
#define GLOBO "\xF0\x9F\x8C\x8E"
#define PESSOA "\xF0\x9F\x91\xA4"
int main(void) {
  Stream *v = NULL;
  int n = stream_extrair("{\"streams\":["
    "{\"url\":\"https://example.invalid/0\",\"name\":\"4K " PONTO " " RAIO " Instant\","
      "\"description\":\"WEB-DL\\nSize:  16.4 GB\\nDuration: 1h:48m:11s\\nLanguages " ES " " PONTO " " GB
      "\\nSubtitles: " ES " " PONTO " " GB "\"},"
    "{\"url\":\"https://example.invalid/1\",\"name\":\"4K\","
      "\"description\":\"Languages " GB " " PONTO " " RU "\\nSubtitles: " ES " " PONTO " " GB "\"},"
    "{\"url\":\"https://example.invalid/2\",\"name\":\"Torrentio\\n4k\","
      "\"title\":\"Animals 2025 2160p WEB-DL\\n" PESSOA " 12 \\nMulti Audio / " ES " / " GB "\"},"
    "{\"url\":\"https://example.invalid/3\",\"name\":\"AIO\",\"description\":\"" GLOBO " English | Spanish\"},"
    "{\"url\":\"https://example.invalid/4\",\"name\":\"x\","
      "\"description\":\"The.French.Connection.1971.1080p.BluRay.ITA.ENG.x264\"},"
    "{\"url\":\"https://example.invalid/5\",\"name\":\"x\",\"description\":\"Movie 2020 1080p SPA ENG SUBS\"},"
    "{\"url\":\"https://example.invalid/6\",\"name\":\"x\",\"description\":\"Audio: Spanish Latino\"},"
    "{\"url\":\"https://example.invalid/7\",\"name\":\"x\",\"description\":\"Castellano | Latino | Sub Esp\"},"
    "{\"url\":\"https://example.invalid/8\",\"name\":\"x\",\"description\":\"Recomendado por Dan, pizza ita\"},"
    "{\"url\":\"https://example.invalid/9\",\"name\":\"x\",\"subtitles\":["
      "{\"id\":\"1\",\"url\":\"https://example.invalid/s1\",\"lang\":\"spa\"},"
      "{\"id\":\"2\",\"url\":\"https://example.invalid/s2\",\"lang\":\"pt-BR\"},"
      "{\"id\":\"3\",\"url\":\"https://example.invalid/s3\",\"lang\":\"klingon\"}]},"
    "{\"url\":\"https://example.invalid/10\",\"name\":\"x\",\"description\":\"Movie 2021 1080p MULTi multi-subs\"},"
    "{\"url\":\"https://example.invalid/11\",\"name\":\"x\",\"description\":\"Filme 2019 720p PT-BR\"},"
    "{\"url\":\"https://example.invalid/12\",\"name\":\"x\",\"description\":\"" FALA " English, Spanish\"},"
    "{\"url\":\"https://example.invalid/13\",\"name\":\"x\",\"description\":\"Pelicula 2018 1080p VOSE\"},"
    "{\"url\":\"https://example.invalid/14\",\"name\":\"x\",\"description\":\"" MX " Latino\"},"
    "{\"url\":\"https://example.invalid/15\",\"name\":\"The Italian Job\",\"description\":\"1080p WEB-DL\"},"
    "{\"url\":\"https://example.invalid/16\",\"name\":\"x\",\"description\":\"Animals.2025.1080p.WEB-DL.DUAL.CAST.LAT.ENG.SUBS.x264\"},"
    // Peerflix (addon espanhol): abreviaturas capitalizadas, "Subt Forz-Esp-Ing".
    "{\"url\":\"https://example.invalid/17\",\"name\":\"[TB+] Peerflix " ES " 1080p\","
      "\"title\":\"Animales Lat-Cast-Ing Subt Forz-Esp-Ing H264 E-AC3 5.1 BDRip Hd 1080p\\n"
      "Animales Lat-Cast-Ing Subt Forz-Esp-Ing H264 E-AC3 5.1 BDRip Hd 1080p.mkv\"},"
    // Campos estruturados lidos pelo #487 (upstream): vem primeiro.
    "{\"url\":\"https://example.invalid/18\",\"name\":\"AIO 1080p\",\"description\":\"WEB-DL\","
      "\"streamData\":{\"parsedFile\":{\"languages\":[\"English\",\"Spanish\"],\"subtitles\":[\"Spanish\",\"French\"]}}},"
    "{\"url\":\"https://example.invalid/19\",\"name\":\"x 1080p\",\"audioLanguages\":[\"en\",\"es-419\"],\"subtitleLanguages\":[\"pt-BR\"]},"
    "{\"url\":\"https://example.invalid/20\",\"name\":\"x 1080p\","
      "\"tracks\":{\"audio\":[{\"language\":\"jpn\"},{\"language\":\"eng\"}],\"subtitles\":[{\"lang\":\"spa\"}]}},"
    "{\"url\":\"https://example.invalid/21\",\"name\":\"x 1080p\",\"description\":\"Audio: Malayalam | Subtitles: Malay\"},"
    // "Cast:" e o elenco, nao castelhano; "The Cast Away" sem marco e titulo.
    "{\"url\":\"https://example.invalid/22\",\"name\":\"The Cast Away\",\"description\":\"Cast: Tom Hanks\"},"
    // Peerflix: nomes por extenso antes da resolucao, mas em LISTA.
    "{\"url\":\"https://example.invalid/23\",\"name\":\"x\","
      "\"title\":\"Animales Castellano-Ingles-Subt-Castellano-Forz-Comp H264 E-AC3 5.1 BDRip HD 1080p\"}"
    "]}", "fixture", &v);
  if (n != 24) { printf("FAIL contagem %d\n", n); return 1; }
  caso(&v[0], "es,en", "es,en", "iOS: Languages / Subtitles com bandeiras");
  caso(&v[1], "en,ru", "es,en", "iOS: audio e legenda diferentes");
  caso(&v[2], "multi,es,en", "", "Torrentio: Multi Audio / bandeiras no title");
  caso(&v[3], "en,es", "", "AIOStreams: nomes por extenso");
  caso(&v[4], "it,en", "", "release: titulo antes do ano nao conta");
  caso(&v[5], "es", "en", "release: ENG SUBS e legenda");
  caso(&v[6], "lat", "", "Spanish Latino e um idioma so");
  caso(&v[7], "es,lat", "es", "Castellano | Latino | Sub Esp");
  caso(&v[8], "", "", "minuscula e palavra, nao sigla");
  caso(&v[9], "", "es,ptbr", "array subtitles do Stremio");
  caso(&v[10], "multi", "multi", "MULTi e multi-subs");
  caso(&v[11], "ptbr", "", "PT-BR");
  caso(&v[12], "", "en,es", "emoji de fala abre legenda");
  caso(&v[13], "", "es", "VOSE");
  caso(&v[14], "lat", "", "bandeira do Mexico + Latino");
  caso(&v[15], "", "", "titulo no name sem marco nao vira idioma");
  caso(&v[16], "es,lat", "en", "CAST.LAT sao duas faixas");
  caso(&v[17], "es,lat,en", "es,en", "Peerflix: Lat-Cast-Ing Subt Forz-Esp-Ing");
  caso(&v[18], "en,es", "es,fr", "#487: streamData.parsedFile do AIOStreams");
  caso(&v[19], "en,lat", "ptbr", "#487: audioLanguages/subtitleLanguages");
  caso(&v[20], "ja,en", "es", "#487: tracks");
  caso(&v[21], "ml", "ms", "#487: Malayalam e Malay distintos");
  caso(&v[22], "", "", "Cast: (elenco) e titulo nao viram idioma");
  caso(&v[23], "es,en", "es", "Peerflix: Castellano-Ingles-Subt-Castellano-Forz-Comp");
  free(v);
  printf(falhas ? "%d falha(s)\n" : "tudo ok\n", falhas);
  return falhas != 0;
}
