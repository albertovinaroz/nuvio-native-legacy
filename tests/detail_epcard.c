// OS CARDS DE EPISODIO DA MODERNA (#205), sem janela e sem rede.
//
// Pedido do dono: a base do selo "EPISODIO n" na mesma altura do selo
// Assistido, tres tamanhos de card no Ajustes, titulo um degrau abaixo e
// sinopse de 4 linhas. O que este teste cobra:
//   1. as BASES casam: selo do numero e selo Assistido terminam na mesma
//      linha do topo da miniatura — e o Assistido e o lado FIXO do casamento;
//   2. os TRES TAMANHOS dao TRES ALTURAS de card (e de miniatura e de passo),
//      crescentes, com a miniatura em 16:9 e o Medio igual ao de sempre;
//   3. a SINOPSE de 4 LINHAS CABE no card: o bloco abaixo da miniatura reserva
//      as 4 linhas em qualquer tamanho (as fontes nao escalam);
//   4. o titulo desce UM DEGRAU na escala de sempre (TXT_CALLOUT, nao
//      TXT_PLR_CORPO) na Moderna;
//   5. o foco continua ANDANDO na fileira em cada tamanho: as colunas ficam
//      no passo certo e nao se atropelam (passo maior que o card).
//
// A escolha do tamanho e gravada como a do dono: um ajustes.txt de verdade
// em uma pasta temporaria, lido por ajustes_dir — o caminho da TV.
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
  SDL_Event e = {0};
  float h[3], th[3], w[3], p[3];
  int v, i;

  nvt_set(dir, sizeof dir, "/nuvio-detail-epcard-XXXXXX");
  assert(mkdtemp(dir));
  assert(setenv("NUVIO_DADOS", dir, 1) == 0);
  dados_iniciar(dir);
  snprintf(caminho, sizeof caminho, "%s/ajustes.txt", dir);

  // Uma serie de ensaio com 6 episodios na T1: a fileira existe de verdade.
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

  // 1. BASE DO EPISODIO NA MESMA ALTURA DO WATCHED. O selo Assistido e fixo
  //    (NV_DETP_EP_VISTO_*); o do numero e que desce para as bases cairem
  //    juntas — e nao ao contrario, que e o que o dono pediu.
  assert(NV_DETP_EPM_SELO_Y + NV_DETP_EP_SELO_H ==
         NV_DETP_EP_VISTO_Y + NV_DETP_EP_VISTO_H);

  // 2 e 3. TRES TAMANHOS: alturas, larguras e passos crescentes; a miniatura
  //    continua 16:9; o Medio e o card de sempre (640x360); as 4 linhas de
  //    sinopse cabem no bloco de texto em TODOS os tamanhos.
  for (v = 0; v < 3; v++) {
    char linha[48];
    snprintf(linha, sizeof linha, "epTamanhoLocal %d\n", v);
    gravar(linha);
    assert(ajustes_ep_tamanho() == v);
    assert(ajustes_home_layout() == HOME_LAYOUT_MODERNA);
    h[v] = epCardH(); th[v] = epThumbH(); w[v] = epCardW(); p[v] = epCardPasso();
    // As bases continuam casando em qualquer tamanho: nenhum dos dois selos
    // escala com o card.
    assert(NV_DETP_EPM_SELO_Y + NV_DETP_EP_SELO_H ==
           NV_DETP_EP_VISTO_Y + NV_DETP_EP_VISTO_H);
    // A sinopse de 4 linhas cabe entre o fim da miniatura e o fim do card.
    assert(epSinLinhas() == 4);
    assert(h[v] - th[v] >= NV_DETP_EPM_SIN_DY + 4.0f * NV_DETP_EP_LD_SIN);
    // A secao mede o card que sera desenhado: o empilhamento acompanha.
    assert(alturaSecao(SEC_EPISODIOS) == h[v]);
    // Colunas lado a lado no passo certo (card + a margem do #205).
    assert(p[v] > w[v]);
    assert(fabsf(p[v] - w[v] - NV_DETP_EP_GAP * epEscala()) < 0.01f);
    // O Medio e o card de sempre mais a margem nova; os passos crescem com o
    // tamanho porque a margem acompanha a escala.
    assert(fabsf(xItem(SEC_EPISODIOS, 3) - xItem(SEC_EPISODIOS, 2) - p[v]) < 0.01f);
    // O foco anda pela fileira inteira neste tamanho. Um punhado de quadros
    // de atualizacao primeiro: e ali que a pagina reconta as colunas de cada
    // fileira (foco.nColunas), que e o que o focus_mover consulta.
    aberto = 1; saindo = 0; nivel = 1;
    foco.fileira = SEC_EPISODIOS;
    foco.coluna = 0;
    for (i = 0; i < 30; i++) detail_atualizar(1.0f / 60.0f, 1000 + i * 16);
    assert(secaoN(SEC_EPISODIOS) == 6 && foco.nColunas[SEC_EPISODIOS] == 6);
    e.type = SDL_KEYDOWN;
    for (i = 0; i < 5; i++) {
      e.key.keysym.sym = SDLK_RIGHT;
      detail_evento(&e);
      assert(foco.fileira == SEC_EPISODIOS && foco.coluna == i + 1);
    }
    e.key.keysym.sym = SDLK_RIGHT;   // na ponta: para
    detail_evento(&e);
    assert(foco.coluna == 5);
    e.key.keysym.sym = SDLK_LEFT;
    detail_evento(&e);
    assert(foco.coluna == 4);
  }
  assert(h[0] < h[1] && h[1] < h[2]);
  assert(th[0] < th[1] && th[1] < th[2]);
  assert(w[0] < w[1] && w[1] < w[2]);
  assert(p[0] < p[1] && p[1] < p[2]);
  assert(w[1] == 640.0f && th[1] == 360.0f && p[1] == 640.0f + NV_DETP_EP_GAP);

  // 4. O TITULO UM DEGRAU ABAIXO na Moderna: TXT_CALLOUT (28), e nao o
  //    TXT_PLR_CORPO (32) do Padrao.
  assert(epTituloEstilo() == TXT_CALLOUT);

  // 5. A PLACA do texto e mais estreita que a miniatura, centrada, e abriga o
  //    texto (que comeca em r.x+12 e +6 no foco) com folga, em repouso e em foco.
  { GfxRect r = {100, 50, epCardW(), epCardH()};
    for (i = 0; i <= 1; i++) {
      GfxRect pl = epPlacaRect(r, epThumbH(), (float)i);
      assert(pl.w < r.w);
      assert(pl.x > r.x && pl.x + pl.w < r.x + r.w);
      assert(fabsf((pl.x + pl.w * 0.5f) - (r.x + r.w * 0.5f)) < 0.01f);
      if (i) assert(pl.x + 6.0f <= r.x + 18.0f);   // em repouso nao ha placa
      if (i) assert(r.x + 18.0f + epTextoW() <= pl.x + pl.w);
    } }

  // Standard keeps its full-card focus plate: text overlays the thumbnail.
  gravar("homeLayoutLocal 1\n");
  assert(ajustes_home_layout() == HOME_LAYOUT_PADRAO);
  assert(epCardH() == epThumbH());
  { GfxRect r = {100, 50, epCardW(), epCardH()};
    for (i = 0; i <= 1; i++) {
      GfxRect pl = epPlacaRect(r, epThumbH(), (float)i);
      float scale = 1.0f + 0.03f * i;
      assert(fabsf(pl.w - (r.w + 2 * NV_DETP_EP_PLACA_PAD) * scale) < 0.01f);
      assert(fabsf(pl.h - (r.h + 2 * NV_DETP_EP_PLACA_PAD) * scale) < 0.01f);
      assert(fabsf(pl.x + pl.w * 0.5f - (r.x + r.w * 0.5f)) < 0.01f);
      assert(fabsf(pl.y + pl.h * 0.5f - (r.y + r.h * 0.5f)) < 0.01f);
    } }

  puts("detail_epcard: bases casadas, tres tamanhos, sinopse de 4 linhas e foco andando");
  return 0;
}
