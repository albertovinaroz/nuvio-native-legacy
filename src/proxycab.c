// PROXY DE CABECALHOS PARA O VOD DA SAMSUNG. Ver proxycab.h para o porque.
#include "proxycab.h"
#include <string.h>
#include <strings.h>

int proxycab_precisa(const char *cabs) {
  const char *p = cabs;
  while (p && *p) {
    const char *fim = strchr(p, '\n');
    size_t n = fim ? (size_t)(fim - p) : strlen(p);
    while (n && (*p == ' ' || *p == '\t' || *p == '\r')) { p++; n--; }
    if (n && memchr(p, ':', n) &&
        strncasecmp(p, "User-Agent:", 11) && strncasecmp(p, "Cookie:", 7)) return 1;
    p = fim ? fim + 1 : NULL;
  }
  return 0;
}

#if defined(__EMSCRIPTEN__) || defined(NV_ANDROID)
int  proxycab_url(const char *f, const char *c, char *s, size_t n) { (void)f; (void)c; (void)s; (void)n; return 0; }
void proxycab_parar(void) {}
int  proxycab_e_url(const char *u) { (void)u; return 0; }
#else

#include "rede.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0   // Mac: SO_NOSIGPIPE no socket
#endif

#define PC_URL_MAX    8192
#define PC_CABS_MAX   2048
#define PC_PARADO_MS  20000L   // sem byte da fonte por isto: desiste da conexao

static pthread_mutex_t trava = PTHREAD_MUTEX_INITIALIZER;
static int sockOuvir = -1, porta;
static char fonteAtual[PC_URL_MAX], cabsAtual[PC_CABS_MAX];
static atomic_uint sessaoAtual;

typedef struct {
  int s; unsigned sessao;
  char fonte[PC_URL_MAX], cabs[PC_CABS_MAX];
  int64_t falta;          // bytes que o player pediu (-1 = ate o fim)
  volatile int cancelar;
} PcConexao;

static int pcEnviar(int s, const void *b, size_t n) {
  const char *p = b;
  while (n) {
    ssize_t r = send(s, p, n, MSG_NOSIGNAL);
    if (r < 0 && errno == EINTR) continue;
    if (r <= 0) return 0;
    p += r; n -= (size_t)r;
  }
  return 1;
}

// Corpo da fonte -> player. Para quando o player fechou, a sessao mudou, ou o
// trecho pedido (Range com fim) ja foi entregue.
static int pcSink(void *o, const char *d, size_t n) {
  PcConexao *c = o;
  if (c->cancelar || c->sessao != atomic_load(&sessaoAtual)) return 0;
  if (c->falta >= 0 && (int64_t)n > c->falta) n = (size_t)c->falta;
  if (n && !pcEnviar(c->s, d, n)) { c->cancelar = 1; return 0; }
  if (c->falta >= 0) { c->falta -= (int64_t)n; if (c->falta <= 0) return 0; }
  return 1;
}

static const char *pcTipo(const char *url) {
  const char *q = strchr(url, '?');
  size_t n = q ? (size_t)(q - url) : strlen(url);
  if (n >= 4 && !strncasecmp(url + n - 4, ".mkv", 4)) return "video/x-matroska";
  if (n >= 4 && !strncasecmp(url + n - 4, ".mp4", 4)) return "video/mp4";
  if (n >= 5 && !strncasecmp(url + n - 5, ".webm", 5)) return "video/webm";
  if (n >= 3 && !strncasecmp(url + n - 3, ".ts", 3)) return "video/mp2t";
  return "application/octet-stream";
}

static void *pcAtender(void *u) {
  PcConexao *c = u;
  char req[4096], cab[512];
  ssize_t r, tot = 0;
  const char *q;
  int head = 0, temRange = 0;
  int64_t ini = 0, fim = -1;
  while (tot < (ssize_t)sizeof req - 1) {
    r = recv(c->s, req + tot, sizeof req - 1 - (size_t)tot, 0);
    if (r <= 0) break;
    tot += r; req[tot] = 0;
    if (strstr(req, "\r\n\r\n") || strstr(req, "\n\n")) break;
  }
  req[tot > 0 ? tot : 0] = 0;
  head = !strncmp(req, "HEAD ", 5);
  q = strstr(req, "?s=");
  c->sessao = q ? (unsigned)strtoul(q + 3, NULL, 10) : 0;
  pthread_mutex_lock(&trava);
  if (c->sessao && c->sessao == atomic_load(&sessaoAtual)) {
    snprintf(c->fonte, sizeof c->fonte, "%s", fonteAtual);
    snprintf(c->cabs, sizeof c->cabs, "%s", cabsAtual);
  } else c->fonte[0] = 0;
  pthread_mutex_unlock(&trava);
  if (!c->fonte[0] || (!head && strncmp(req, "GET ", 4))) {
    static const char nao[] = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    pcEnviar(c->s, nao, sizeof nao - 1);
    goto sair;
  }
  { const char *p = req;
    while ((p = strchr(p, '\n')) != NULL) {
      p++;
      if (!strncasecmp(p, "Range:", 6)) {
        const char *v = p + 6;
        while (*v == ' ' || *v == '\t') v++;
        if (!strncasecmp(v, "bytes=", 6)) {
          char *e;
          ini = strtoll(v + 6, &e, 10);
          if (*e == '-' && e[1] >= '0' && e[1] <= '9') fim = strtoll(e + 1, NULL, 10);
          temRange = ini >= 0;
        }
        break;
      }
    } }
  // O TAMANHO E O DESTINO: um byte, com os cabecalhos do addon. Sem isso nao ha
  // Content-Range para o player, e e onde um 403 (Referer errado) aparece.
  { long sz = 0;
    int64_t total = -1;
    int st = 0, cross = 0;
    char *fin = malloc(PC_URL_MAX), *b;
    if (!fin) goto sair;
    fin[0] = 0;
    b = rede_baixar_trecho64_final(c->fonte, c->cabs, ini, ini, &sz, &total, &st,
                                   &c->cancelar, fin, &cross);
    free(b); free(fin);
    if (st != 206 || total <= 0 || ini >= total) {
      printf("[proxy-cab] sessao %u: a fonte respondeu HTTP %d (total %lld)\n",
             c->sessao, st, (long long)total);
      fflush(stdout);
      snprintf(cab, sizeof cab, "HTTP/1.1 %d %s\r\nContent-Length: 0\r\nConnection: close\r\n\r\n",
               st == 416 || (total > 0 && ini >= total) ? 416 : 502,
               st == 416 || (total > 0 && ini >= total) ? "Range Not Satisfiable" : "Bad Gateway");
      pcEnviar(c->s, cab, strlen(cab));
      goto sair;
    }
    if (fim < 0 || fim >= total) fim = total - 1;
    c->falta = fim - ini + 1;
    if (temRange)
      snprintf(cab, sizeof cab,
               "HTTP/1.1 206 Partial Content\r\nContent-Type: %s\r\nAccept-Ranges: bytes\r\n"
               "Content-Range: bytes %lld-%lld/%lld\r\nContent-Length: %lld\r\nConnection: close\r\n\r\n",
               pcTipo(c->fonte), (long long)ini, (long long)fim, (long long)total, (long long)c->falta);
    else
      snprintf(cab, sizeof cab,
               "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nAccept-Ranges: bytes\r\n"
               "Content-Length: %lld\r\nConnection: close\r\n\r\n",
               pcTipo(c->fonte), (long long)total);
    printf("[proxy-cab] sessao %u: %s %s (%lld de %lld bytes)\n", c->sessao, head ? "HEAD" : "GET",
           temRange ? (ini ? "Range no meio" : "Range do inicio") : "sem Range",
           (long long)c->falta, (long long)total);
    fflush(stdout);
    if (!pcEnviar(c->s, cab, strlen(cab)) || head) goto sair; }
  { int64_t total = -1, bytes = 0;
    int st = 0, cross = 0, rr;
    char *fin = malloc(PC_URL_MAX);
    if (!fin) goto sair;
    fin[0] = 0;
    rr = rede_fluxo64(c->fonte, c->cabs, ini, pcSink, c, &total, &st, &c->cancelar,
                      fin, &cross, PC_PARADO_MS, &bytes);
    free(fin);
    if (rr < 0 && !c->cancelar)
      printf("[proxy-cab] sessao %u: a fonte parou (HTTP %d, %lld bytes entregues)\n",
             c->sessao, st, (long long)bytes);
    fflush(stdout); }
sair:
  close(c->s);
  free(c);
  return NULL;
}

static void *pcOuvir(void *u) {
  (void)u;
  for (;;) {
    int s = accept(sockOuvir, NULL, NULL);
    PcConexao *c;
    pthread_t t;
    if (s < 0) { if (errno == EINTR) continue; usleep(200 * 1000); continue; }
#ifdef SO_NOSIGPIPE
    { int um = 1; setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, &um, sizeof um); }
#endif
    { struct timeval tv = { 10, 0 };
      setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
      setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof tv); }
    c = calloc(1, sizeof *c);
    if (!c) { close(s); continue; }
    c->s = s; c->falta = -1;
    if (pthread_create(&t, NULL, pcAtender, c) == 0) pthread_detach(t);
    else { close(s); free(c); }
  }
  return NULL;
}

static int pcSubir(void) {
  struct sockaddr_in a;
  socklen_t al = sizeof a;
  pthread_t t;
  int um = 1;
  if (sockOuvir >= 0) return 1;
  signal(SIGPIPE, SIG_IGN);
  sockOuvir = socket(AF_INET, SOCK_STREAM, 0);
  if (sockOuvir < 0) return 0;
  setsockopt(sockOuvir, SOL_SOCKET, SO_REUSEADDR, &um, sizeof um);
  memset(&a, 0, sizeof a);
  a.sin_family = AF_INET;
  a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.sin_port = 0;
  if (bind(sockOuvir, (struct sockaddr *)&a, sizeof a) < 0 || listen(sockOuvir, 8) < 0 ||
      getsockname(sockOuvir, (struct sockaddr *)&a, &al) < 0) {
    close(sockOuvir); sockOuvir = -1; return 0;
  }
  porta = ntohs(a.sin_port);
  if (pthread_create(&t, NULL, pcOuvir, NULL) != 0) { close(sockOuvir); sockOuvir = -1; return 0; }
  pthread_detach(t);
  printf("[proxy-cab] ouvindo em 127.0.0.1:%d\n", porta);
  fflush(stdout);
  return 1;
}

int proxycab_url(const char *fonte, const char *cabs, char *saida, size_t n) {
  unsigned s;
  if (!fonte || !fonte[0] || !saida || n < 40) return 0;
  if (strncmp(fonte, "http://", 7) && strncmp(fonte, "https://", 8)) return 0;
  pthread_mutex_lock(&trava);
  if (!pcSubir()) { pthread_mutex_unlock(&trava); return 0; }
  snprintf(fonteAtual, sizeof fonteAtual, "%s", fonte);
  snprintf(cabsAtual, sizeof cabsAtual, "%s", cabs ? cabs : "");
  s = atomic_fetch_add(&sessaoAtual, 1) + 1;
  pthread_mutex_unlock(&trava);
  snprintf(saida, n, "http://127.0.0.1:%d/v?s=%u", porta, s);
  printf("[proxy-cab] sessao %u: fonte com cabecalhos que o player nao manda\n", s);
  fflush(stdout);
  return 1;
}

void proxycab_parar(void) {
  pthread_mutex_lock(&trava);
  atomic_fetch_add(&sessaoAtual, 1);
  fonteAtual[0] = 0; cabsAtual[0] = 0;
  pthread_mutex_unlock(&trava);
}

int proxycab_e_url(const char *url) {
  return url && !strncmp(url, "http://127.0.0.1:", 17) && strstr(url, "/v?s=");
}

#endif
