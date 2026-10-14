#include "streams.h"
#include "badges.h"
#include "js.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <math.h>

// Addons tambem mandam numeros como strings. Evita conversao indefinida de
// infinito ou expoentes enormes a int/long, preservando tamanho desconhecido.
static long tamanhoMB(double valor) {
  return isfinite(valor) && valor > 0 && valor < (double)LONG_MAX ? (long)valor : 0;
}

// Find only a direct property of this exact object. JSON strings and nested
// objects are skipped; proxyHeaders/request cannot masquerade as videoSize.
static const char *valorRaiz(const char *p, const char *fim, const char *nome) {
  int depth = 0;
  size_t n = strlen(nome);
  if (!p || p >= fim || *p != '{') return NULL;
  while (p < fim) {
    if (*p == '"') {
      const char *ini = ++p;
      while (p < fim && *p != '"') {
        if (*p == '\\' && p + 1 < fim) p++;
        p++;
      }
      if (p >= fim) return NULL;
      const char *apos = p + 1;
      while (apos < fim && isspace((unsigned char)*apos)) apos++;
      if (depth == 1 && (size_t)(p - ini) == n && !memcmp(ini, nome, n) && apos < fim && *apos == ':') {
        apos++; while (apos < fim && isspace((unsigned char)*apos)) apos++;
        return apos < fim ? apos : NULL;
      }
    } else if (*p == '{' || *p == '[') depth++;
    else if (*p == '}' || *p == ']') { if (--depth == 0) break; }
    p++;
  }
  return NULL;
}

// Decimal integers only, parsed without double rounding. Numeric strings
// are accepted for compatibility, but fractions/exponents/overflow are
// unavailable for fit. Legacy display size remains separate below.
static uint64_t bytesExatos(const char *obj, const char *fim) {
  const char *bh = valorRaiz(obj, fim, "behaviorHints"), *p, *bf;
  uint64_t valor = 0;
  int quoted;
  if (!bh || *bh != '{' || !(bf = js_fim(bh)) || bf > fim) return 0;
  p = valorRaiz(bh, bf, "videoSize");
  if (!p) return 0;
  quoted = *p == '"'; if (quoted) p++;
  if (p >= bf || !isdigit((unsigned char)*p)) return 0;
  if (!quoted && *p == '0' && p + 1 < bf && isdigit((unsigned char)p[1])) return 0;
  while (p < bf && isdigit((unsigned char)*p)) {
    unsigned d = (unsigned)(*p++ - '0');
    if (valor > (STREAMFIT_BYTES_MAX - d) / 10) return 0;
    valor = valor * 10 + d;
  }
  if (quoted) { if (p >= bf || *p++ != '"') return 0; }
  while (p < bf && isspace((unsigned char)*p)) p++;
  return p < bf && (*p == ',' || *p == '}') ? valor : 0;
}

static int contem(const char *s, const char *termo) {
  for (; *s; s++) if (!strncasecmp(s, termo, strlen(termo))) return 1;
  return 0;
}

// "FHD"/"Full HD" como ROTULO SOLTO do formatador (#402): AIOStreams e afins
// escrevem "FHD | REMUX | SDR" ou "FHD \u2022 REMUX". Aceita so se:
//  - os dois lados sao inicio/fim de linha, espaco, '|', '\u2022' ou '\u00b7'
//    (nunca '.', '_', '-', '/', '[' — isso e nome de arquivo ou URL);
//  - e o primeiro rotulo da linha, ou encosta num '|' / bullet (pulando
//    espaco). "The FHD Story" e frase, nao rotulo;
//  - o texto nao tem "://".
// Nao o "hd" solto do nv_res_do_texto ("DTS-HD" viraria 720).
static int ehSepFhd(const char *a, const char *ini, int antes) {
  const unsigned char *u = (const unsigned char *)a;
  if (antes) {
    if (a == ini || u[-1] == '\n' || u[-1] == ' ' || u[-1] == '\t' || u[-1] == '|') return 1;
    if (a - ini >= 3 && u[-3] == 0xE2 && u[-2] == 0x80 && u[-1] == 0xA2) return 1;
    return a - ini >= 2 && u[-2] == 0xC2 && u[-1] == 0xB7;
  }
  return !*u || *u == '\n' || *u == ' ' || *u == '\t' || *u == '|' ||
         (u[0] == 0xE2 && u[1] == 0x80 && u[2] == 0xA2) || (u[0] == 0xC2 && u[1] == 0xB7);
}
// Prefixo da linha feito so de SIMBOLO/emoji e espaco, e curto (ate 2 simbolos):
// "\u23f3 FHD", "\u26a1 FHD". So os blocos de simbolo do UTF-8 contam — E2 80..AF xx
// (U+2000..U+2BFF: setas, relogios, raios, estrelas; de U+2C00 em diante ha letras,
// Glagolitico e Georgiano; U+2E00..U+2FFF tem pontuacao e radicais, recusados por
// seguranca) e F0 9F xx xx (emoji), com EF B8 8F
// (seletor de variacao) junto. Letra de outro alfabeto (Cirilico D0/D1, CJK
// E3..E9) e palavra, nao rotulo: "Фильм FHD" e "我的 FHD" continuam frase.
static int prefixoSoSimbolos(const char *ini, const char *fim) {
  const unsigned char *u = (const unsigned char *)ini, *f = (const unsigned char *)fim;
  int simbolos = 0;
  while (u < f) {
    if (*u == ' ' || *u == '\t') { u++; continue; }
    if (f - u >= 3 && u[0] == 0xEF && u[1] == 0xB8 && u[2] == 0x8F) { u += 3; continue; }
    if (f - u >= 3 && u[0] == 0xE2 && u[1] >= 0x80 && u[1] <= 0xAF && (u[2] & 0xC0) == 0x80) u += 3;
    else if (f - u >= 4 && u[0] == 0xF0 && u[1] == 0x9F && (u[2] & 0xC0) == 0x80 &&
             (u[3] & 0xC0) == 0x80) u += 4;
    else return 0;
    if (++simbolos > 2) return 0;
  }
  return simbolos > 0;
}
static int ehRotuloSep(const unsigned char *u) {   // '|' ou bullet comecando em u
  return *u == '|' || (u[0] == 0xE2 && u[1] == 0x80 && u[2] == 0xA2) ||
         (u[0] == 0xC2 && u[1] == 0xB7);
}
static int fhdNoNome(const char *s) {
  static const char *const t[] = {"fhd", "fullhd", "full hd", "full-hd"};
  if (strstr(s, "://")) return 0;
  for (const char *p = s; *p; p++)
    for (size_t i = 0; i < sizeof t / sizeof t[0]; i++) {
      size_t n = strlen(t[i]);
      if (strncasecmp(p, t[i], n) || !ehSepFhd(p, s, 1) || !ehSepFhd(p + n, s, 0)) continue;
      const char *a = p, *d = p + n;
      while (a > s && (a[-1] == ' ' || a[-1] == '\t')) a--;
      while (*d == ' ' || *d == '\t') d++;
      if (a == s || a[-1] == '\n' || ehRotuloSep((const unsigned char *)d)) return 1;
      if (a[-1] == '|' ||
          (a - s >= 3 && (unsigned char)a[-1] == 0xA2 && (unsigned char)a[-2] == 0x80 &&
           (unsigned char)a[-3] == 0xE2) ||
          (a - s >= 2 && (unsigned char)a[-1] == 0xB7 && (unsigned char)a[-2] == 0xC2)) return 1;
      // So simbolos (emoji, bytes >= 0x80) e espaco antes na linha: "\u23f3 FHD" do
      // AIOStreams e rotulo; uma palavra ASCII antes ("The FHD") continua frase.
      { const char *ls = a;
        while (ls > s && ls[-1] != '\n') ls--;
        if (prefixoSoSimbolos(ls, a)) return 1; }
    }
  return 0;
}

// Token isolado reconhece .DV., DV/HDR e [DV], mas nunca DVD/DVDRip.
static int token(const char *s, const char *t) {
  size_t n = strlen(t);
  const char *p;
  for (p = s; *p; p++)
    if ((p == s || !isalnum((unsigned char)p[-1])) &&
        !strncasecmp(p, t, n) && !isalnum((unsigned char)p[n])) return 1;
  return 0;
}

// CABECALHOS EXIGIDOS PELO ADDON: behaviorHints.proxyHeaders.request.
//
// MEDIDO em 17/09 contra o addon de um relato: o CDN devolve 403 sem
// Referer/Origin/User-Agent e 200 com eles. O parser entrava em behaviorHints
// so para pegar bingeGroup, entao esses cabecalhos eram descartados e QUALQUER
// addon que dependa de Referer ficava sem tocar, nos dois alvos.
//
// A MAO, e nao com js_texto: os nomes das chaves aqui sao escolhidos pelo
// ADDON (sao nomes de cabecalho HTTP), entao nao ha lista de chaves conhecidas
// para procurar — e preciso varrer os pares que existirem. So profundidade 1
// de "request", que e onde a convencao do Stremio poe os cabecalhos.
//
// Saida no formato que rede.h aceita: uma linha "Nome: valor" por cabecalho.
static void lerProxyHeaders(const char *bh, const char *fim, char *dst, unsigned tam) {
  const char *ph, *rq, *q;
  unsigned u = 0;
  dst[0] = 0;
  ph = strstr(bh, "\"proxyHeaders\"");
  if (!ph || ph >= fim) return;
  rq = strstr(ph, "\"request\"");
  if (!rq || rq >= fim) return;
  q = strchr(rq, '{');
  if (!q || q >= fim) return;
  q++;
  // Varre pares "nome":"valor" ate a chave que fecha o objeto. Valor de
  // cabecalho e string, entao um '{' aqui e coisa que nao se esperava: encerra
  // em vez de tentar adivinhar.
  while (q < fim && *q && *q != '}' && *q != '{') {
    char nome[96], valor[320];
    unsigned k;
    const char *f;
    while (q < fim && *q && *q != '"' && *q != '}') q++;
    if (q >= fim || *q != '"') break;
    q++;
    f = q;
    while (f < fim && *f && *f != '"') f++;
    if (f >= fim || *f != '"') break;
    k = (unsigned)(f - q);
    if (k >= sizeof nome) k = sizeof nome - 1;
    memcpy(nome, q, k); nome[k] = 0;
    q = f + 1;
    while (q < fim && *q && *q != ':') q++;
    if (q >= fim || *q != ':') break;
    q++;
    while (q < fim && (*q == ' ' || *q == '\t')) q++;
    if (q >= fim || *q != '"') break;   // valor nao-string: nao e cabecalho
    q++;
    f = q;
    while (f < fim && *f && *f != '"') f++;
    if (f >= fim || *f != '"') break;
    k = (unsigned)(f - q);
    if (k >= sizeof valor) k = sizeof valor - 1;
    memcpy(valor, q, k); valor[k] = 0;
    q = f + 1;
    if (nome[0] && valor[0]) {
      int esc = snprintf(dst + u, tam - u, "%s%s: %s", u ? "\n" : "", nome, valor);
      if (esc < 0 || (unsigned)esc >= tam - u) { dst[u] = 0; break; }
      u += (unsigned)esc;
    }
    while (q < fim && (*q == ' ' || *q == '\t' || *q == '\n' || *q == '\r')) q++;
    if (q < fim && *q == ',') q++;
  }
}

// "FORA DE CACHE" DITO PELO ADDON. As marcas sao as que os addons mandam de
// verdade: "⏳" e o que o AIOStreams poe no nome de toda fonte nao cacheada
// (registros 1136, 2191, 2501: "⏳ UHD", "⏳ FHD"; a cacheada vem com "⚡"), o
// Torrentio escreve "[TB download]" / "[RD download]" (a cacheada e "[TB+]"),
// e outros usam "⬇" ou a palavra "uncached". "download]" e nao "download"
// solto: "WEB-DL"/"Download" aparecem em titulo de fonte cacheada.
int stream_texto_fora_de_cache(const char *t) {
  if (!t) return 0;
  return strstr(t, "\xe2\x8f\xb3") != NULL          // U+23F3 ⏳
      || strstr(t, "\xe2\xac\x87") != NULL          // U+2B07 ⬇
      || contem(t, " download]") || contem(t, "uncached") || contem(t, "not cached");
}

// "sources": ["tracker:udp://...", "dht:<hash>"] de um stream de torrent, uma
// entrada por linha em `dst`. So as que o servidor de streaming do Stremio
// entende (prefixo tracker: ou dht:); uma URL solta de tracker ganha o prefixo.
// A entrada que nao cabe inteira fica de fora (nunca cortada no meio: um
// tracker pela metade e endereco torto). \/ vira / (JSON escapa a barra).
static void lerFontesP2P(const char *ini, const char *fim, char *dst, unsigned tam) {
  const char *q = strstr(ini, "\"sources\"");
  unsigned u = 0;
  dst[0] = 0;
  if (!q || q >= fim) return;
  q += 9;
  while (q < fim && (*q == ' ' || *q == ':' || *q == '\n' || *q == '\t')) q++;
  if (q >= fim || *q != '[') return;
  q++;
  while (q < fim && *q != ']') {
    char e[300];
    unsigned k = 0;
    if (*q != '"') { q++; continue; }
    q++;
    while (q < fim && *q != '"' && k + 1 < sizeof e) {
      if (*q == '\\' && q + 1 < fim) q++;      // \/ -> /
      e[k++] = *q++;
    }
    e[k] = 0;
    while (q < fim && *q != '"') q++;          // entrada maior que o buffer: pula o resto
    if (q < fim) q++;
    if (!strncmp(e, "tracker:", 8) || !strncmp(e, "dht:", 4) ||
        !strncmp(e, "udp://", 6) || !strncmp(e, "http://", 7) || !strncmp(e, "https://", 8)) {
      char ent[320];
      int L = snprintf(ent, sizeof ent, "%s%s",
                       strncmp(e, "tracker:", 8) && strncmp(e, "dht:", 4) ? "tracker:" : "", e);
      if (L > 0 && u + (unsigned)L + 2 <= tam) {
        u += (unsigned)snprintf(dst + u, tam - u, "%s%s", u ? "\n" : "", ent);
      }
    }
  }
}

// SEMEADORES no texto do addon. Nao ha campo para isso no protocolo: cada
// addon escreve do seu jeito, sempre marcador + numero. Torrentio e
// MediaFusion usam "👤 12", Comet/Jackettio "👥 12", outros "Seeders: 12".
// Devolve 1 e preenche *qtd; limite de 9 digitos para nao estourar o int.
static int lerSemeadores(const char *t, int *qtd) {
  static const char *const marcas[] = { "\xF0\x9F\x91\xA4", "\xF0\x9F\x91\xA5", "\xF0\x9F\x8C\xB1",
                                        "Seeders", "seeders", "Seeds", "seeds" };
  for (size_t k = 0; k < sizeof marcas / sizeof *marcas; k++) {
    const char *p = strstr(t, marcas[k]);
    if (!p) continue;
    p += strlen(marcas[k]);
    while (*p == ' ' || *p == ':' || *p == '\t') p++;
    if (!isdigit((unsigned char)*p)) continue;
    long v = 0; int d = 0;
    while (isdigit((unsigned char)*p) && d < 9) { v = v * 10 + (*p++ - '0'); d++; }
    *qtd = (int)v;
    return 1;
  }
  return 0;
}

// --- IDIOMAS DE AUDIO E DE LEGENDA (as bandeiras da folha) -------------------
//
// O stream do Stremio nao tem campo de idioma de audio: cada addon escreve do
// seu jeito, e e do texto que sai. O que se le:
//   - bandeira emoji ("Languages 🇪🇸 • 🇬🇧", "🇬🇧 / 🇮🇹" do Torrentio), pais ->
//     idioma (🇲🇽 e latino, 🇧🇷 portugues do Brasil);
//   - nome por extenso ("English | Spanish"), em ingles, espanhol, portugues,
//     frances, italiano e alemao, comparado sem acento;
//   - sigla de release so em CAIXA ALTA ("SPA.ENG", "CAST", "LAT"): em
//     minuscula "ita", "por" e "dan" sao palavras de gente;
//   - o array `subtitles` do stream, o unico campo estruturado: e legenda.
//
// LEGENDA NO TEXTO, duas formas. A clausula que ABRE com a marca ("Subtitles:
// 🇪🇸 • 🇬🇧", "💬 English") e legenda ate o fim; "Audio:"/"Languages" abrem
// audio. Fora disso, o idioma COLADO numa marca ("Sub Esp", "ENG.SUBS",
// "Spanish Subtitles", "multi-subs") e legenda. Clausula = linha, ou trecho
// entre "|", ";" e colchetes; "•", "/" e virgula separam itens da mesma lista.
//
// O TITULO NAO CONTA: num nome de release o titulo vem antes do ano, da
// resolucao ou do SxxEyy, e "The.Italian.Job.2003.1080p.ITA" e italiano pelo
// ITA, nao pelo Italian. Palavra antes do primeiro desses marcos e ignorada.
typedef struct { const char *termo, *cod; } IdiomaTermo;
static const IdiomaTermo IDIOMA_NOMES[] = {
  { "english", "en" }, { "ingles", "en" }, { "inglese", "en" }, { "anglais", "en" },
  { "englisch", "en" },
  { "spanish", "es" }, { "espanol", "es" }, { "castellano", "es" }, { "castilian", "es" },
  { "espanhol", "es" }, { "espagnol", "es" }, { "spagnolo", "es" },
  { "latino", "lat" }, { "latam", "lat" }, { "latinoamericano", "lat" },
  { "portuguese", "pt" }, { "portugues", "pt" }, { "portugais", "pt" }, { "portoghese", "pt" },
  { "brazilian", "ptbr" }, { "brasileiro", "ptbr" }, { "ptbr", "ptbr" },
  { "french", "fr" }, { "frances", "fr" }, { "francais", "fr" }, { "francese", "fr" },
  { "german", "de" }, { "aleman", "de" }, { "alemao", "de" }, { "deutsch", "de" },
  { "allemand", "de" }, { "tedesco", "de" },
  { "italian", "it" }, { "italiano", "it" }, { "italien", "it" },
  { "japanese", "ja" }, { "japones", "ja" }, { "japonais", "ja" }, { "giapponese", "ja" },
  { "korean", "ko" }, { "coreano", "ko" }, { "coreen", "ko" },
  { "chinese", "zh" }, { "chino", "zh" }, { "chines", "zh" }, { "mandarin", "zh" },
  { "cantonese", "zh" },
  { "russian", "ru" }, { "ruso", "ru" }, { "russo", "ru" }, { "russe", "ru" },
  { "ukrainian", "uk" }, { "ucraniano", "uk" },
  { "polish", "pl" }, { "polaco", "pl" }, { "polones", "pl" },
  { "turkish", "tr" }, { "turco", "tr" },
  { "dutch", "nl" }, { "holandes", "nl" }, { "nederlands", "nl" },
  { "swedish", "sv" }, { "sueco", "sv" },
  { "norwegian", "no" }, { "noruego", "no" },
  { "danish", "da" }, { "danes", "da" },
  { "finnish", "fi" }, { "finlandes", "fi" },
  { "greek", "el" }, { "griego", "el" }, { "grego", "el" },
  { "czech", "cs" }, { "checo", "cs" }, { "tcheco", "cs" },
  { "hungarian", "hu" }, { "hungaro", "hu" },
  { "romanian", "ro" }, { "rumano", "ro" }, { "romeno", "ro" },
  { "hebrew", "he" }, { "hebreo", "he" }, { "hebraico", "he" },
  { "thai", "th" }, { "tailandes", "th" },
  { "vietnamese", "vi" }, { "vietnamita", "vi" },
  { "indonesian", "id" }, { "indonesio", "id" },
  { "hindi", "hi" }, { "tamil", "ta" }, { "telugu", "te" },
  { "arabic", "ar" }, { "arabe", "ar" },
  { "bulgarian", "bg" }, { "bulgaro", "bg" },
  { "croatian", "hr" }, { "croata", "hr" },
  { "serbian", "sr" }, { "serbio", "sr" },
  { "slovak", "sk" }, { "eslovaco", "sk" },
  { "slovenian", "sl" }, { "esloveno", "sl" },
  { "multi", "multi" }, { "multilang", "multi" }, { "multilanguage", "multi" },
};
// Siglas de release: so casam escritas em caixa alta no original.
static const IdiomaTermo IDIOMA_SIGLAS[] = {
  { "ENG", "en" }, { "SPA", "es" }, { "ESP", "es" }, { "CAST", "es" }, { "LAT", "lat" },
  { "POR", "pt" }, { "FRE", "fr" }, { "FRA", "fr" }, { "GER", "de" }, { "DEU", "de" },
  { "ITA", "it" }, { "JPN", "ja" }, { "JAP", "ja" }, { "KOR", "ko" }, { "CHI", "zh" },
  { "ZHO", "zh" }, { "RUS", "ru" }, { "UKR", "uk" }, { "POL", "pl" }, { "TUR", "tr" },
  { "DUT", "nl" }, { "NLD", "nl" }, { "SWE", "sv" }, { "NOR", "no" }, { "DAN", "da" },
  { "FIN", "fi" }, { "GRE", "el" }, { "CZE", "cs" }, { "HUN", "hu" }, { "RUM", "ro" },
  { "RON", "ro" }, { "HEB", "he" }, { "THA", "th" }, { "VIE", "vi" }, { "HIN", "hi" },
  { "TAM", "ta" }, { "TEL", "te" }, { "ARA", "ar" }, { "BUL", "bg" }, { "HRV", "hr" },
  { "SRP", "sr" }, { "SLK", "sk" }, { "SLV", "sl" },
};
// Codigo do campo `lang` das legendas (ISO 639-1/2, BCP 47): minusculo, exato.
static const IdiomaTermo IDIOMA_CODIGOS[] = {
  { "en", "en" }, { "eng", "en" }, { "es", "es" }, { "spa", "es" }, { "esp", "es" },
  { "es-419", "lat" }, { "es-mx", "lat" }, { "ea", "lat" }, { "pt", "pt" }, { "por", "pt" },
  { "pt-pt", "pt" }, { "pt-br", "ptbr" }, { "pob", "ptbr" }, { "pb", "ptbr" },
  { "fr", "fr" }, { "fre", "fr" }, { "fra", "fr" }, { "de", "de" }, { "ger", "de" },
  { "deu", "de" }, { "it", "it" }, { "ita", "it" }, { "ja", "ja" }, { "jpn", "ja" },
  { "ko", "ko" }, { "kor", "ko" }, { "zh", "zh" }, { "chi", "zh" }, { "zho", "zh" },
  { "ze", "zh" }, { "zt", "zh" }, { "ru", "ru" }, { "rus", "ru" }, { "uk", "uk" },
  { "ukr", "uk" }, { "pl", "pl" }, { "pol", "pl" }, { "tr", "tr" }, { "tur", "tr" },
  { "nl", "nl" }, { "dut", "nl" }, { "nld", "nl" }, { "sv", "sv" }, { "swe", "sv" },
  { "no", "no" }, { "nor", "no" }, { "nb", "no" }, { "da", "da" }, { "dan", "da" },
  { "fi", "fi" }, { "fin", "fi" }, { "el", "el" }, { "gre", "el" }, { "ell", "el" },
  { "cs", "cs" }, { "cze", "cs" }, { "ces", "cs" }, { "hu", "hu" }, { "hun", "hu" },
  { "ro", "ro" }, { "rum", "ro" }, { "ron", "ro" }, { "he", "he" }, { "heb", "he" },
  { "th", "th" }, { "tha", "th" }, { "vi", "vi" }, { "vie", "vi" }, { "id", "id" },
  { "ind", "id" }, { "hi", "hi" }, { "hin", "hi" }, { "ta", "ta" }, { "tam", "ta" },
  { "te", "te" }, { "tel", "te" }, { "ar", "ar" }, { "ara", "ar" }, { "bg", "bg" },
  { "bul", "bg" }, { "hr", "hr" }, { "hrv", "hr" }, { "sr", "sr" }, { "srp", "sr" },
  { "sk", "sk" }, { "slk", "sk" }, { "slo", "sk" }, { "sl", "sl" }, { "slv", "sl" },
};
// Bandeira -> idioma. A de pais que nao esta aqui nao vira idioma.
static const IdiomaTermo IDIOMA_PAISES[] = {
  { "GB", "en" }, { "US", "en" }, { "AU", "en" }, { "CA", "en" }, { "IE", "en" },
  { "NZ", "en" }, { "ES", "es" }, { "MX", "lat" }, { "AR", "lat" }, { "CO", "lat" },
  { "CL", "lat" }, { "PE", "lat" }, { "VE", "lat" }, { "PT", "pt" }, { "BR", "ptbr" },
  { "FR", "fr" }, { "BE", "fr" }, { "DE", "de" }, { "AT", "de" }, { "IT", "it" },
  { "JP", "ja" }, { "KR", "ko" }, { "CN", "zh" }, { "TW", "zh" }, { "HK", "zh" },
  { "RU", "ru" }, { "UA", "uk" }, { "PL", "pl" }, { "TR", "tr" }, { "NL", "nl" },
  { "SE", "sv" }, { "NO", "no" }, { "DK", "da" }, { "FI", "fi" }, { "GR", "el" },
  { "CZ", "cs" }, { "HU", "hu" }, { "RO", "ro" }, { "IL", "he" }, { "TH", "th" },
  { "VN", "vi" }, { "ID", "id" }, { "IN", "hi" }, { "SA", "ar" }, { "AE", "ar" },
  { "EG", "ar" }, { "BG", "bg" }, { "HR", "hr" }, { "RS", "sr" }, { "SK", "sk" },
  { "SI", "sl" },
};
static const char *const MARCAS_LEGENDA[] = {
  "sub", "subs", "subbed", "subtitle", "subtitles", "subtitulo", "subtitulos",
  "subtitulado", "subtitulada", "legenda", "legendas", "legendado", "legendada",
  "sottotitoli", "untertitel", "cc",
};
static const char *const MARCAS_AUDIO[] = {
  "audio", "audios", "language", "languages", "lang", "langs", "idioma", "idiomas",
  "dub", "dubbed", "dublado", "doblado", "lingua",
};
#define IDIOMA_N(v) (sizeof (v) / sizeof *(v))

static const char *idiomaTabela(const IdiomaTermo *t, size_t n, const char *p) {
  for (size_t i = 0; i < n; i++) if (!strcmp(t[i].termo, p)) return t[i].cod;
  return NULL;
}
static int idiomaNaLista(const char *const *v, size_t n, const char *p) {
  for (size_t i = 0; i < n; i++) if (!strcmp(v[i], p)) return 1;
  return 0;
}
static void idiomaPor(char lista[][6], unsigned char *n, const char *cod) {
  for (unsigned i = 0; i < *n; i++) if (!strcmp(lista[i], cod)) return;
  if (*n >= STREAM_IDIOMAS) return;
  snprintf(lista[*n], sizeof lista[0], "%s", cod);
  (*n)++;
}

enum { TK_FIM, TK_PALAVRA, TK_BANDEIRA, TK_QUEBRA, TK_EMOJI_LEG, TK_EMOJI_AUDIO };
typedef struct {
  int tipo;
  char norm[24];   // minuscula sem acento (palavra) ou o pais ("ES", bandeira)
  int caixaAlta;   // a palavra veio toda em maiuscula no texto, com 3+ letras
  int doisPontos;  // logo depois dela vem ":" ("Subtitles:")
} IdiomaTk;

// Proximo token a partir de *p. Separadores de item (espaco, ponto, hifen,
// virgula, "/", "•", "·", parenteses, emoji que nao e marca) sao pulados; o
// que fecha clausula vira TK_QUEBRA.
static void idiomaToken(const char **pp, IdiomaTk *tk) {
  static const char *BASE = "aaaaaaaceeeeiiiidnooooo*ouuuuy";  // C3 80..9E, ver fontepref.c
  const unsigned char *p = (const unsigned char *)*pp;
  memset(tk, 0, sizeof *tk);
  for (;;) {
    if (!*p) { tk->tipo = TK_FIM; break; }
    if (*p == '\n' || *p == '\r' || *p == '|' || *p == ';' || *p == '[' || *p == ']') {
      tk->tipo = TK_QUEBRA; p++; break;
    }
    if (p[0] == 0xF0 && p[1] == 0x9F && p[2] == 0x87 && p[3] >= 0xA6 && p[3] <= 0xBF &&
        p[4] == 0xF0 && p[5] == 0x9F && p[6] == 0x87 && p[7] >= 0xA6 && p[7] <= 0xBF) {
      tk->tipo = TK_BANDEIRA;
      tk->norm[0] = (char)('A' + (p[3] - 0xA6));
      tk->norm[1] = (char)('A' + (p[7] - 0xA6));
      p += 8; break;
    }
    if (p[0] == 0xF0 && p[1] && p[2] && p[3]) {
      // 💬 💭 📝 abrem legenda; 🔊 🔉 🎧 🗣 abrem audio. Outro emoji so separa.
      if (p[1] == 0x9F && ((p[2] == 0x92 && p[3] == 0xAC) || (p[2] == 0x92 && p[3] == 0xAD) ||
                           (p[2] == 0x93 && p[3] == 0x9D))) { tk->tipo = TK_EMOJI_LEG; p += 4; break; }
      if (p[1] == 0x9F && ((p[2] == 0x94 && (p[3] == 0x8A || p[3] == 0x89)) ||
                           (p[2] == 0x8E && p[3] == 0xA7) || (p[2] == 0x97 && p[3] == 0xA3))) {
        tk->tipo = TK_EMOJI_AUDIO; p += 4; break;
      }
      p += 4; continue;
    }
    // Letra: ASCII alfanumerico ou latino acentuado (C3 xx), rebaixado.
    if (isalnum(*p) || (p[0] == 0xC3 && p[1] >= 0x80 && p[1] <= 0xBE)) {
      size_t k = 0;
      int maiusc = 1, letras = 0;
      while (*p && (isalnum(*p) || (p[0] == 0xC3 && p[1] >= 0x80 && p[1] <= 0xBE))) {
        char c;
        if (*p == 0xC3) {
          unsigned d = p[1], idx = d >= 0xA0 ? d - 0xA0 : d - 0x80;
          c = idx < 30 ? BASE[idx] : '?';
          if (d >= 0xA0) maiusc = 0;
          p += 2; letras++;
        } else {
          if (islower(*p)) maiusc = 0;
          if (isalpha(*p)) letras++;
          c = (char)tolower(*p);
          p++;
        }
        if (k + 1 < sizeof tk->norm) tk->norm[k++] = c;
      }
      tk->norm[k] = 0;
      tk->tipo = TK_PALAVRA;
      tk->caixaAlta = maiusc && letras >= 3;
      { const unsigned char *q = p;
        while (*q == ' ' || *q == '\t') q++;
        tk->doisPontos = *q == ':'; }
      break;
    }
    p++;
  }
  *pp = (const char *)p;
}

// Ano (19xx/20xx), resolucao (720p, 4k) ou SxxEyy: o que separa o titulo do
// resto num nome de release.
static int idiomaMarco(const char *w) {
  size_t n = strlen(w), d = 0;
  while (d < n && isdigit((unsigned char)w[d])) d++;
  if (n == 4 && d == 4 && (!strncmp(w, "19", 2) || !strncmp(w, "20", 2))) return 1;
  if (d >= 3 && d == n - 1 && w[n - 1] == 'p') return 1;
  if (!strcmp(w, "4k") || !strcmp(w, "8k")) return 1;
  if (w[0] == 's' && isdigit((unsigned char)w[1])) {
    const char *e = w + 1;
    while (isdigit((unsigned char)*e)) e++;
    if (*e == 'e' && isdigit((unsigned char)e[1])) return 1;
  }
  return 0;
}

// O idioma que o token diz, ou NULL. Bandeira e sempre idioma; palavra so
// depois do marco do titulo.
static const char *idiomaDoToken(const IdiomaTk *tk, int depoisDoTitulo, const char *anterior) {
  const char *c;
  if (tk->tipo == TK_BANDEIRA) return idiomaTabela(IDIOMA_PAISES, IDIOMA_N(IDIOMA_PAISES), tk->norm);
  if (tk->tipo != TK_PALAVRA || !depoisDoTitulo) return NULL;
  // "x265", "1080p", "5.1": numero nunca e idioma, e pula as tabelas.
  for (c = tk->norm; *c; c++) if (isdigit((unsigned char)*c)) return NULL;
  if (!strcmp(tk->norm, "br") && !strcmp(anterior, "pt")) return "ptbr";   // "PT-BR"
  if ((c = idiomaTabela(IDIOMA_NOMES, IDIOMA_N(IDIOMA_NOMES), tk->norm))) return c;
  if (tk->caixaAlta) {
    char up[24];
    size_t k;
    for (k = 0; tk->norm[k] && k + 1 < sizeof up; k++) up[k] = (char)toupper((unsigned char)tk->norm[k]);
    up[k] = 0;
    return idiomaTabela(IDIOMA_SIGLAS, IDIOMA_N(IDIOMA_SIGLAS), up);
  }
  return NULL;
}

static void idiomasDoCampo(Stream *s, const char *campo) {
  const char *p = campo;
  while (p && *p) {
    // UMA CLAUSULA: primeiro o marco do titulo, depois os idiomas.
    const char *ini = p, *q = p;
    IdiomaTk tk;
    int nPal = 0, marco = -1;
    for (idiomaToken(&q, &tk); tk.tipo != TK_FIM && tk.tipo != TK_QUEBRA; idiomaToken(&q, &tk))
      if (tk.tipo == TK_PALAVRA) { if (marco < 0 && idiomaMarco(tk.norm)) marco = nPal; nPal++; }
    const char *fimClausula = q;
    int legenda = 0, prevLeg = 0, prevPlano = 0, primeiro = 1, iPal = 0;
    // O idioma do token anterior espera o seguinte para saber o destino:
    // "ENG SUBS" e legenda, "The Italian Job" (sem marco) e titulo.
    const char *pend = NULL;
    int pendLeg = 0, pendTitulo = 0;
    char anterior[24] = "";
#define SOLTA() do { if (pend) idiomaPor(pendLeg ? s->idiomasLegenda : s->idiomasAudio, \
                                         pendLeg ? &s->nIdiomasLegenda : &s->nIdiomasAudio, pend); \
                     pend = NULL; } while (0)
    q = ini;
    for (idiomaToken(&q, &tk); tk.tipo != TK_FIM && tk.tipo != TK_QUEBRA; idiomaToken(&q, &tk)) {
      const char *cod;
      int marcaLeg = tk.tipo == TK_EMOJI_LEG ||
                     (tk.tipo == TK_PALAVRA && idiomaNaLista(MARCAS_LEGENDA, IDIOMA_N(MARCAS_LEGENDA), tk.norm));
      int marcaAud = tk.tipo == TK_EMOJI_AUDIO ||
                     (tk.tipo == TK_PALAVRA && idiomaNaLista(MARCAS_AUDIO, IDIOMA_N(MARCAS_AUDIO), tk.norm));
      int depois = marco < 0 || (tk.tipo == TK_PALAVRA && iPal > marco);
      if (tk.tipo == TK_PALAVRA) iPal++;
      // "VOSE"/"VOSTFR" (original com legenda em espanhol/frances), "ESub".
      if (tk.tipo == TK_PALAVRA && (!strcmp(tk.norm, "vose") || !strcmp(tk.norm, "esub") ||
                                    !strcmp(tk.norm, "esubs") || !strcmp(tk.norm, "vostfr"))) {
        idiomaPor(s->idiomasLegenda, &s->nIdiomasLegenda,
                  tk.norm[0] == 'v' ? (tk.norm[3] == 'e' ? "es" : "fr") : "en");
        SOLTA(); prevLeg = prevPlano = 0;
      } else if (marcaLeg) {
        if (pend) { pendLeg = 1; SOLTA(); }   // "ENG SUBS": o anterior era legenda
        if (primeiro || tk.doisPontos || tk.tipo == TK_EMOJI_LEG) legenda = 1;
        prevLeg = 1; prevPlano = 0;
      } else if (marcaAud && (primeiro || tk.doisPontos || tk.tipo == TK_EMOJI_AUDIO)) {
        SOLTA();
        legenda = prevLeg = prevPlano = 0;
      } else if ((cod = idiomaDoToken(&tk, depois, anterior)) ||
                 // Colado na marca, codigo em qualquer caixa: "Sub Esp", "subs ita".
                 (prevLeg && tk.tipo == TK_PALAVRA && strlen(tk.norm) == 3 &&
                  (cod = idiomaTabela(IDIOMA_CODIGOS, IDIOMA_N(IDIOMA_CODIGOS), tk.norm)))) {
        // "Spanish Latino" e um idioma so: o latino toma o lugar do espanhol.
        // "CAST.LAT" e "Castellano Latino" sao duas faixas e ficam as duas.
        if (pend && !strcmp(cod, "lat") && !strcmp(pend, "es") &&
            (!strcmp(anterior, "spanish") || !strcmp(anterior, "espanol") ||
             !strcmp(anterior, "espanhol") || !strcmp(anterior, "espagnol") ||
             !strcmp(anterior, "spagnolo"))) pend = NULL;
        SOLTA();
        if (prevLeg) idiomaPor(s->idiomasLegenda, &s->nIdiomasLegenda, cod);
        else {
          pend = cod; pendLeg = legenda;
          pendTitulo = tk.tipo == TK_PALAVRA && marco < 0 && prevPlano && !tk.caixaAlta;
        }
        prevLeg = prevPlano = 0;
      } else if (tk.tipo == TK_PALAVRA) {
        if (pend && pendTitulo) pend = NULL;   // palavra comum dos dois lados: titulo
        SOLTA();
        prevLeg = 0; prevPlano = 1;
      }
      if (tk.tipo == TK_PALAVRA) snprintf(anterior, sizeof anterior, "%s", tk.norm);
      if (tk.tipo != TK_BANDEIRA) primeiro = 0;
    }
    SOLTA();
#undef SOLTA
    if (fimClausula == ini) break;
    p = fimClausula;
  }
}

// `subtitles` do Stremio: [{"id","url","lang"}...]. O `lang` vem como codigo
// ("spa", "pt-BR", "es-419") ou, de alguns addons, como nome ("Spanish").
static void idiomasDasLegendas(Stream *s, const char *obj, const char *fim) {
  const char *arr = valorRaiz(obj, fim, "subtitles"), *e;
  if (!arr || *arr != '[') return;
  e = arr + 1;
  while (e < fim && isspace((unsigned char)*e)) e++;
  while (e && e < fim && *e == '{') {
    const char *ef = js_fim(e);
    char lang[32] = "", low[32];
    size_t k;
    if (!ef || ef > fim) break;
    js_texto_raiz_em(e, ef, "lang", lang, sizeof lang);
    for (k = 0; lang[k] && k + 1 < sizeof low; k++) low[k] = (char)tolower((unsigned char)lang[k]);
    low[k] = 0;
    const char *cod = idiomaTabela(IDIOMA_CODIGOS, IDIOMA_N(IDIOMA_CODIGOS), low);
    if (!cod) cod = idiomaTabela(IDIOMA_NOMES, IDIOMA_N(IDIOMA_NOMES), low);
    if (cod) idiomaPor(s->idiomasLegenda, &s->nIdiomasLegenda, cod);
    e = js_prox(ef);
  }
}

static void idiomasLer(Stream *s, const char *titulo, const char *obj, const char *fim) {
  idiomasDoCampo(s, s->rotulo);
  idiomasDoCampo(s, s->descricao);
  if (strcmp(titulo, s->descricao)) idiomasDoCampo(s, titulo);
  idiomasDoCampo(s, s->arquivo);
  idiomasDasLegendas(s, obj, fim);
}

int stream_extrair(const char *json, const char *provedor, Stream **saida) {
  const char *p, *fim;
  int n = 0, cap = 0;
  Stream *v = NULL;
  *saida = NULL;
  if (!json) return 0;
  p = js_array(json, json + strlen(json), "streams");
  while (p && *p == '{') {
    Stream s = {0};
    char titulo[2048] = "", texto[5000];
    fim = js_fim(p);
    if (!fim || fim <= p) break;
    js_texto(p, fim, "url", s.url, sizeof s.url);
    if (!s.url[0]) js_texto(p, fim, "externalUrl", s.url, sizeof s.url);
    s.fileIdx = -1;
    // Torrent puro: sem url, com infoHash (no topo ou dentro de clientResolve,
    // como o AIOStreams manda). A url nasce depois, no debrid.
    if (!s.url[0] || strncmp(s.url, "http", 4)) {
      const char *cr = strstr(p, "\"clientResolve\"");
      s.url[0] = 0;
      if (!js_texto(p, fim, "infoHash", s.infoHash, sizeof s.infoHash) && cr && cr < fim)
        js_texto(cr, fim, "infoHash", s.infoHash, sizeof s.infoHash);
      double indice = js_num(p, fim, "fileIdx", -1);
      if (isfinite(indice) && indice >= 0 && indice <= INT_MAX)
        s.fileIdx = (int)indice;
      if (s.infoHash[0]) lerFontesP2P(p, fim, s.fontes, sizeof s.fontes);
    }
    // Nao tocar URL cortada; sem url e sem hash nao ha o que tocar.
    if ((s.infoHash[0] || !strncmp(s.url, "http", 4)) &&
        strlen(s.url) < sizeof s.url - 1) {
      js_texto(p, fim, "name", s.rotulo, sizeof s.rotulo);
      // So o name da PROPRIA fonte, inteiro, conta para o FHD (#402): o
      // rotulo de 192 bytes pode cortar "...FHDx" em "...FHD", e o provedor
      // entra no lugar do name que falta mais abaixo.
      int fhd = 0;
      { char nome[1024];
        if (js_texto_linhas(p, fim, "name", nome, sizeof nome) && strlen(nome) < sizeof nome - 1)
          fhd = fhdNoNome(nome); }
      js_texto_linhas(p, fim, "description", s.descricao, sizeof s.descricao);
      js_texto_linhas(p, fim, "title", titulo, sizeof titulo);
      js_texto(p, fim, "filename", s.arquivo, sizeof s.arquivo);
      // behaviorHints.bingeGroup — ANCORADO NO OBJETO, e nao procurado solto.
      //
      // Duas coisas separadas, e as duas custam uma linha:
      //
      // 1. `bh < fim` E O QUE IMPORTA DE VERDADE. strstr varre ate o fim do
      //    documento, nao ate o fim DESTE stream: sem a guarda, uma fonte que
      //    nao manda behaviorHints herdaria o bingeGroup da fonte SEGUINTE do
      //    array — e herdaria calada, com o sintoma sendo o app lembrar da
      //    fonte errada no episodio seguinte. Mesma guarda que o
      //    "clientResolve" logo acima ja usa, e pelo mesmo motivo.
      // 2. js_texto_raiz_em le so as chaves de PROFUNDIDADE 1 de
      //    behaviorHints. behaviorHints tem objeto dentro dele
      //    (proxyHeaders.request/response, com nomes de cabecalho que o addon
      //    escolhe), e a regra da casa para chave aninhada de Stremio esta em
      //    js.h: procurar solto pega a primeira ocorrencia, que nem sempre e a
      //    que se quer. Nas respostas que deu para inspecionar aqui o js_texto
      //    solto daria o mesmo resultado; ancorar e seguro de graca.
      { const char *bh = strstr(p, "\"behaviorHints\"");
        if (bh && bh < fim) {
          js_texto_raiz_em(bh, fim, "bingeGroup", s.bingeGroup, sizeof s.bingeGroup);
          js_texto_raiz_em(bh, fim, "videoHash", s.videoHash, sizeof s.videoHash);
          lerProxyHeaders(bh, fim, s.cabecalhos, sizeof s.cabecalhos);
        } }
      if (!s.descricao[0]) snprintf(s.descricao, sizeof s.descricao, "%s", titulo);
      // Antes do provedor entrar no rotulo: o nome do addon nao diz idioma.
      idiomasLer(&s, titulo, p, fim);
      if (!s.rotulo[0]) snprintf(s.rotulo, sizeof s.rotulo, "%s", provedor);
      snprintf(s.provedor, sizeof s.provedor, "%s", provedor);
      snprintf(texto, sizeof texto, "%s %s %s %s", s.rotulo, s.descricao, titulo, s.arquivo);
      for (char *q = texto; *q; q++) if (*q == '\n') *q = ' ';
      s.altura = contem(texto, "2160") || token(texto, "4k") || token(texto, "uhd") ? 2160 :
                 contem(texto, "1440") ? 1440 : contem(texto, "1080") ? 1080 :
                 contem(texto, "720") ? 720 : contem(texto, "480") ? 480 : 0;
      s.dolbyVision = token(texto, "dv") || token(texto, "dovi") ||
                      contem(texto, "dolby vision") || contem(texto, "dolbyvision");
      s.dolbyAtmos = token(texto, "atmos");
      s.badges = badges_detectar(texto);
      // "FHD"/"Full HD" (#402): formatadores tipo AIOStreams poem a resolucao
      // so como sigla no name ("FHD | REMUX | SDR"). Vale so na FALTA de
      // resolucao escrita em qualquer campo, e so do name: descricao e
      // filename trazem URL ("https://fhd...") e grupo de release ("x265-FHD").
      // E nunca ao lado de outro selo de resolucao que o badges_detectar ja deu.
      if (!s.altura && fhd &&
          !(s.badges & (badges_bit("r-4k") | badges_bit("r-1080") | badges_bit("r-720")))) {
        s.altura = 1080;
        s.badges |= badges_bit("r-1080");
      }
      s.mp4 = token(texto, "mp4") || contem(s.url, ".mp4");
      s.foraCache = stream_texto_fora_de_cache(texto);
      // AIOStreams: clientResolve.isCached e explicito; ausente nao e false.
      if (s.infoHash[0] && !s.url[0]) {
        const char *cr = valorRaiz(p, fim, "clientResolve");
        const char *v = cr ? valorRaiz(cr, fim, "isCached") : NULL;
        if (v && fim - v >= 5 && !memcmp(v, "false", 5) &&
            (v + 5 == fim || v[5] == ',' || v[5] == '}' || isspace((unsigned char)v[5])))
          s.foraCache = 1;
      }
      if (s.infoHash[0]) s.temSemeadores = lerSemeadores(texto, &s.semeadores);
      s.tamanhoBytes = bytesExatos(p, fim);
      double bytes = js_num(p, fim, "videoSize", 0);
      if (s.tamanhoBytes) s.tamanhoMB = tamanhoMB((double)s.tamanhoBytes / 1048576.0);
      else if (bytes > 0) s.tamanhoMB = tamanhoMB(bytes / (1024.0 * 1024.0));
      else {
        const char *u = strstr(texto, " GB");
        double escala = 1024;
        if (!u) { u = strstr(texto, " MB"); escala = 1; }
        if (u) {
          const char *ini = u;
          while (ini > texto && (isdigit((unsigned char)ini[-1]) || ini[-1] == '.')) ini--;
          if (ini < u) s.tamanhoMB = tamanhoMB(atof(ini) * escala);
        }
      }
      if (n == cap) {
        int nova = cap ? cap * 2 : 32;
        Stream *tmp = realloc(v, (size_t)nova * sizeof *tmp);
        if (!tmp) { free(v); return -1; }
        v = tmp; cap = nova;
      }
      v[n++] = s;
    }
    p = js_prox(fim);
  }
  *saida = v;
  return n;
}
