// Issue #512: anime split seasons must keep addon S/E for next-episode and Trakt
// scrobble must carry episode ids from the selected meta (TVDB/TMDB/Trakt).
#include "trakt.h"
#include "traktscrobble.h"
#include "metaep.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int SDL_GetTicks(void) { return 0; }
void marco(const char *m) { (void)m; }

static char *ler(const char *nome) {
  char path[200]; FILE *f; char *b; long n;
  snprintf(path, sizeof path, "tests/fixtures/%s", nome);
  f = fopen(path, "rb"); assert(f);
  fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET);
  b = malloc((size_t)n + 1); assert(fread(b, 1, (size_t)n, f) == (size_t)n);
  b[n] = 0; fclose(f); return b;
}

char *rede_baixar(const char *url, int s) {
  (void)s;
  if (strstr(url, "tt29422052")) return ler("kaiju_tvdb_s2.json");
  return NULL;
}
char *rede_baixar_com(const char *u, int s, const char *const *c) {
  (void)s; (void)c;
  if (strstr(u, "/sync/playback")) return strdup("[]");
  if (strstr(u, "/sync/last_activities")) {
    return strdup("{\"movies\":{\"watched_at\":\"2026-03-01T00:00:00.000Z\"}}");
  }
  if (strstr(u, "/progress/watched")) {
    return strdup("{\"aired\":13,\"completed\":12,\"next_episode\":{\"season\":1,\"number\":13,\"title\":\"x\"}}");
  }
  if (strstr(u, "/sync/history")) {
    return strdup("[{\"watched_at\":\"2026-03-01T10:00:00.000Z\",\"type\":\"episode\","
                  "\"episode\":{\"season\":1,\"number\":12,\"title\":\"finale\"},"
                  "\"show\":{\"title\":\"Kaiju\",\"ids\":{\"imdb\":\"tt29422052\"}}}]");
  }
  return NULL;
}
char *rede_apagar(const char *u, int s, const char *const *c, int *st) { (void)u; (void)s; (void)c; if (st) *st = 0; return NULL; }
void rede_avisar_401(void (*cb)(const char *)) { (void)cb; }
char *rede_baixar_st(const char *u, int s, const char *const *c, int *st) {
  (void)u; (void)s; (void)c;
  if (st) *st = 200;
  if (strstr(u, "/sync/watched/shows")) return strdup("[]");
  if (strstr(u, "/sync/watched/movies")) return strdup("[]");
  if (st) *st = 0;
  return NULL;
}
int cwo_conta_a_seguir(const char *id) { (void)id; return 0; }
void cwo_conta_trocar(const char *a, const char *b) { (void)a; (void)b; }
void cwo_marcar_estreia(const char *id, long long ms) { (void)id; (void)ms; }
int cwo_virada_aceita(long long e, long long a) { (void)e; (void)a; return 1; }
int ajustes_tmdb_cw(void) { return 0; }
const char *ajustes_tmdb_idioma(void) { return "en-US"; }
const char *desc_chave_tmdb(void) { return ""; }
const char *desc_tmdb_idioma(void) { return "en"; }
const char *i18n(const char *s) { return s; }
unsigned long long cat_historico_geracao(void) { return 1; }
int cat_historico_definir_se_geracao(const char *a, const char *b, int c, unsigned long long g) {
  (void)a; (void)b; (void)c; (void)g; return 1;
}

int main(void) {
  static CatItem v[4];
  char corpo[500];
  MetaEpInfo ep;
  ScrobbleEpIds ids = { "10600123", "5234501", "9923456" };
  int n;
  char *meta = ler("kaiju_tvdb_s2.json");

  assert(metaep_existe(meta, 2, 1));
  assert(!metaep_existe(meta, 1, 13));
  assert(metaep_proximo(meta, 1, 12, &ep));
  assert(ep.temporada == 2 && ep.episodio == 1);
  // Fixture stores episode ids as JSON numbers, not strings.
  assert(metaep_buscar(meta, 2, 1, &ep));
  assert(!strcmp(ep.tvdb, "10600123"));
  assert(!strcmp(ep.tmdb, "5234501"));
  assert(!strcmp(ep.trakt, "9923456"));
  metaep_scrobble_corpo(corpo, sizeof corpo, "tt29422052", 2, 1, &ep, 41.0);
  assert(strstr(corpo, "\"tvdb\":10600123"));
  assert(strstr(corpo, "\"tmdb\":5234501"));
  assert(strstr(corpo, "\"trakt\":9923456"));
  assert(strstr(corpo, "\"season\":2"));
  assert(strstr(corpo, "\"number\":1"));

  scrobble_corpo_ep(corpo, sizeof corpo, "tt29422052:2:1", 41.0, &ids);
  assert(strstr(corpo, "\"tvdb\":10600123"));
  assert(strstr(corpo, "\"season\":2"));
  assert(strstr(corpo, "\"number\":1"));

  trakt_definir("token", "client");
  n = trakt_continuar(v, 4);
  assert(n == 1);
  assert(v[0].temporada == 2 && v[0].episodio == 1);
  assert(!strcmp(v[0].imdb, "tt29422052:2:1"));

  free(meta);
  puts("trakt_season_meta: PASS");
  return 0;
}
