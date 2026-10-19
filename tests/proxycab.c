// proxycab (fork): o player pede ao 127.0.0.1 e o proxy repete na fonte com
// o Referer do addon. A fonte de teste (proxycab_srv.py) da 403 sem ele.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/proxycab.h"
#include "../src/rede.h"

static char dados[300000];

static int pedir(const char *url, long ini, long fim, long *n, int *st) {
  int64_t tot = -1;
  char *b = rede_baixar_trecho64_final(url, "", ini, fim, n, &tot, st, NULL, NULL, NULL);
  int ok = b && *n == fim - ini + 1 && !memcmp(b, dados + ini, (size_t)*n) && tot == (int64_t)sizeof dados;
  free(b);
  return ok;
}

int main(int argc, char **argv) {
  char fonte[128], local[96];
  long n = 0; int st = 0, falhas = 0;
  FILE *f;
  assert(argc == 3);
  for (size_t i = 0; i < sizeof dados; i++) dados[i] = (char)(i * 7 + i / 251);
  f = fopen(argv[2], "wb"); fwrite(dados, 1, sizeof dados, f); fclose(f);
  snprintf(fonte, sizeof fonte, "http://127.0.0.1:%s/filme.mkv", argv[1]);
  // Regra de quando entra: so com cabecalho que o player nao manda.
  assert(!proxycab_precisa("") && !proxycab_precisa("User-Agent: X\nCookie: a=b"));
  assert(proxycab_precisa("Referer: https://addon.invalid/") && proxycab_precisa("User-Agent: X\nOrigin: y"));
  // Sem o proxy, a fonte recusa.
  { int64_t t; char *b = rede_baixar_trecho64_final(fonte, "", 0, 9, &n, &t, &st, NULL, NULL, NULL);
    free(b); if (st != 403) { printf("FALHA: sem Referer devia dar 403, deu %d\n", st); falhas++; } }
  assert(proxycab_url(fonte, "Referer: https://addon.invalid/", local, sizeof local));
  assert(proxycab_e_url(local));
  if (!pedir(local, 0, 1023, &n, &st)) { printf("FALHA: comeco (HTTP %d, %ld B)\n", st, n); falhas++; }
  if (!pedir(local, 123456, 223455, &n, &st)) { printf("FALHA: meio (HTTP %d, %ld B)\n", st, n); falhas++; }
  if (!pedir(local, sizeof dados - 100, sizeof dados - 1, &n, &st)) { printf("FALHA: fim (HTTP %d)\n", st); falhas++; }
  // Sessao velha nao serve mais.
  { char velho[96]; snprintf(velho, sizeof velho, "%s", local);
    proxycab_parar();
    { int64_t t; char *b = rede_baixar_trecho64_final(velho, "", 0, 9, &n, &t, &st, NULL, NULL, NULL);
      free(b); if (st != 404) { printf("FALHA: sessao parada devia dar 404, deu %d\n", st); falhas++; } } }
  printf(falhas ? "proxycab: %d falha(s)\n" : "proxycab OK\n", falhas);
  return falhas != 0;
}
