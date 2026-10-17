// #512: numeric episode ids from addon meta must survive into the scrobble body.
// idsScrobble used to point at a stack MetaEpInfo that dies on return.
#include "traktscrobble.h"
#include "metaep.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *ler(const char *nome) {
  char path[200]; FILE *f; char *b; long n;
  snprintf(path, sizeof path, "tests/fixtures/%s", nome);
  f = fopen(path, "rb"); assert(f);
  fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET);
  b = malloc((size_t)n + 1); assert(fread(b, 1, (size_t)n, f) == (size_t)n);
  b[n] = 0; fclose(f); return b;
}

static void ids_do_meta(const char *meta, int t, int e, ScrobbleEpIds *out) {
  MetaEpInfo ep;
  memset(out, 0, sizeof *out);
  assert(metaep_buscar(meta, t, e, &ep));
  scrobble_ids_guardar(out, ep.tvdb, ep.tmdb, ep.trakt);
}

int main(void) {
  char corpo[500];
  ScrobbleEpIds ids;
  char *meta = ler("kaiju_tvdb_s2.json");
  char poison[sizeof(MetaEpInfo)];

  ids_do_meta(meta, 2, 1, &ids);
  memset(poison, 'Z', sizeof poison);
  assert(ids.tvdb && !strcmp(ids.tvdb, "10600123"));
  assert(ids.tmdb && !strcmp(ids.tmdb, "5234501"));
  assert(ids.trakt && !strcmp(ids.trakt, "9923456"));
  scrobble_corpo_ep(corpo, sizeof corpo, "tt29422052:2:1", 41.0, &ids);
  assert(strstr(corpo, "\"tvdb\":10600123"));
  assert(strstr(corpo, "\"tmdb\":5234501"));
  assert(strstr(corpo, "\"trakt\":9923456"));
  assert(strstr(corpo, "\"season\":2"));
  assert(strstr(corpo, "\"number\":1"));
  scrobble_ids_limpar(&ids);

  free(meta);
  puts("trakt_scrobble_ids: PASS");
  return 0;
}
