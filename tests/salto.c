// Avanco do player (salto.h): toques somam 10 s; segurar anda por ESCALOES
// fixos de velocidade (20x, 40x, 80x...), dobrando a cada SALTO_NIVEL_MS.
#include <stdio.h>
#include <math.h>
#include "../src/salto.h"

static int falhas;
#define CHECK(c, ...) do { if (!(c)) { printf("FALHA: " __VA_ARGS__); printf("\n"); falhas++; } } while (0)

// `n` toques separados por `gapMs`, cada um uma rajada de uma tecla so.
static float toques(int n, unsigned gapMs, float dur) {
  SaltoEst st = {0}; float tot = 0;
  for (int i = 0; i < n; i++)
    tot += salto_tecla(&st, i == 0, 0, 1000u + i * gapMs, dur);
  return tot;
}

int main(void) {
  const float filme = 7200.0f;
  CHECK(toques(5, 200, filme) == 50.0f, "5 toques em 1 s = 50 s, deu %.0f", toques(5, 200, filme));
  CHECK(toques(2, 0, filme) == 20.0f, "dois toques no mesmo instante = 20 s");
  CHECK(toques(3, 160, filme) == 30.0f, "toques a 160 ms nao sao repeticao");
  { SaltoEst st = {0}; float t = salto_tecla(&st, 1, 0, 1000, filme);
    t += salto_tecla(&st, 0, 1, 1050, filme);
    CHECK(t == 10.0f && st.rep, "repeat=1 nao soma passo, marca segurado"); }
  // Escaloes: dobram a cada SALTO_NIVEL_MS, nunca descem segurando.
  CHECK(salto_nivel(0, filme) == 0 && salto_vel(0, filme) == 20.0f, "comeca em 20x");
  CHECK(salto_nivel(SALTO_NIVEL_MS, filme) == 1 && salto_vel(SALTO_NIVEL_MS, filme) == 40.0f, "depois 40x");
  { int ant = 0; for (unsigned h = 0; h <= 20000; h += 100) {
      int n = salto_nivel(h, filme); CHECK(n >= ant, "escalao monotono em %u", h); ant = n; } }
  CHECK(salto_vel(60000, filme) == 640.0f, "filme de 2 h chega a 640x");
  // Teto pela duracao: nunca atravessa o arquivo em menos de 8 s.
  { float durs[] = { 300.0f, 1200.0f, 2700.0f, 7200.0f };
    for (int i = 0; i < 4; i++)
      CHECK(salto_vel(60000, durs[i]) <= durs[i] / SALTO_TRAVESSIA_MIN_S || salto_nivel(60000, durs[i]) == 0,
            "dur %.0f: %.0fx atravessa rapido demais", durs[i], salto_vel(60000, durs[i])); }
  CHECK(salto_vel(60000, 1200.0f) == 80.0f, "episodio de 20 min para em 80x");
  CHECK(salto_vel(60000, 0.0f) == 20.0f, "duracao desconhecida: 20x");
  // Escalao para a tela: -1 sem tecla segurada.
  { SaltoEst st = {0};
    salto_tecla(&st, 1, 0, 1000, filme);
    CHECK(salto_nivel_atual(&st, 1050, filme) == -1, "toque nao mostra escalao");
    salto_tecla(&st, 0, 0, 1100, filme);
    CHECK(salto_nivel_atual(&st, 1150, filme) == 0, "segurando: 20x");
    CHECK(salto_nivel_atual(&st, 1100 + SALTO_VIVO_MS + 50, filme) == -1, "soltou: some"); }
  // Janela de confirmacao: toques espacados ate 700 ms somam no mesmo salto.
  CHECK(salto_fim_ms(1) == 1000u && salto_fim_ms(0) == 700u, "janela de fim");
  CHECK(salto_fim_ms(0) > SALTO_REP_MS * 2, "janela maior que a repeticao");
  printf(falhas ? "salto: %d falha(s)\n" : "salto OK\n", falhas);
  return falhas != 0;
}
