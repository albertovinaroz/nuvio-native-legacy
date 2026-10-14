// IDIOMA DE AUDIO DA PESSOA na folha de fontes (streams.c):
//   - escolha automatica: com preferencia (linguas.h), a fonte cujo addon
//     declara SO outros idiomas perde para a que tem o da pessoa, mesmo um
//     degrau de resolucao acima; fonte que nao declara idioma nao perde nada;
//   - filtro "Dublado": audio no idioma da pessoa ("lat" conta como espanhol);
//   - stream_idioma_codigo: etiquetas de faixa/legenda -> codigo da fonte.
// Compilado duas vezes pelo .sh: normal e -DNV_TPK (rotulo sem "DV" na Samsung).
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../src/streams.c"

static void fonte(Stream *s, const char *rot, int altura, const char *audio) {
  memset(s, 0, sizeof *s);
  snprintf(s->provedor, sizeof s->provedor, "Addon");
  snprintf(s->rotulo, sizeof s->rotulo, "%s", rot);
  snprintf(s->url, sizeof s->url, "https://h.invalid/v/%s.mkv", rot);
  s->altura = altura; s->fileIdx = -1;
  s->badges = badges_detectar(altura >= 2160 ? "2160p WEB-DL" : "1080p WEB-DL");
  for (const char *p = audio; p && *p && s->nIdiomasAudio < STREAM_IDIOMAS; ) {
    size_t n = strcspn(p, ",");
    snprintf(s->idiomasAudio[s->nIdiomasAudio++], sizeof s->idiomasAudio[0], "%.*s", (int)n, p);
    p += n + (p[n] == ',');
  }
}
static const char *vence(const Stream *l, int k) {
  int i;
  usleep(250000);   // a foto do StreamFit no automatico vale 200 ms
  stream_definir_alvo("tt1234567");
  stream_definir_lista(l, k);
  i = stream_automatico();
  assert(i >= 0);
  return stream_item(i)->rotulo;
}
static int falhas;
static void espera(const char *quem, const char *obtido, const char *caso) {
  if (strcmp(quem, obtido)) { printf("FAIL %s: venceu \"%s\", esperado \"%s\"\n", caso, obtido, quem); falhas++; }
  else printf("ok   %s\n", caso);
}
static void verdade(int cond, const char *caso) {
  if (!cond) { printf("FAIL %s\n", caso); falhas++; } else printf("ok   %s\n", caso);
}

int main(void) {
  Stream l[3];
  char dir[256];
  snprintf(dir, sizeof dir, "%s/nv-idpref-%d", getenv("TMPDIR") ? getenv("TMPDIR") : "/tmp", (int)getpid());
  { char m[300]; snprintf(m, sizeof m, "mkdir -p %s", dir); (void)!system(m); }
  ajustes_dir(dir);
  stream_definir_tela(1, 0);
  streamfit_limpar();

  fonte(&l[0], "4K ingles", 2160, "en");
  fonte(&l[1], "1080p espanhol", 1080, "es,en");
  ling_local_audio("");
  espera("4K ingles", vence(l, 2), "sem preferencia: a resolucao manda");
  ling_local_audio("es");
  espera("1080p espanhol", vence(l, 2), "preferencia es: 1080p em espanhol > 4K so em ingles");
  fonte(&l[1], "1080p latino", 1080, "lat");
  espera("1080p latino", vence(l, 2), "latino conta como espanhol");
  fonte(&l[0], "4K sem idioma", 2160, "");
  espera("4K sem idioma", vence(l, 2), "fonte que nao declara idioma nao perde");
  fonte(&l[0], "4K multi", 2160, "multi");
  espera("4K multi", vence(l, 2), "so 'multi' nao se sabe: nao perde");
  ling_local_audio("spa");
  fonte(&l[0], "4K ingles", 2160, "en");
  espera("1080p latino", vence(l, 2), "preferencia em ISO 639-2 (spa)");

  // Legenda no idioma da pessoa: segundo criterio, abaixo do audio.
  ling_local_audio("es");
  ling_local_legenda("es");
  fonte(&l[0], "4K ingles", 2160, "en");
  fonte(&l[1], "4K ingles leg es", 2160, "en");
  snprintf(l[1].idiomasLegenda[0], 6, "es"); l[1].nIdiomasLegenda = 1;
  espera("4K ingles leg es", vence(l, 2), "sem audio dela: a legenda dela desempata");
  fonte(&l[2], "1080p espanhol", 1080, "es");
  espera("1080p espanhol", vence(l, 3), "audio dela > legenda dela, mesmo um degrau abaixo");
  ling_local_legenda("");

  // Filtro "Dublado" percorre: desligado -> meu idioma -> idiomas da lista.
  { Stream f[3];
    fonte(&f[0], "a", 1080, "en"); fonte(&f[1], "b", 1080, "lat"); fonte(&f[2], "c", 1080, "en,es");
    stream_definir_lista(f, 3);
    soDub = 0;
    filtroIdiomaProximo(); verdade(soDub == 1, "filtro: 1o OK = meu idioma");
    filtroIdiomaProximo(); verdade(soDub == 2 && !strcmp(filtroCod, "en"), "filtro: 2o OK = o mais declarado (en)");
    verdade(passaChips(0) && !passaChips(1) && passaChips(2), "filtro en: so as com ingles");
    filtroIdiomaProximo(); verdade(soDub == 2 && !strcmp(filtroCod, "lat"), "filtro: 3o OK = latino");
    verdade(!passaChips(0) && passaChips(1) && !passaChips(2), "filtro lat: so a latina");
    filtroIdiomaProximo(); verdade(soDub == 2 && !strcmp(filtroCod, "es"), "filtro: 4o OK = castellano");
    filtroIdiomaProximo(); verdade(soDub == 0, "filtro: depois do ultimo idioma, desliga");
    verdade(!strcmp(nomeIdiomaFonte("lat"), "Espanhol latino"), "nome do latino"); }

  // Filtro "Dublado": audio no idioma da pessoa.
  ling_local_audio("es");
  fonte(&l[0], "4K ingles", 2160, "en");
  fonte(&l[1], "1080p latino", 1080, "lat");
  verdade(temAudioMeu(&l[1]) && !temAudioMeu(&l[0]), "filtro: latino entra, so ingles fica de fora");
  verdade(!filtroEmPortugues(), "filtro em espanhol usa o texto 'no seu idioma'");
  ling_local_audio("pt");
  { Stream d; fonte(&d, "Filme Dublado", 1080, ""); verdade(temAudioMeu(&d), "portugues: 'Dublado' sem idioma declarado ainda entra"); }

  // Etiquetas de faixa e de legenda.
  verdade(!strcmp(stream_idioma_codigo("spa"), "es") && !strcmp(stream_idioma_codigo("pt-BR"), "ptbr") &&
          !strcmp(stream_idioma_codigo("es-419"), "lat") && !strcmp(stream_idioma_codigo("en-US"), "en") &&
          !strcmp(stream_idioma_codigo("Spanish"), "es") && !stream_idioma_codigo("und"),
          "stream_idioma_codigo: spa, pt-BR, es-419, en-US, Spanish, und");

  // Rotulo de qualidade de uma fonte Dolby Vision perfil 8 e perfil 5.
  { Stream d; char res[16], fx[16]; int hd;
    fonte(&d, "DV p8", 2160, "en"); d.dolbyVision = 1; snprintf(d.descricao, sizeof d.descricao, "DV Profile 8 HDR10");
    rotuloQualidade(&d, res, sizeof res, fx, sizeof fx, &hd);
#ifdef NV_TPK
    verdade(!strcmp(fx, "HDR10") && !dvSemBase(&d), "Samsung: DV perfil 8 mostra HDR10, sem aviso");
    snprintf(d.descricao, sizeof d.descricao, "DV Profile 5"); d.badges = badges_detectar("2160p DV");
    rotuloQualidade(&d, res, sizeof res, fx, sizeof fx, &hd);
    verdade(strcmp(fx, "DV") && dvSemBase(&d) == 2 && !(logosDa(&d, 0) & badges_bit("v-dv")),
            "Samsung: DV perfil 5 sem 'DV', sem logo DV e com aviso de cor");
    d.badges = badges_detectar("2160p DTS-HD MA 7.1");
    verdade(semSomNaTv(&d), "Samsung: so DTS-HD avisa 'Sem som'");
    d.badges = badges_detectar("2160p DTS-HD MA DDP5.1");
    verdade(!semSomNaTv(&d), "Samsung: DTS com DD+ junto toca, sem aviso");
#else
    verdade(!strcmp(fx, "DV") && !dvSemBase(&d), "fora da Samsung: DV continua DV");
#endif
  }
  ling_local_audio("");
  printf(falhas ? "%d falha(s)\n" : "tudo ok\n", falhas);
  return falhas != 0;
}
