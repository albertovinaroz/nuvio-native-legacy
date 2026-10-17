// A ordem do audio automatico: ver audioescolha.h. Codigo puro mais uma
// lembranca em memoria; sem disco, sem rede, sem tela.
#include "audioescolha.h"
#include "audioinfo.h"
#include "linguas.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

static char chaveAtual[96];
static AudioLembrada lembrada;
static int temLembrada;

const char *audioesc_origem_nome(int o) {
  switch (o) {
    case AUDIOESC_LEMBRADA: return "picked by hand earlier in this playback";
    case AUDIOESC_PREF1:    return "preferred language";
    case AUDIOESC_PREF2:    return "second language";
    case AUDIOESC_ORIGINAL: return "original language";
    default:                return "no rule";
  }
}

int audioesc_dts_truehd(const char *codec, const char *rotulo) {
  char s[128];
  size_t i;
  snprintf(s, sizeof s, " %s %s ", codec ? codec : "", rotulo ? rotulo : "");
  for (i = 0; s[i]; i++) s[i] = (char)tolower((unsigned char)s[i]);
  return strstr(s, "dts") || strstr(s, " dca") || strstr(s, "truehd") || strstr(s, "true-hd") ||
         strstr(s, "a_mlp") || strstr(s, " mlp");
}

void audioesc_titulo(const char *chave) {
  const char *c = chave ? chave : "";
  if (!strcmp(c, chaveAtual)) return;
  if (temLembrada) {
    printf("[audio] outro titulo: a faixa escolhida a mao no anterior nao vale aqui\n");
    fflush(stdout);
  }
  snprintf(chaveAtual, sizeof chaveAtual, "%s", c);
  temLembrada = 0;
  memset(&lembrada, 0, sizeof lembrada);
}

void audioesc_lembrar(const char *idioma, const char *codec, const char *rotulo, int canais) {
  memset(&lembrada, 0, sizeof lembrada);
  snprintf(lembrada.idioma, sizeof lembrada.idioma, "%s", idioma ? idioma : "");
  snprintf(lembrada.codec, sizeof lembrada.codec, "%s", codec ? codec : "");
  snprintf(lembrada.rotulo, sizeof lembrada.rotulo, "%s", rotulo ? rotulo : "");
  lembrada.canais = canais;
  temLembrada = 1;
  printf("[audio] escolha a mao lembrada nesta reproducao: %s (%s)\n",
         lembrada.idioma[0] ? lembrada.idioma : "?", lembrada.rotulo);
  fflush(stdout);
}
int audioesc_tem_lembrada(void) { return temLembrada; }

static int mesmoCodec(const char *a, const char *b) {
  const char *x = audioinfo_codec(a ? a : ""), *y = audioinfo_codec(b ? b : "");
  return x[0] && y[0] && !strcmp(x, y);
}

// Quanto a candidata parece a lembrada: 0 nao e ela.
static int parecenca(const AudioCand *c, const AudioLembrada *m) {
  int p = 0, rot = c->rotulo && m->rotulo[0] && !strcmp(c->rotulo, m->rotulo);
  int cod = mesmoCodec(c->codec, m->codec), ch = c->canais > 0 && c->canais == m->canais;
  if (m->idioma[0]) {
    if (!c->idioma || !c->idioma[0]) return rot ? 4 : 0;
    if (!ling_casa(c->idioma, m->idioma) || !ling_casa(m->idioma, c->idioma)) return 0;
    p = 1;
  } else if (!rot && !(cod && ch)) return 0;
  return p + (rot ? 4 : 0) + (cod ? 2 : 0) + (ch ? 1 : 0);
}

// Num idioma: a primeira que nao e DTS/TrueHD; senao a primeira DTS/TrueHD.
static int noIdioma(const AudioCand *c, int n, const char *pref) {
  int i, dts = -1;
  for (i = 0; i < n; i++) {
    if (!c[i].pode || !c[i].idioma || !c[i].idioma[0] || !ling_casa(c[i].idioma, pref)) continue;
    if (!c[i].dts) return i;
    if (dts < 0) dts = i;
  }
  return dts;
}

int audioesc_escolher_com(const AudioCand *c, int n, const AudioLembrada *m,
                          const char *pref1, const char *pref2, const char *original, int *origem) {
  const char *prefs[3] = { pref1, pref2, original };
  static const int ORIG[3] = { AUDIOESC_PREF1, AUDIOESC_PREF2, AUDIOESC_ORIGINAL };
  int i, k, j;
  if (origem) *origem = AUDIOESC_NADA;
  if (!c || n < 1) return -1;
  if (m) {
    int melhor = -1, nota = 0;
    for (i = 0; i < n; i++) {
      int p = c[i].pode ? parecenca(&c[i], m) : 0;
      if (p > nota) { nota = p; melhor = i; }
    }
    if (melhor >= 0) { if (origem) *origem = AUDIOESC_LEMBRADA; return melhor; }
  }
  for (k = 0; k < 3; k++) {
    const char *p = prefs[k];
    int repetida = 0;
    if (!p || !p[0] || !strcmp(p, "none")) continue;
    for (j = 0; j < k; j++) if (prefs[j] && prefs[j][0] && ling_casa(p, prefs[j]) && ling_casa(prefs[j], p)) repetida = 1;
    if (repetida) continue;
    if ((i = noIdioma(c, n, p)) >= 0) { if (origem) *origem = ORIG[k]; return i; }
  }
  return -1;
}

int audioesc_escolher(const AudioCand *c, int n, int *origem) {
  return audioesc_escolher_com(c, n, temLembrada ? &lembrada : NULL, ling_audio(), ling_audio2(),
                               ling_original(), origem);
}
