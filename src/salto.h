// Regra do AVANCO SEGURADO (setas esquerda/direita do player), sem SDL nem
// estado global para poder ser testada com um fluxo de teclas simulado.
//
// O QUE ESTAVA ERRADO: o passo crescia por REPETICAO da tecla. O controle da
// C9 repete a cada ~100 ms, entao em 0,6 s cada repeticao ja valia 30 s e em
// 2,6 s valia 120 s: ~20 minutos por segundo. "As vezes voce quer pular so um
// pedacinho e ele ja voa pro final." E o ritmo de repeticao muda de TV e de
// controle (Samsung, Android), entao o mesmo gesto dava distancias diferentes.
//
// AGORA O RELOGIO MANDA, nao a contagem de teclas:
//  - o degrau vem do TEMPO desde a primeira tecla da rajada;
//  - no maximo um passo a cada SALTO_INTERVALO_MS; repeticoes extras entre
//    dois passos sao IGNORADAS (nao acumulam). 100 ms ou 40 ms de repeticao
//    dao a mesma distancia.
// Toque unico continua sendo exatamente 10 s.
#ifndef NUVIO_SALTO_H
#define NUVIO_SALTO_H

#define SALTO_SEG           10.0f

// SEGURAR = ESCALOES FIXOS DE VELOCIDADE (fork, 18/10). A rampa continua
// proporcional a duracao (#340) era imprevisivel: em 1,5 s o avanco passava de
// devagar a ~17 min por segundo, e nada na tela dizia a quanto se andava. Agora
// e como o Apple TV / Plex: a velocidade dobra a cada SALTO_NIVEL_MS segurando,
// e o escalao em vigor aparece na tela ("20x", "40x"...). A unidade e "segundos
// de video por segundo segurado", que e exatamente o "Nx" de quem ve.
#define SALTO_NIVEL_MS 1200u
static const float SALTO_NIVEIS[] = { 20.0f, 40.0f, 80.0f, 160.0f, 320.0f, 640.0f };
#define SALTO_N_NIVEIS ((int)(sizeof SALTO_NIVEIS / sizeof SALTO_NIVEIS[0]))
// Teto pela duracao: atravessar o arquivo inteiro nunca leva menos que isto
// (um episodio de 20 min nao chega a 640x — a barra so piscaria).
#define SALTO_TRAVESSIA_MIN_S 8.0f

// O escalao (0..N-1) de quem esta com a tecla ha `heldMs` numa midia de `dur`.
static inline int salto_nivel(unsigned heldMs, float duracaoSeg) {
  int n = (int)(heldMs / SALTO_NIVEL_MS), teto = 0;
  float vmax = duracaoSeg > 0.0f ? duracaoSeg / SALTO_TRAVESSIA_MIN_S : SALTO_NIVEIS[0];
  while (teto + 1 < SALTO_N_NIVEIS && SALTO_NIVEIS[teto + 1] <= vmax) teto++;
  return n < teto ? n : teto;
}

// ESTADO DA RAJADA e a regra de quem e toque e quem e tecla segurada.
//
// TOQUE SOLTO nunca e perdido: cada KEYDOWN novo aplica 10 s na hora e NAO
// inicia a rampa — cinco toques rapidos sao +50 s. So a REPETICAO de tecla
// segurada passa pelo limite de SALTO_INTERVALO_MS e pela rampa de tempo.
//
// Como saber que e repeticao, se cada plataforma entrega de um jeito: o
// teclado do SDL marca key.repeat; o firmware da TV manda a tecla segurada como
// KEYDOWNs SEPARADOS com repeat=0 (mesma observacao de app.c, issue #11), e a
// casca Tizen ainda despacha um par keydown+keyup por tecla, entao "descida sem
// subida" nao serve de criterio. Sobra o RELOGIO: repeticao de controle vem a
// ~100 ms (SDL de teclado, ~30 ms); o toque mais rapido de uma mao, ~150 ms.
// Logo, descida a menos de SALTO_REP_MS da anterior e repeticao. Abaixo de
// SALTO_REP_MIN_MS (dois eventos do mesmo quadro) nao existe mao nem controle:
// sao dois toques entregues juntos, e valem os dois.
#define SALTO_REP_MS      130u
#define SALTO_REP_MIN_MS   20u

typedef struct { unsigned inicio, ultimaTecla, ultimoPasso; int rep; } SaltoEst;

// `novo`: rajada nova (primeira tecla, ou a direcao mudou). `flagRepeat`:
// key.repeat do SDL. Devolve o passo em segundos (0 = ignorar a repeticao).
static inline float salto_tecla(SaltoEst *st, int novo, int flagRepeat,
                                unsigned agoraMs, float duracaoSeg) {
  unsigned gap = agoraMs - st->ultimaTecla;
  int rep = !novo && (flagRepeat || (gap >= SALTO_REP_MIN_MS && gap <= SALTO_REP_MS));
  st->ultimaTecla = agoraMs;
  st->rep = rep;
  if (!rep) {                       // toque: 10 s agora, rampa recomeca
    st->inicio = agoraMs; st->ultimoPasso = agoraMs;
    return SALTO_SEG;
  }
  // Repeticao (tecla segurada) nao soma passo: quem anda e salto_quadro.
  (void)duracaoSeg;
  return 0.0f;
}


// QUANDO O AVANCO TERMINA (#235). O avanco ja e "escolher e depois confirmar":
// video pausado, so a barra anda, UMA busca no fim, OK confirma na hora. O que
// faltava e o tempo de espera: 420 ms nao davam folga para olhar a miniatura
// do Seekr e acertar o ponto ("1 s de atraso antes de aplicar"). Com o Seekr
// ligado a espera e 1 s; sem miniatura nao ha o que olhar e ficam os 420 ms,
// para o filme voltar a tocar logo depois de um toque so.
// Fork: 700 ms entre toques ainda somam no mesmo salto ("+30 s"). A imagem
// para no primeiro toque e o player ja pre-carrega o alvo (player.c).
#define SALTO_FIM_MS        700u
#define SALTO_FIM_SEEKR_MS 1000u
static inline unsigned salto_fim_ms(int seekrLigado) {
  return seekrLigado ? SALTO_FIM_SEEKR_MS : SALTO_FIM_MS;
}


// AVANCO CONTINUO (2.0.3). O passo discreto (10 s ... 5 min a cada 300 ms) fazia
// a posicao andar aos pulos: a barra (mola) parava entre dois passos e o tempo
// escrito e a miniatura do Seekr, que liam a posicao crua, saltavam o passo
// inteiro de uma vez. A velocidade media nao muda (V0 = 10 s / 300 ms, e a
// rampa de salto_passo), mas a REPETICAO da tecla agora so mantem o avanco vivo
// e a posicao anda um pouco por quadro: salto_quadro().
// O toque (rep == 0) continua somando 10 s na hora: salto_tecla devolve o passo
// e o player so o aplica quando `st->rep == 0`.
#define SALTO_VIVO_MS 260u   // sem tecla ha mais que isso, o avanco para

// Velocidade (s de midia por s real) de quem esta com a tecla ha `heldMs`.
static inline float salto_vel(unsigned heldMs, float duracaoSeg) {
  return SALTO_NIVEIS[salto_nivel(heldMs, duracaoSeg)];
}
// O escalao em vigor para a TELA: -1 quando nao ha tecla segurada.
static inline int salto_nivel_atual(const SaltoEst *st, unsigned agoraMs, float duracaoSeg) {
  if (!st->rep || agoraMs - st->ultimaTecla > SALTO_VIVO_MS) return -1;
  return salto_nivel(agoraMs - st->inicio, duracaoSeg);
}

// Quanto a posicao anda neste quadro (segundos, ja com o sinal de `dir`).
static inline float salto_quadro(const SaltoEst *st, int dir, unsigned agoraMs,
                                 float dt, float duracaoSeg) {
  if (!st->rep || agoraMs - st->ultimaTecla > SALTO_VIVO_MS || dt <= 0.0f) return 0.0f;
  if (dt > 0.05f) dt = 0.05f;
  return dir * salto_vel(agoraMs - st->inicio, duracaoSeg) * dt;
}

#endif
