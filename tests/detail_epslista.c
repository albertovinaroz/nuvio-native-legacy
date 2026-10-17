// #205 (dono): margem entre os cards da fileira de episodios e prato de foco
// "estilo Apple TV" (placa cinza-claro translucida que nasce na mola do foco,
// com o texto acompanhando). Sem janela e sem rede — a mesma receita de
// tests/detail_epcard.c. O que este teste cobra:
//
//   1. a MARGEM entre cards consecutivos existe e e UMA so: o passo menos a
//      largura do card da o token NV_DETP_EP_GAP, e as colunas vizinhas ficam
//      a UMA largura de card mais esse vao, em todos os tres tamanhos da
//      Moderna e no layout Apple;
//   2. o PRATO DE FOCO e a mola do foco com a tinta no teto do token:
//      epPlacaA(0) == 0 (sem foco nao ha placa), epPlacaA(1) e os 12-16% de
//      branco pedidos, e a mola da fileira atravessa valores intermediarios —
//      a placa nao "pula" ligada;
//   3. com as animacoes reduzidas a mola exige o snapping (anim_politica_
//      reduzida), entao a placa tambem — estados 0 e 1 diretos, sem voo;
//   4. focar um episodio nao move os vizinhos: xItem nao olha para o foco.
//      (prova de "no layout jump" com o prato desenhado por cima)
#include "nvtmp.h"
#include "dados.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../src/detail.c"

static char dir[NVT_MAX], caminho[700];
static void gravar(const char *txt) {
  FILE *f = fopen(caminho, "w");
  assert(f);
  fputs(txt, f);
  fclose(f);
  ajustes_dir(dir);
}

int main(void) {
  CatItem c;
  CatEp eps[6];
  int v, i;

  nvt_set(dir, sizeof dir, "/nuvio-detail-epslista-XXXXXX");
  assert(mkdtemp(dir));
  assert(setenv("NUVIO_DADOS", dir, 1) == 0);
  dados_iniciar(dir);
  snprintf(caminho, sizeof caminho, "%s/ajustes.txt", dir);

  memset(&c, 0, sizeof c);
  snprintf(c.tipo, sizeof c.tipo, "series");
  snprintf(c.titulo, sizeof c.titulo, "Série de Ensaio");
  snprintf(c.imdb, sizeof c.imdb, "tt0000205");
  c.temporadas[0] = 1; c.nTemporadas = 1;
  cat_definir_tudo(&c, 1, NULL, 0); idx = 0;
  memset(eps, 0, sizeof eps);
  for (i = 0; i < 6; i++) {
    eps[i].temporada = 1;
    eps[i].episodio = i + 1;
    snprintf(eps[i].nome, sizeof eps[i].nome, "Episódio %d", i + 1);
  }
  cat_definir_episodios(0, eps, 6);

  gravar("");
  assert(ajustes_home_layout() == HOME_LAYOUT_MODERNA && ajustes_ep_tamanho() == 1);

  // 1. A MARGEM E O TOKEN, nos TRES TAMANHOS: o passo mede card + NV_DETP_EP_
  //    GAP (na escala do tamanho) e cada par de colunas vizinhas repete
  //    exatamente esse vao.
  for (v = 0; v < 3; v++) {
    char linha[48];
    float gap;
    snprintf(linha, sizeof linha, "epTamanhoLocal %d\n", v);
    gravar(linha);
    gap = epCardPasso() - epCardW();
    assert(gap > 0.0f && "o card nao pode encostar no vizinho");
    assert(fabsf(gap - NV_DETP_EP_GAP * epEscala()) < 0.01f);
    for (i = 0; i < 3; i++)
      assert(fabsf(xItem(SEC_EPISODIOS, i + 1) - xItem(SEC_EPISODIOS, i) -
                   epCardPasso()) < 0.01f);
  }
  // No layout Apple o vao e o mesmo token, sem escala (o card e fixo).
  gravar("homeLayoutLocal 2\n");
  assert(epApple());
  assert(fabsf((epCardPasso() - epCardW()) - NV_DETP_EP_GAP) < 0.01f);
  gravar("");

  // 2. O PRATO NASCE NA MOLA DO FOCO. Focado no primeiro episodio, os quadros
  //    intermediarios da mola atravessam alfas intermediarios da placa; em
  //    repouso (foco fora da fileira) nao ha placa nenhuma.
  assert(epPlacaA(0.0f) == 0.0f);
  assert(epPlacaA(1.0f) >= 0.12f && epPlacaA(1.0f) <= 0.16f);
  aberto = 1; saindo = 0; nivel = 1;
  foco.fileira = SEC_EPISODIOS; foco.coluna = 1;
  for (i = 0; i < N_SECOES; i++) memset(animFoco[i], 0, sizeof animFoco[i]);
  detail_atualizar(1.0f / 60.0f, 1000);
  { float f1 = animFoco[SEC_EPISODIOS][1];
    assert(f1 > 0.0f && f1 < 1.0f && "a mola atravessa valores intermediarios");
    assert(epPlacaA(f1) > epPlacaA(0.0f) && epPlacaA(f1) < epPlacaA(1.0f));
    assert(animFoco[SEC_EPISODIOS][0] == 0.0f && "quem nao tem foco nao tem placa");
    for (i = 0; i < 120; i++) detail_atualizar(1.0f / 60.0f, 1000 + (i + 1) * 16);
    assert(animFoco[SEC_EPISODIOS][1] > 0.99f); }

  // 3. REDUZIDAS: a mola vai direto ao alvo, e a placa tambem.
  anim_politica_reduzida = 1;
  foco.coluna = 2;
  detail_atualizar(1.0f / 60.0f, 5000);
  assert(animFoco[SEC_EPISODIOS][2] == 1.0f);
  assert(animFoco[SEC_EPISODIOS][1] == 0.0f);
  anim_politica_reduzida = 0;

  // 4. FOCAR NAO MOVE OS VIZINHOS: as posicoes das colunas sao as mesmas com
  //    o foco em 0, em 2 ou fora da fileira.
  { float x0 = xItem(SEC_EPISODIOS, 0), x1 = xItem(SEC_EPISODIOS, 1),
          x2 = xItem(SEC_EPISODIOS, 2);
    foco.coluna = 0;
    assert(xItem(SEC_EPISODIOS, 0) == x0 && xItem(SEC_EPISODIOS, 1) == x1 &&
           xItem(SEC_EPISODIOS, 2) == x2);
    foco.fileira = SEC_TEMPORADAS;
    assert(xItem(SEC_EPISODIOS, 0) == x0 && xItem(SEC_EPISODIOS, 1) == x1); }

  puts("detail_epslista: margem entre cards, prato de foco na mola, reduzidas snap, sem salto");
  return 0;
}
