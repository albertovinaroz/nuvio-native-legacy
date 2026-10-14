// CAPTURA DA FOLHA DE FONTES com as bandeiras de audio e de legenda. As fontes
// passam pelo parser de verdade (stream_extrair), entao a captura mostra o que
// o app desenha para cada formato de addon: o do Nuvio no iOS ("Languages 🇪🇸 •
// 🇬🇧 / Subtitles: ..."), Torrentio, AIOStreams, nome de release, `subtitles`.
//
// NAO ENTRA NA SUITE (tools/testa-tudo.sh pula *_shot.sh): precisa de janela GL
// e de olho humano. Nao chama dados_iniciar, como fontes_glifos_shot.c.
#include "streams.h"
#include "badges.h"
#include "gfx.h"
#include "text.h"
#include "tex_cache.h"
#include "ajustes.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void captura(const char *nome, SDL_Window *win) {
  for (int i = 0; i < 60; i++) {
    SDL_PumpEvents();
    txt_novo_quadro();
    tex_novo_quadro();
    tex_bombear(6);
    stream_folha_atualizar(1.0f / 60.0f, SDL_GetTicks());
    glClearColor(0.025f, 0.025f, 0.03f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    stream_folha_desenhar(SDL_GetTicks());
    if (i == 59) {
      unsigned char *pix = malloc(1920 * 1080 * 4);
      SDL_Surface *s;
      assert(pix);
      glReadPixels(0, 0, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, pix);
      s = SDL_CreateRGBSurfaceWithFormat(0, 1920, 1080, 32, SDL_PIXELFORMAT_RGBA32);
      assert(s);
      for (int y = 0; y < 1080; y++)
        memcpy((char *)s->pixels + y * s->pitch, pix + (1079 - y) * 1920 * 4, 1920 * 4);
      assert(SDL_SaveBMP(s, nome) == 0);
      SDL_FreeSurface(s);
      free(pix);
    }
    SDL_GL_SwapWindow(win);
  }
  printf("captura: %s\n", nome);
}

static void tecla(SDL_Keycode k) {
  SDL_Event e = { 0 };
  e.type = SDL_KEYDOWN;
  e.key.keysym.sym = k;
  stream_folha_evento(&e);
}

#define ES "\xF0\x9F\x87\xAA\xF0\x9F\x87\xB8"
#define GB "\xF0\x9F\x87\xAC\xF0\x9F\x87\xA7"
#define RU "\xF0\x9F\x87\xB7\xF0\x9F\x87\xBA"
#define FR "\xF0\x9F\x87\xAB\xF0\x9F\x87\xB7"
#define PONTO "\xE2\x80\xA2"

int main(int argc, char **argv) {
  const char *saida = argc > 1 ? argv[1] : "/tmp/nuvio-fontes-idiomas";
  char nome[600];
  SDL_Window *w;
  SDL_GLContext gl;
  Stream *v = NULL;
  int n;

  assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0);
  IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  w = SDL_CreateWindow("Nuvio: idiomas das fontes", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 1920, 1080,
                       SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  assert(w);
  gl = SDL_GL_CreateContext(w);
  assert(gl);
  SDL_GL_SetSwapInterval(0);
  glViewport(0, 0, 1920, 1080);
  gfx_tamanho_alvo(1920, 1080);
  assert(gfx_iniciar());
  assert(txt_iniciar("deploy/app", 1));
  tex_iniciar(64);
  gfx_icones_dir("deploy/app/art");
  badges_carregar("deploy/app/art");   // quem faz isto no app e home.c
  stream_dir_arte("deploy/app/art");

  n = stream_extrair("{\"streams\":["
    "{\"url\":\"https://exemplo.invalido/0.mkv\",\"name\":\"4K " PONTO " Instant\","
      "\"description\":\"WEB-DL\\nSize: 16.4 GB\\nDuration: 1h:48m:11s\\nLanguages " ES " " PONTO " " GB
      "\\nSubtitles: " ES " " PONTO " " GB "\","
      "\"behaviorHints\":{\"filename\":\"Animals.2025.2160p.AMZN.WEB-DL.DDP5.1.Atmos.DV.HDR10.H.265-GROUP.mkv\"}},"
    "{\"url\":\"https://exemplo.invalido/1.mkv\",\"name\":\"4K " PONTO " Instant\","
      "\"description\":\"WEB-DL\\nSize: 14.2 GB\\nLanguages " GB " " PONTO " " RU "\\nSubtitles: " ES " " PONTO " " GB "\"},"
    "{\"url\":\"https://exemplo.invalido/2.mp4\",\"name\":\"Torrentio\\n4k\","
      "\"title\":\"Animals.2025.2160p.WEB-DL.DV.HDR.Atmos.mp4\\n\xF0\x9F\x91\xA4 32 \xF0\x9F\x92\xBE 11.2 GB\\nMulti Audio / " ES " / " GB " / " FR "\"},"
    "{\"url\":\"https://exemplo.invalido/3.mkv\",\"name\":\"AIOStreams 1080p\","
      "\"description\":\"BluRay HEVC DDP5.1\\n\xF0\x9F\x8C\x8E English | Spanish | Japanese\"},"
    "{\"url\":\"https://exemplo.invalido/4.mkv\",\"name\":\"Release 1080p\","
      "\"description\":\"Animals.2025.1080p.WEB-DL.DUAL.CAST.LAT.ENG.SUBS.x264\"},"
    "{\"url\":\"https://exemplo.invalido/5.mkv\",\"name\":\"Com subtitles 720p\",\"subtitles\":["
      "{\"id\":\"1\",\"url\":\"https://exemplo.invalido/s1\",\"lang\":\"spa\"},"
      "{\"id\":\"2\",\"url\":\"https://exemplo.invalido/s2\",\"lang\":\"pt-BR\"},"
      "{\"id\":\"3\",\"url\":\"https://exemplo.invalido/s3\",\"lang\":\"kor\"}]},"
    "{\"url\":\"https://exemplo.invalido/6.mkv\",\"name\":\"Sem idioma 720p\",\"description\":\"WEBRip x264\"}"
    "]}", "Addon", &v);
  assert(n == 7);
  stream_definir_lista(v, n);
  free(v);
  stream_folha_nome("Animals");
  stream_folha_contexto("Animals");
  stream_folha_abrir();
  tecla(SDLK_DOWN);   // do grupo de botoes para a lista
  snprintf(nome, sizeof nome, "%s-1.bmp", saida);
  captura(nome, w);
  for (int i = 0; i < 4; i++) tecla(SDLK_DOWN);
  snprintf(nome, sizeof nome, "%s-2.bmp", saida);
  captura(nome, w);
  return 0;
}
