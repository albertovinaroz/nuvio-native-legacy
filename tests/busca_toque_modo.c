// Search result rows drag-scroll only in Mobile mode; TV keeps drag-as-arrows.
// Samsung: IME=0, voice=1, phone=0 must still have a D-pad route to the mic.
#include <assert.h>
#define st_ime_disponivel teste_ime
#define st_voz_disponivel teste_voz
#define st_voz_iniciar teste_iniciar_voz
#define st_teclado_modo teste_modo_teclado
#define st_abre_sozinho teste_abre_sozinho
#define st_ime_abrir teste_abrir_ime
#define celb_disponivel teste_cel
#include "../src/busca.c"
#undef st_ime_disponivel
#undef st_voz_disponivel
#undef st_voz_iniciar
#undef st_teclado_modo
#undef st_abre_sozinho
#undef st_ime_abrir
#undef celb_disponivel

static int temIme, temVoz, temCel, vozIniciada;
static int modoTeclado, abreSozinho, imeIniciado;
int teste_modo_teclado(void) { return modoTeclado; }
int teste_abre_sozinho(void) { return abreSozinho; }
int teste_abrir_ime(int dono, const char *texto, int max) {
  assert(texto && !texto[0] && max == BU_MAX_CONSULTA - 1);
  imeIniciado = dono; return 1;
}
int teste_ime(void) { return temIme; }
int teste_voz(void) { return temVoz; }
int teste_cel(void) { return temCel; }
int teste_iniciar_voz(int dono) { vozIniciada = dono; return 1; }
#include <assert.h>

static void arrasto(int mobile) {
  PonteiroRolagem e = { PONT_ROL_INICIO, 0, 0.0f, 0.0f, 0.0f, 0.0f };
  layout_modo_definir(mobile);
  nFil = 1; fil[0].n = 20; fil[0].pessoas = 0; fil[0].melhor = 0;
  scrollX[0] = 0.0f; toqueLivreX[0] = 0; toqueFileira = -1;
  e.x = BU_RES_X + 10.0f; e.y = BU_RES_Y + 10.0f;
  int captura = toqueBuscaRolar(&e);
  e.fase = PONT_ROL_MOVER; e.delta = -200.0f;
  toqueBuscaRolar(&e);
  if (mobile) assert(captura == 1 && scrollX[0] == 200.0f && toqueLivreX[0]);
  else assert(captura == 0 && scrollX[0] == 0.0f && !toqueLivreX[0]);
}

int main(void) {
  layout_tela_definir(1920, 1080);
  arrasto(0);
  arrasto(1);
  layout_modo_definir(0);
  toqueBuscaLimpar();
  SDL_Event e; SDL_zero(e); e.type = SDL_KEYDOWN;
  for (int modo = 0; modo < 4; modo++) {
    temIme = modo == 1; temVoz = modo == 0; temCel = modo == 2;
    painel = campoFoco = 0;
    focus_iniciar(&focoKb, 1, KB_COLUNAS);
    e.key.keysym.sym = SDLK_UP; busca_evento(&e);
    assert(campoFoco == (temIme ? 1 : temVoz ? 2 : temCel ? 3 : 0));
    if (temVoz) {
      e.key.keysym.sym = SDLK_RETURN; busca_evento(&e);
      assert(vozIniciada == ST_BUSCA);
    }
  }
  // TV/web: only "Do sistema" opens automatically; Android/Mobile keep
  // automatic IME entry, except Android's explicit "Do app" mode.
  for (int mobile = 0; mobile <= 1; mobile++) {
    layout_modo_definir(mobile);
    for (abreSozinho = 0; abreSozinho <= 1; abreSozinho++) {
      for (temIme = 0; temIme <= 1; temIme++) {
        for (modoTeclado = 0; modoTeclado <= 2; modoTeclado++) {
          imeIniciado = 0;
          busca_iniciar();
          int abre = temIme && modoTeclado != 1 &&
                     (abreSozinho || mobile || modoTeclado == 2);
          assert(imeIniciado == (abre ? ST_BUSCA : 0));
          assert(busca_foco_campo() == ((abre || (mobile && temIme)) ? 1 : 0));
          busca_encerrar();
        }
      }
    }
  }
  puts("PASS: Busca drag, mic navigation and automatic system keyboard modes");
  return 0;
}
