// A pilula do avanco (fork): toques "+30 s" / "-20 s" e segurando "40x".
// Player real (OSD/GL) sem pipeline, como player_seekr_status_shot.c.
#include "../src/player.c"
#define main iniciarFixtureGlass
#include "player_glass_shot.c"
#undef main

static void toque(double base, double alvo, int dir) {
  player_shot_estado(relogio, (float)base, 7200, 1, 0, 1, 1);
  player_shot_buscando(1);
  scrubBase = base; posSeg = alvo; scrubDir = dir;
  scrubSalto = (SaltoEst){0};
  quadros(40);
}
static void segurando(double base, double alvo, int dir, unsigned heldMs) {
  player_shot_estado(relogio, (float)base, 7200, 1, 0, 1, 1);
  player_shot_buscando(1);
  scrubBase = base; posSeg = alvo; scrubDir = dir;
  for (int i = 0; i < 40; i++) {
    scrubSalto.rep = 1; scrubSalto.inicio = relogio + 16 - heldMs; scrubSalto.ultimaTecla = relogio + 16;
    quadros(1);
  }
}

int main(int argc, char **argv) {
  assert(argc == 2);
  char *init[] = {argv[0], argv[1], "salto-init"};
  assert(iniciarFixtureGlass(3, init) == 0);
  ajustes_shot_valor("seekrLocal", 1);   // sem miniatura: so a pilula
  abrir(&filme); simular(3840,1606,"",1,1);
  toque(1800, 1830, 1);
  assert(pilSaltoNivel == -1 && fabs(pilSaltoDelta - 30) < 1.0);
  salvar("salto-toque-mais30");
  toque(1800, 1780, -1);
  salvar("salto-toque-menos20");
  segurando(1800, 1960, 1, 1500);
  assert(pilSaltoNivel == 1);
  salvar("salto-segurando-40x");
  segurando(4000, 3100, -1, 4000);
  assert(pilSaltoNivel == 3);
  salvar("salto-segurando-tras-160x");
  // Atras cancela: sem busca, posicao de antes, pilula some.
  cancelarSalto();
  assert(!scrubbing);
  shotBusca = 0;   // o gancho de captura religaria o avanco a cada quadro
  quadros(40);
  assert(pilSaltoA < 0.1f);
  puts("player salto: PASS pilula de toque e de escalao, Atras cancela");
  return 0;
}
