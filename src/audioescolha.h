// A ORDEM EM QUE O AUDIO E ESCOLHIDO SOZINHO (dono, 15/10/2026), em todos os
// alvos:
//   1. a faixa que a pessoa escolheu a mao neste titulo (o filme, ou a serie
//      inteira: os outros episodios tambem; troca de fonte, reconexao,
//      retomada, ida e volta do convertido). Fica so na memoria, nunca em
//      disco: fechar o app esquece;
//   2. o idioma de audio preferido (ling_audio);
//   3. o segundo idioma de audio (ling_audio2);
//   4. o idioma original do titulo (ling_original, do TMDB);
//   5. nada: fica a que o arquivo/player escolheu.
// Em cada idioma (2-4), uma faixa que nao e DTS/TrueHD ganha; DTS/TrueHD so
// quando e a unica naquele idioma. No Android (o FFmpeg do player decodifica
// tudo) nao ha essa regra: o alvo marca dts = 0 e vale a primeira do idioma. Uma faixa que o alvo nao consegue tocar
// (`pode` = 0: DTS/TrueHD no .wgt, ou no .tpk com a conversao desligada) nao
// entra. Quem toca a escolhida (converter ou nao) e o alvo.
#ifndef NV_AUDIOESCOLHA_H
#define NV_AUDIOESCOLHA_H

typedef struct {
  const char *idioma;   // "en"; "" sem etiqueta
  const char *codec;    // qualquer grafia (audioinfo_codec): "A_DTS", "eac3", "audio/ac3"
  const char *rotulo;   // o da lista ("Ingles · DTS-HD MA 5.1")
  int canais;           // 0 = nao se sabe
  int dts;              // DTS ou TrueHD/MLP (audioesc_dts_truehd)
  int pode;             // o alvo toca (ou converte) esta faixa
} AudioCand;

// De onde veio a escolha (para o log).
enum { AUDIOESC_NADA = 0, AUDIOESC_LEMBRADA, AUDIOESC_PREF1, AUDIOESC_PREF2, AUDIOESC_ORIGINAL };
const char *audioesc_origem_nome(int origem);

// 1 quando o codec/rotulo e DTS (qualquer variante) ou TrueHD/MLP.
int audioesc_dts_truehd(const char *codec, const char *rotulo);

// O titulo aberto agora (imdb da serie ou do filme, ou o nome). Outro titulo
// esquece a escolha lembrada; o mesmo a mantem.
void audioesc_titulo(const char *chave);
// A pessoa escolheu esta faixa a mao (folha de audio): vale ate outro titulo.
void audioesc_lembrar(const char *idioma, const char *codec, const char *rotulo, int canais);
// Ha escolha lembrada para o titulo atual?
int  audioesc_tem_lembrada(void);

// O indice escolhido entre `c[0..n)`, ou -1 (nao mexer). `origem` (opcional)
// diz o passo. Usa a escolha lembrada e as preferencias em vigor.
int audioesc_escolher(const AudioCand *c, int n, int *origem);

// O mesmo, puro, com tudo explicito (teste). `lembrada` pode ser NULL; prefs
// vazias ou repetidas sao puladas.
typedef struct { char idioma[16], codec[24], rotulo[48]; int canais; } AudioLembrada;
int audioesc_escolher_com(const AudioCand *c, int n, const AudioLembrada *lembrada,
                          const char *pref1, const char *pref2, const char *original, int *origem);
#endif
