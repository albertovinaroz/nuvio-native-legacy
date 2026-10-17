#include "visto.h"
#include "perfisessao.h"
#include "trakt.h"
#include "simkl.h"
#include "contapend.h"
#include "sessao.h"
#include "ajustes.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Optional imports belong to consumers: public declarations keep producers strong.
extern int trakt_destino_capturar(TraktDestino *) NV_PERFIL_OPCIONAL;
extern int trakt_episodios_marcar_destino(const TraktDestino *, const char *, const VistoPar *, int, int) NV_PERFIL_OPCIONAL;
extern int simkl_destino_capturar(SimklDestino *) NV_PERFIL_OPCIONAL;
extern int simkl_episodios_marcar_destino(const SimklDestino *, const char *, const VistoPar *, int, int) NV_PERFIL_OPCIONAL;
extern int simkl_titulo_marcar_destino(const SimklDestino *, const char *, const char *, const int *, int, int) NV_PERFIL_OPCIONAL;

// Teto do lote: o mesmo do Simkl, que e o menor dos tres (Trakt aceita 256
// desde este mesmo conserto; a conta nao tem teto no cliente).
#define VT_LOTE SMK_LOTE_MAX
#define VT_TEMPS 64

int visto_destinos(void) {
  return (trakt_ativo() ? VISTO_TRAKT : 0) |
         (simkl_ativo() ? VISTO_SIMKL : 0) |
         // "Enviar histórico para a conta Nuvio" (por perfil): desligado, o visto
         // deste perfil so vai para o Trakt e o Simkl que ele tiver vinculados.
         (sessao_logada() && ajustes_hist_conta() ? VISTO_CONTA : 0);
}

int visto_episodios_ja(const char *imdb, const char *tipo, const VistoPar *pares,
                       int n, int visto, int destinos) {
  int ok = 1;
  if (!imdb || !imdb[0] || !pares || n < 1) return 0;
  if (n > VT_LOTE) n = VT_LOTE;
  // Ordem: Trakt, Simkl, conta. Nenhum depende do outro; a falha de um nao
  // impede os seguintes (cada um diz no log o que aconteceu).
  if (destinos & VISTO_TRAKT) ok &= trakt_episodios_marcar(imdb, pares, n, visto) ? 1 : 0;
  if (destinos & VISTO_SIMKL) ok &= simkl_episodios_marcar(imdb, pares, n, visto) ? 1 : 0;
  // A CONTA PASSA PELO JORNAL (contapend.c): o gesto fica em disco antes da
  // rede e so sai de la com 2xx. Era uma RPC solta — sem rede, a marca se
  // perdia. Quem ja registrou (visto_episodios) manda VISTO_CONTA_JORNAL.
  if (destinos & VISTO_CONTA) {
    if (!(destinos & VISTO_CONTA_JORNAL)) contapend_episodios(imdb, tipo, pares, n, visto);
    ok &= contapend_enviar() >= 0 ? 1 : 0;
  }
  return ok;
}

int visto_titulo_ja(const char *imdb, const char *tipo, const int *temporadas,
                    int nt, int visto, int destinos) {
  int ok = 1;
  if (!imdb || !imdb[0]) return 0;
  if (destinos & VISTO_SIMKL) ok &= simkl_titulo_marcar(imdb, tipo, temporadas, nt, visto) ? 1 : 0;
  if (destinos & VISTO_CONTA) {
    if (!(destinos & VISTO_CONTA_JORNAL)) contapend_titulo(imdb, tipo, visto);
    ok &= contapend_enviar() >= 0 ? 1 : 0;
  }
  return ok;
}

// A COPIA E DO FIO: quem chamou pode ter o CatItem trocado de bloco pela
// descoberta antes de o fio ler (mesma razao do Envio de episodios.c).
typedef struct {
  char imdb[24], tipo[12];
  VistoPar pares[VT_LOTE];
  int temps[VT_TEMPS];
  int n, nt, visto, destinos, titulo;
  NvPerfilSessao sessao; TraktDestino trakt; SimklDestino simkl;
} Envio;

static void *fio(void *u) {
  Envio *e = (Envio *)u;
  // The UI/journal already received the gesture before this worker started.
  // Remote destinations own copied authentication, never the active profile.
  if (e->destinos & VISTO_TRAKT) {
    if (trakt_episodios_marcar_destino) trakt_episodios_marcar_destino(&e->trakt, e->imdb, e->pares, e->n, e->visto);
    else trakt_episodios_marcar(e->imdb, e->pares, e->n, e->visto);
  }
  if (e->destinos & VISTO_SIMKL) {
    if (e->titulo) {
      if (simkl_titulo_marcar_destino) simkl_titulo_marcar_destino(&e->simkl, e->imdb, e->tipo, e->temps, e->nt, e->visto);
      else simkl_titulo_marcar(e->imdb, e->tipo, e->temps, e->nt, e->visto);
    } else {
      if (simkl_episodios_marcar_destino) simkl_episodios_marcar_destino(&e->simkl, e->imdb, e->pares, e->n, e->visto);
      else simkl_episodios_marcar(e->imdb, e->pares, e->n, e->visto);
    }
  }
  if (e->destinos & VISTO_CONTA) contapend_enviar(); // each journal record owns its account/profile
  memset(e, 0, sizeof *e);
  free(e);
  return NULL;
}

static int lancar(Envio *e) {
  pthread_t t;
  if (pthread_create(&t, NULL, fio, e) != 0) { free(e); return 0; }
  pthread_detach(t);
  return 1;
}

static void capturarDestinos(Envio *e) { // caller holds the profile opening gate
  if ((e->destinos & VISTO_TRAKT) && trakt_destino_capturar && !trakt_destino_capturar(&e->trakt)) e->destinos &= ~VISTO_TRAKT;
  if ((e->destinos & VISTO_SIMKL) && simkl_destino_capturar && !simkl_destino_capturar(&e->simkl)) e->destinos &= ~VISTO_SIMKL;
}
int visto_episodios(const char *imdb, const char *tipo, const VistoPar *pares,
                    int n, int visto, int destinos) {
  if (!imdb || !imdb[0] || !pares || n < 1) return 0;
  if (!destinos) return 1;
  if (n > VT_LOTE) n = VT_LOTE;
  NvPerfilSessao sessao = nv_perfil_sessao_ler();
  if (!nv_perfil_sessao_travar(sessao)) return 0;
  if (destinos & VISTO_CONTA) {
    contapend_episodios(imdb, tipo, pares, n, visto);
    destinos |= VISTO_CONTA_JORNAL;
  }
  Envio *e = calloc(1, sizeof *e);
  if (!e) { nv_perfil_sessao_destravar(); return 0; }
  snprintf(e->imdb, sizeof e->imdb, "%s", imdb);
  snprintf(e->tipo, sizeof e->tipo, "%s", tipo && tipo[0] ? tipo : "series");
  memcpy(e->pares, pares, sizeof(VistoPar) * (size_t)n);
  e->n = n; e->visto = visto; e->destinos = destinos; e->sessao = sessao;
  capturarDestinos(e);
  int ok = lancar(e);
  nv_perfil_sessao_destravar(); return ok;
}
int visto_titulo(const char *imdb, const char *tipo, const int *temporadas,
                 int nt, int visto, int destinos) {
  destinos &= ~VISTO_TRAKT;
  if (!imdb || !imdb[0]) return 0;
  NvPerfilSessao sessao = nv_perfil_sessao_ler();
  if (!nv_perfil_sessao_travar(sessao)) return 0;
  vistoep_titulo_gesto(imdb, visto);
  if (!destinos) { nv_perfil_sessao_destravar(); return 1; }
  if (destinos & VISTO_CONTA) {
    contapend_titulo(imdb, tipo && tipo[0] ? tipo : "movie", visto);
    destinos |= VISTO_CONTA_JORNAL;
  }
  Envio *e = calloc(1, sizeof *e);
  if (!e) { nv_perfil_sessao_destravar(); return 0; }
  snprintf(e->imdb, sizeof e->imdb, "%s", imdb);
  snprintf(e->tipo, sizeof e->tipo, "%s", tipo && tipo[0] ? tipo : "movie");
  if (temporadas && nt > 0) {
    if (nt > VT_TEMPS) nt = VT_TEMPS;
    memcpy(e->temps, temporadas, sizeof(int) * (size_t)nt); e->nt = nt;
  }
  e->visto = visto; e->destinos = destinos; e->titulo = 1; e->sessao = sessao;
  capturarDestinos(e);
  int ok = lancar(e);
  nv_perfil_sessao_destravar(); return ok;
}
