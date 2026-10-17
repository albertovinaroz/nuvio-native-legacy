#include "sinopse.h"
#include "ajustes.h"
#include "gfx.h"
#include "idioma.h"
#include "perfis.h"
#include "plrui.h"
#include "ponteiro.h"
#include "rolagemtoque.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define NV_ESCALA_TELA
#include "escala.h"

static int corpoOriginal(TxtEstilo e) {
  switch (e) {
    case TXT_ILHA_APOIO: return 16;
    case TXT_AJ_ESTADO: return 18;
    case TXT_ILHA_SUB: return 19;
    case TXT_ILHA_TEXTO: return 20;
    case TXT_CAPTION: case TXT_HERO_SIN: return 22;
    case TXT_DET_META2: return 23;
    case TXT_DET_SIN: return 26;
    default: return 0;
  }
}
static int corpo(TxtEstilo original) {
  int n = corpoOriginal(original), tamanho = ajustes_sinopse_tamanho();
  if (!tamanho || !n) return n;
  if (original == TXT_HERO_SIN) return 24; // Integration: Grande is 24/32.
  return (n * 125 + 50) / 100;
}
TxtEstilo sinopse_estilo(TxtEstilo original) {
  if (!ajustes_sinopse_tamanho()) return original;
  switch (corpo(original)) {
    case 19: return TXT_SINOPSE_19;
    case 20: return TXT_SINOPSE_20;
    case 21: return TXT_SINOPSE_21;
    case 22: return TXT_SINOPSE_22;
    case 23: return TXT_SINOPSE_23;
    case 24: return TXT_SINOPSE_24;
    case 25: return TXT_SINOPSE_25;
    case 26: return TXT_SINOPSE_26;
    case 27: return TXT_SINOPSE_27;
    case 28: return TXT_SINOPSE_28;
    case 29: return TXT_SINOPSE_29;
    case 30: return TXT_SINOPSE_30;
    case 31: return TXT_SINOPSE_31;
    case 33: return TXT_SINOPSE_33;
    case 34: return TXT_SINOPSE_34;
    case 35: return TXT_SINOPSE_35;
    case 39: return TXT_SINOPSE_39;
    default: return original;
  }
}
float sinopse_entrelinha(TxtEstilo original, float leading) {
  int n = corpoOriginal(original);
  if (!ajustes_sinopse_tamanho()) return leading;
  if (original == TXT_HERO_SIN) return 32.0f;
  return n ? leading * (float)corpo(original) / (float)n : leading;
}
int sinopse_linhas(TxtEstilo original, float leading, int maxLinhas) {
  if (!ajustes_sinopse_tamanho() || original == TXT_HERO_SIN) return maxLinhas;
  float novo = sinopse_entrelinha(original, leading);
  int n = maxLinhas > 0 && novo > 0 ? (int)floorf((float)maxLinhas * leading / novo + .001f) : maxLinhas;
  return maxLinhas > 0 && n < 1 ? 1 : n;
}
float sinopse_bloco_corta(TxtEstilo original, const char *texto, int r, int g, int b,
                          float x, float y, float larg, float leading, float alpha, int maxLinhas) {
  int linhas = sinopse_linhas(original, leading, maxLinhas);
  if (maxLinhas > 0 && linhas <= 0) return 0;
  return txt_bloco_corta(sinopse_estilo(original), texto, r, g, b, x, y, larg,
                         sinopse_entrelinha(original, leading), alpha, linhas);
}
float sinopse_bloco(TxtEstilo original, const char *texto, int r, int g, int b,
                    float x, float y, float larg, float leading, float alpha, int maxLinhas) {
  if (!ajustes_sinopse_tamanho())
    return txt_bloco(original, texto, r, g, b, x, y, larg, leading, alpha, maxLinhas);
  return sinopse_bloco_corta(original, texto, r, g, b, x, y, larg, leading, alpha, maxLinhas);
}

// Copy metadata at open: a late catalog refresh cannot replace the text under
// the reader or transfer it to another account/profile visit.
static struct {
  int aberta;
  uint64_t sessao;
  char titulo[160], texto[2048];
  float scroll, max, pagina, passo;
  float largura, escala;
  TxtFamilia familia;
  TxtEstilo estilo;
  int nLinhas;
  struct { unsigned short inicio, n; } linhas[2048];
#ifdef NV_TOUCH_UI
  ToqueRolagem toque;
#endif
} leitor;
void sinopse_fechar(void) { memset(&leitor, 0, sizeof leitor); }
int sinopse_aberta(void) {
  if (leitor.aberta && !perfis_sessao_atual(leitor.sessao)) sinopse_fechar();
  return leitor.aberta;
}
static void copiar(char *dst, size_t cap, const char *src) {
  size_t n = strlen(src);
  if (n >= cap) {
    n = cap - 1;
    while (n && ((unsigned char)src[n] & 0xc0) == 0x80) n--;
  }
  memcpy(dst, src, n); dst[n] = 0;
}
void sinopse_abrir(const char *titulo, const char *texto) {
  sinopse_fechar();
  if (!texto || !texto[0]) return;
  copiar(leitor.titulo, sizeof leitor.titulo, titulo ? titulo : "");
  copiar(leitor.texto, sizeof leitor.texto, texto);
  leitor.sessao = perfis_sessao_geracao(); leitor.aberta = 1;
}
static void fecharPonteiro(int a, int b) { (void)a; (void)b; sinopse_fechar(); }
#ifdef NV_TOUCH_UI
static int rolar(const PonteiroRolagem *e) { return sinopse_aberta() && toquerol_evento(&leitor.toque, e); }
#endif
void sinopse_evento(const SDL_Event *e) {
  SDL_Keycode k;
  if (!sinopse_aberta() || e->type != SDL_KEYDOWN) return;
  k = e->key.keysym.sym;
  if (k == SDLK_ESCAPE || k == SDLK_AC_BACK || k == SDLK_BACKSPACE || k == SDLK_DELETE ||
      k == SDLK_RETURN || k == SDLK_KP_ENTER || e->key.keysym.scancode == NV_SCANCODE_BACK) {
    sinopse_fechar(); return;
  }
#ifdef NV_TOUCH_UI
  if (toquerol_navegacao(e)) toquerol_limpar(&leitor.toque);
#endif
  if (k == SDLK_UP) leitor.scroll -= leitor.passo;
  else if (k == SDLK_DOWN) leitor.scroll += leitor.passo;
  else if (k == SDLK_PAGEUP) leitor.scroll -= leitor.pagina;
  else if (k == SDLK_PAGEDOWN) leitor.scroll += leitor.pagina;
  else if (k == SDLK_HOME) leitor.scroll = 0;
  else if (k == SDLK_END) leitor.scroll = leitor.max;
  if (leitor.scroll < 0) leitor.scroll = 0;
  if (leitor.scroll > leitor.max) leitor.scroll = leitor.max;
}

// Complete UTF-8 wrapping, including a word longer than the viewport. Work in
// codepoints rather than cutting bytes or dropping the rest of a long token.
// The same pass measures and draws, so rotation/font changes cannot leave an
// unreachable tail. Metadata is already bounded by the catalog's own limits.
static size_t proximo(const char *p) {
  size_t n = 1;
  while (p[n] && ((unsigned char)p[n] & 0xc0) == 0x80) n++;
  return n;
}
static float textoCompleto(TxtEstilo estilo, float x, float y, float w, float lead, int desenhar) {
  char linha[2048];
  int i;
  if (!leitor.nLinhas || leitor.largura != w || leitor.estilo != estilo || leitor.escala != gfx_escala() || leitor.familia != txt_fonte_interface()) {
    const char *p = leitor.texto;
    leitor.nLinhas = 0; leitor.largura = w; leitor.estilo = estilo;
    leitor.escala = gfx_escala(); leitor.familia = txt_fonte_interface();
    while (*p) {
      size_t n = 0, quebra = 0;
      const char *inicio;
      while (*p == ' ' || *p == '\r') p++;
      if (!*p) break;
      inicio = p;
      while (*p && *p != '\n') {
        size_t k = proximo(p);
        if (n + k >= sizeof linha) break;
        memcpy(linha + n, p, k); linha[n + k] = 0;
        if (n && txt_largura(estilo, linha) > w) break;
        if (*p == ' ') quebra = n;
        n += k; p += k;
      }
      if (*p && *p != '\n' && quebra) { n = quebra; p = inicio + quebra; }
      i = leitor.nLinhas++;
      leitor.linhas[i].inicio = (unsigned short)(inicio - leitor.texto);
      leitor.linhas[i].n = (unsigned short)n;
      if (*p == '\n') p++;
      // The minimum sheet width accommodates ordinary glyphs. In case of a
      // malformed/wider glyph, still advance and keep drawing inside scissor.
      if (p == inicio && *p) p += proximo(p);
    }
  }
  if (desenhar) for (i = 0; i < leitor.nLinhas; i++) {
    float top = (float)i * lead;
    size_t n = leitor.linhas[i].n;
    if (top + lead < leitor.scroll || top > leitor.scroll + leitor.pagina) continue;
    memcpy(linha, leitor.texto + leitor.linhas[i].inicio, n); linha[n] = 0;
    txt_desenhar_alpha(txt_linha(estilo, linha, 243, 242, 239, 255), x, y + top - leitor.scroll, 1);
  }
  return (float)leitor.nLinhas * lead;
}
void sinopse_desenhar(void) {
  float w, h, pad, x, y, lead, altura;
  GfxRect folha, texto, fechar, toqueFechar;
  TxtLinha cab, titulo, botao;
  TxtEstilo estilo = sinopse_estilo(TXT_DET_SIN);
  if (!sinopse_aberta()) return;
  ESCALA_INI();
  w = fminf(1100, NV_TELA_W - 48); h = NV_TELA_H - 48;
  pad = fminf(40, w * .06f); x = (NV_TELA_W - w) * .5f; y = 24;
  folha = (GfxRect){ x, y, w, h };
  cab = txt_linha_corta(TXT_AJ_INSP, i18n("Ler sinopse"), 243, 242, 239, 255, w - 2 * pad);
  titulo = txt_linha_corta(TXT_ILHA_SUB, leitor.titulo, 243, 242, 239, 255, w - 2 * pad);
  botao = txt_linha_corta(TXT_PG_ROTULO, i18n("Fechar"), 243, 242, 239, 255, w - 2 * pad - 40);
  { float visualH = fmaxf(56, botao.h + 24), alvoH = visualH;
#ifdef NV_TOUCH_UI
    // The Mobile canvas normalizes the short edge to1080. Reserve48dp on a
    // 360dp phone, independently of UI zoom, without covering scrollable text.
    if (layout_modo_mobile()) alvoH = fmaxf(alvoH, 144.0f / gfx_escala());
#endif
    toqueFechar = (GfxRect){ x + pad, y + h - pad - alvoH, w - 2 * pad, alvoH };
    fechar = toqueFechar; fechar.y += (alvoH - visualH) * .5f; fechar.h = visualH;
  }
  texto = (GfxRect){ x + pad, y + pad + cab.h + 8 + titulo.h + 24, w - 2 * pad - 12, 0 };
  texto.h = toqueFechar.y - 24 - texto.y;
  if (texto.h < 1) texto.h = 1;
  lead = sinopse_entrelinha(TXT_DET_SIN, 40);
  leitor.pagina = texto.h; leitor.passo = lead;
  altura = textoCompleto(estilo, texto.x, texto.y, texto.w, lead, 0);
  leitor.max = fmaxf(0, altura - texto.h);
  leitor.scroll = fmaxf(0, fminf(leitor.max, leitor.scroll));
  ponteiro_camada();
  ponteiro_alvo(0, 0, NV_TELA_W, NV_TELA_H, NULL, NULL, 0, 0);
  gfx_cor((GfxRect){ 0, 0, NV_TELA_W, NV_TELA_H }, 0, 0, 0, 0, .65f);
  plrui_material(folha, 28, 1, 1);
  txt_desenhar_alpha(cab, x + pad, y + pad, 1);
  txt_desenhar_alpha(titulo, x + pad, y + pad + cab.h + 8, .65f);
  gfx_recorte(texto.x, texto.y, texto.w, texto.h);
  textoCompleto(estilo, texto.x, texto.y, texto.w, lead, 1);
  gfx_sem_recorte();
  if (leitor.max > 0) {
    float bh = fmaxf(20, texto.h * texto.h / altura);
    gfx_cor((GfxRect){ x + w - pad - 4, texto.y + (texto.h - bh) * leitor.scroll / leitor.max, 4, bh }, .5f, 1, 1, 1, .4f);
  }
  plrui_linha_foco(fechar, 20, 1);
  txt_desenhar_alpha(botao, fechar.x + (fechar.w - botao.w) * .5f, fechar.y + (fechar.h - botao.h) * .5f, 1);
  ponteiro_alvo(toqueFechar.x, toqueFechar.y, toqueFechar.w, toqueFechar.h, NULL, fecharPonteiro, 0, 0);
#ifdef NV_TOUCH_UI
  toquerol_vincular(&leitor.toque, texto, gfx_escala(), 0, leitor.max, 1, &leitor.scroll);
  ponteiro_rolagem(rolar);
#endif
  ESCALA_FIM();
}
