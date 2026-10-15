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

static const char *objetoRaiz(const char *p, const char *fim, const char *nome) {
  const char *v = valorRaiz(p, fim, nome), *f;
  if (!v || *v != '{' || !(f = js_fim(v)) || f <= v || f > fim || f[-1] != '}') return NULL;
  return v;
}

// Idiomas de faixas sao metadados positivos do addon.
// Valida cada valor antes de usa-lo: JSON cortado, objetos muito
// fundos e chaves em cabecalhos nunca viram disponibilidade de audio/legenda.
static const char *faixasEspacos(const char *p, const char *fim) {
  while (p < fim && isspace((unsigned char)*p)) p++;
  return p;
}
static unsigned faixasHex4(const char *p) {
  unsigned n = 0;
  for (int k = 0; k < 4; k++) {
    unsigned char c = (unsigned char)p[k];
    if (!isxdigit(c)) return UINT_MAX;
    n = n * 16 + (c <= '9' ? (unsigned)(c - '0') : (unsigned)(tolower(c) - 'a' + 10));
  }
  return n;
}
static const char *faixasFimTexto(const char *p, const char *fim) {
  if (p >= fim || *p++ != '"') return NULL;
  while (p < fim) {
    unsigned char c = (unsigned char)*p++;
    if (c == '"') return p;
    if (c < 0x20) return NULL;
    if (c != '\\') continue;
    if (p >= fim) return NULL;
    c = (unsigned char)*p++;
    if (strchr("\"\\/bfnrt", c)) continue;
    if (c != 'u' || fim - p < 4) return NULL;
    unsigned cp = faixasHex4(p);
    if (cp == UINT_MAX) return NULL;
    p += 4;
    if (cp >= 0xDC00 && cp <= 0xDFFF) return NULL;
    if (cp >= 0xD800 && cp <= 0xDBFF) {
      if (fim - p < 6 || p[0] != '\\' || p[1] != 'u') return NULL;
      unsigned lo = faixasHex4(p + 2);
      if (lo < 0xDC00 || lo > 0xDFFF) return NULL;
      p += 6;
    }
  }
  return NULL;
}
static const char *faixasFimValor(const char *p, const char *fim, unsigned prof) {
  if (p >= fim || prof > 16) return NULL;
  if (*p == '"') return faixasFimTexto(p, fim);
  if (*p == '{' || *p == '[') {
    int objeto = *p == '{';
    char fecha = objeto ? '}' : ']';
    p = faixasEspacos(p + 1, fim);
    if (p < fim && *p == fecha) return p + 1;
    for (;;) {
      if (objeto) {
        p = faixasFimTexto(p, fim);
        if (!p) return NULL;
        p = faixasEspacos(p, fim);
        if (p >= fim || *p != ':') return NULL;
        p = faixasEspacos(p + 1, fim);
      }
      p = faixasFimValor(p, fim, prof + 1);
      if (!p) return NULL;
      p = faixasEspacos(p, fim);
      if (p >= fim) return NULL;
      if (*p == fecha) return p + 1;
      if (*p != ',') return NULL;
      p = faixasEspacos(p + 1, fim);
    }
  }
  static const char *const literais[] = {"null", "true", "false"};
  for (unsigned k = 0; k < sizeof literais / sizeof *literais; k++) {
    size_t n = strlen(literais[k]);
    if ((size_t)(fim - p) >= n && !memcmp(p, literais[k], n)) return p + n;
  }
  // Numeros sao ignorados como idiomas, mas precisam de sintaxe JSON valida.
  if (*p == '-') p++;
  if (p >= fim || !isdigit((unsigned char)*p)) return NULL;
  if (*p == '0') p++;
  else while (p < fim && isdigit((unsigned char)*p)) p++;
  if (p < fim && *p == '.') {
    p++;
    if (p >= fim || !isdigit((unsigned char)*p)) return NULL;
    while (p < fim && isdigit((unsigned char)*p)) p++;
  }
  if (p < fim && (*p == 'e' || *p == 'E')) {
    p++;
    if (p < fim && (*p == '+' || *p == '-')) p++;
    if (p >= fim || !isdigit((unsigned char)*p)) return NULL;
    while (p < fim && isdigit((unsigned char)*p)) p++;
  }
  return p;
}
static int faixasUtf8(const unsigned char *p) {
  while (*p) {
    unsigned c = *p++, cp, n;
    if (c < 0x80) continue;
    if (c >= 0xC2 && c <= 0xDF) { cp = c & 31; n = 1; }
    else if (c >= 0xE0 && c <= 0xEF) { cp = c & 15; n = 2; }
    else if (c >= 0xF0 && c <= 0xF4) { cp = c & 7; n = 3; }
    else return 0;
    unsigned minimo = n == 1 ? 0x80 : n == 2 ? 0x800 : 0x10000;
    for (unsigned k = 0; k < n; k++) {
      if ((*p & 0xC0) != 0x80) return 0;
      cp = (cp << 6) | (*p++ & 63);
    }
    if (cp < minimo || cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF) ||
        (cp >= 0x80 && cp <= 0x9F) || cp == 0x2028 || cp == 0x2029) return 0;
  }
  return 1;
}
static int faixasTexto(const char *p, const char *fim, char *dst, unsigned tam) {
  const char *f = faixasFimTexto(p, fim);
  char json[1024];
  if (!f || !tam || (size_t)(f - p) >= sizeof json || (size_t)(f - p - 2) >= tam) return 0;
  // O decodificador geral converte alguns controles em espacos. Aqui nao
  // aceitar "en\\u0000..." nem valores com quebras que parecam cabecalhos.
  for (const char *q = p + 1; q < f - 1; q++) if (*q == '\\') {
    q++;
    if (strchr("bfnrt", *q)) return 0;
    if (*q == 'u') {
      unsigned cp = faixasHex4(q + 1);
      if (cp < 0x20 || cp == 0x7F) return 0;
      q += 4;
    }
  }
  memcpy(json, p, (size_t)(f - p)); json[f - p] = 0;
  return js_cadeia(json, dst, tam) && faixasUtf8((const unsigned char *)dst);
}
static void faixaIdiomaAdicionar(char *dst, unsigned tam, const char *ini, const char *fim) {
  char idioma[128];
  while (ini < fim && isspace((unsigned char)*ini)) ini++;
  while (fim > ini && isspace((unsigned char)fim[-1])) fim--;
  size_t n = (size_t)(fim - ini);
  if (!n || n >= sizeof idioma) return;
  int letra = 0;
  for (const char *p = ini; p < fim; p++) {
    unsigned char c = (unsigned char)*p;
    if (c >= 0x80 || isalpha(c)) letra = 1;
    else if (!isdigit(c) && c != ' ' && c != '-' && c != '_' && c != '(' && c != ')' && c != '\'' && c != '.') return;
  }
  if (!letra) return;
  memcpy(idioma, ini, n); idioma[n] = 0;
  static const char *const vazios[] = {
    "und", "unknown", "null", "none", "undefined", "unavailable", "unreported",
    "not reported", "not specified", "n.a.", "zxx", "mul", "multi", "multilingual",
    "multiple", "multiple languages", "auto", "original"
  };
  for (unsigned k = 0; k < sizeof vazios / sizeof *vazios; k++)
    if (!strcasecmp(idioma, vazios[k])) return;
  for (const char *p = dst; *p;) {
    const char *e = strchr(p, ',');
    if (!e) e = p + strlen(p);
    if ((size_t)(e - p) == n && !strncasecmp(p, idioma, n)) return;
    p = *e ? e + 1 : e;
  }
  size_t usado = strlen(dst);
  if (usado + n + (usado ? 1 : 0) >= tam) return;
  if (usado) dst[usado++] = ',';
  memcpy(dst + usado, idioma, n + 1);
}
static void faixasValorLer(const char *v, const char *fim, char *dst, unsigned tam, unsigned prof, int faixa) {
  if (!v || v >= fim || prof > 3) return;
  if (*v == '"') {
    char texto[1024];
    if (!faixasTexto(v, fim, texto, sizeof texto)) return;
    const char *p = texto;
    for (const char *q = p;; q++) if (!*q || *q == ',' || *q == '|' || *q == ';') {
      faixaIdiomaAdicionar(dst, tam, p, q);
      if (!*q) break;
      p = q + 1;
    }
  } else if (*v == '[') {
    const char *q = faixasEspacos(v + 1, fim);
    while (q < fim && *q != ']') {
      const char *f = faixasFimValor(q, fim, 0);
      if (!f) return;
      // Lista aninhada nao tem significado de faixa: nao a achatar.
      if (*q == '"' || *q == '{') faixasValorLer(q, f, dst, tam, prof + 1, faixa);
      q = faixasEspacos(f, fim);
      if (q < fim && *q == ',') q = faixasEspacos(q + 1, fim);
      else break;
    }
  } else if (*v == '{') {
    // language/lang pertencem a faixa; code/name sao aceitos apenas dentro
    // de um objeto declarado como idioma. O nome/codec de uma faixa nao e
    // seu idioma: {name:"Stereo",code:"AAC"} nao informa idioma nenhum.
    static const char *const chaves[] = {"language", "lang", "languageCode", "language_code", "code", "iso639_1", "iso639_2", "name"};
    unsigned n = faixa ? 4 : sizeof chaves / sizeof *chaves;
    for (unsigned k = 0; k < n; k++) {
      const char *p = valorRaiz(v, fim, chaves[k]);
      if (p && (*p == '"' || *p == '{')) {
        faixasValorLer(p, fim, dst, tam, prof + 1, 0);
        return;
      }
    }
  }
}
static void faixasCampo(const char *obj, const char *fim, const char *chave, char *dst, unsigned tam, int faixa) {
  const char *v = valorRaiz(obj, fim, chave), *f;
  if (!v || !(f = faixasFimValor(v, fim, 0))) return;
  const char *apos = faixasEspacos(f, fim);
  if (apos >= fim || (*apos != ',' && *apos != '}')) return;
  faixasValorLer(v, f, dst, tam, 0, faixa);
}

// Um campo de texto do addon tambem pode RELATAR idiomas. Exigir uma secao
// rotulada e um idioma inteiro conhecido evita confundir Audio: Dolby Atmos,
// nomes de filmes e tags de release com idiomas de faixas.
static int faixaIdiomaTextoConhecido(const char *p, const char *fim) {
  static const char *const nomes[] = {
    "English", "French", "German", "Italian", "Spanish", "Portuguese",
    "Brazilian Portuguese", "Portuguese (Brazil)", "Portuguese (Portugal)",
    "Portuguese (Brasil)", "Portuguese (BR)", "Portuguese (PT)", "ptbr",
    "Português", "Portugues", "Português (Brasil)", "Português (BR)",
    "Português (Portugal)", "Inglês", "Ingles", "Francês",
    "Frances", "Alemão", "Alemao", "Italiano", "Español", "Espanhol",
    "Castilian", "Spanish (Latino)", "Spanish (Latin America)", "Latin American Spanish",
    "Arabic", "Bengali", "Bosnian", "Bulgarian", "Burmese", "Cantonese",
    "Catalan", "Chinese", "Chinese (Simplified)", "Chinese (Traditional)",
    "Croatian", "Czech", "Danish", "Dutch", "Estonian", "Finnish", "Greek",
    "Gujarati", "Hebrew", "Hindi", "Hungarian", "Icelandic", "Indonesian",
    "Japanese", "Kannada", "Khmer", "Korean", "Latvian", "Lithuanian",
    "Macedonian", "Malay", "Malayalam", "Mandarin", "Marathi", "Nepali",
    "Norwegian", "Persian", "Polish", "Punjabi", "Romanian", "Russian",
    "Serbian", "Sinhala", "Slovak", "Slovenian", "Swedish", "Tamil",
    "Telugu", "Thai", "Turkish", "Ukrainian", "Urdu", "Vietnamese",
    "Basque", "Galician", "Welsh", "Filipino", "Tagalog", "Latino", "Japonês",
    "日本語", "中文", "한국어"
  };
  size_t n = (size_t)(fim - p);
  char alias[128];
  if (n >= sizeof alias) return 0;
  memcpy(alias, p, n); alias[n] = 0;
  // Alias de entrada do addon; comparar sem traduzir os dados recebidos.
  if (!strcasecmp(alias, "Português (PT)")) return 1;
  for (unsigned k = 0; k < sizeof nomes / sizeof *nomes; k++)
    if (strlen(nomes[k]) == n && !strncasecmp(p, nomes[k], n)) return 1;
  // ISO 639-1 e os codigos de tres letras usuais dos addons. Validar a familia
  // antes das variantes BCP-47; "AAC", "DTS" e uma extensao .mkv nao contam.
  static const char *const codigos[] = {
    "aa", "ab", "ae", "af", "ak", "am", "an", "ar", "as", "av", "ay", "az",
    "ba", "be", "bg", "bh", "bi", "bm", "bn", "bo", "br", "bs", "ca", "ce",
    "ch", "co", "cr", "cs", "cu", "cv", "cy", "da", "de", "dv", "dz", "ee",
    "el", "en", "eo", "es", "et", "eu", "fa", "ff", "fi", "fj", "fo", "fr",
    "fy", "ga", "gd", "gl", "gn", "gu", "gv", "ha", "he", "hi", "ho", "hr",
    "ht", "hu", "hy", "hz", "ia", "id", "ie", "ig", "ii", "ik", "io", "is",
    "it", "iu", "ja", "jv", "ka", "kg", "ki", "kj", "kk", "kl", "km", "kn",
    "ko", "kr", "ks", "ku", "kv", "kw", "ky", "la", "lb", "lg", "li", "ln",
    "lo", "lt", "lu", "lv", "mg", "mh", "mi", "mk", "ml", "mn", "mr", "ms",
    "mt", "my", "na", "nb", "nd", "ne", "ng", "nl", "nn", "no", "nr", "nv",
    "ny", "oc", "oj", "om", "or", "os", "pa", "pi", "pl", "ps", "pt", "qu",
    "rm", "rn", "ro", "ru", "rw", "sa", "sc", "sd", "se", "sg", "si", "sk",
    "sl", "sm", "sn", "so", "sq", "sr", "ss", "st", "su", "sv", "sw", "ta",
    "te", "tg", "th", "ti", "tk", "tl", "tn", "to", "tr", "ts", "tt", "tw",
    "ty", "ug", "uk", "ur", "uz", "ve", "vi", "vo", "wa", "wo", "xh", "yi",
    "yo", "za", "zh", "zu", "eng", "fra", "fre", "deu", "ger", "ita", "spa", "por",
    "pob", "ptb", "ara", "ben", "bos", "bul", "mya", "bur", "yue", "cat", "zho", "chi",
    "cmn", "hrv", "ces", "cze", "dan", "nld", "dut", "est", "fin", "ell", "gre", "guj",
    "heb", "hin", "hun", "isl", "ice", "ind", "jpn", "kan", "khm", "kor", "lav", "lit",
    "mkd", "mac", "msa", "may", "mal", "mar", "nep", "nor", "nob", "nno", "fas", "per",
    "pol", "pan", "ron", "rum", "rus", "srp", "sin", "slk", "slo", "slv", "swe", "tam",
    "tel", "tha", "tur", "ukr", "urd", "vie", "eus", "baq", "glg", "cym", "wel", "fil"
  };
  size_t base = 0;
  while (base < n && isalpha((unsigned char)p[base]) && (unsigned char)p[base] < 128) base++;
  if (base < 2 || base > 3 || (base < n && p[base] != '-' && p[base] != '_')) return 0;
  int conhecido = 0;
  for (unsigned k = 0; k < sizeof codigos / sizeof *codigos; k++)
    if (strlen(codigos[k]) == base && !strncasecmp(p, codigos[k], base)) { conhecido = 1; break; }
  if (!conhecido) return 0;
  for (size_t k = base; k < n;) {
    if (p[k] != '-' && p[k] != '_') return 0;
    size_t ini = ++k;
    while (k < n && (unsigned char)p[k] < 128 && isalnum((unsigned char)p[k])) k++;
    if (k == ini || k - ini > 8) return 0;
  }
  return 1;
}
static const char *faixasRotuloSubsTexto(const char *p, const char *fim, int *tipo) {
  // Torrentio/AIOStreams: so a parte DEPOIS de "Subs /" e legenda. Os
  // indicadores regionais anteriores nao revelam se eram audio ou Multi Subs.
  static const char *const subs[] = {"multi subs", "subs"};
  for (unsigned k = 0; k < sizeof subs / sizeof *subs; k++) {
    size_t n = strlen(subs[k]);
    if ((size_t)(fim - p) < n || strncasecmp(p, subs[k], n)) continue;
    const char *q = faixasEspacos(p + n, fim);
    if (q < fim && *q == '/') { *tipo = 2; return q + 1; }
  }
  return p;
}
static const char *faixasRotuloPortuguesTexto(const char *p, const char *fim, int *tipo) {
  char rotulo[64];
  const char *q = p;
  while (q < fim && *q != ':' && (size_t)(q - p) < sizeof rotulo) q++;
  if (q == fim || *q != ':') return p;
  const char *ate = q;
  while (ate > p && isspace((unsigned char)ate[-1])) ate--;
  size_t n = (size_t)(ate - p);
  if (n >= sizeof rotulo) return p;
  memcpy(rotulo, p, n); rotulo[n] = 0;
  // Estes sao rotulos recebidos, nao texto de interface a traduzir.
  if (!strcasecmp(rotulo, "áudio") || !strcasecmp(rotulo, "Áudio") ||
      !strcasecmp(rotulo, "idiomas de áudio")) {
    *tipo = 1;
    return q + 1;
  }
  return p;
}
static const char *faixasRotuloTexto(const char *p, const char *fim, int *tipo) {
  static const struct { const char *rotulo; int tipo; } rotulos[] = {
    {"audio languages", 1}, {"audio language", 1}, {"audio", 1},
    {"languages", 1}, {"language", 1},
    {"subtitle languages", 2}, {"subtitle language", 2}, {"subtitles", 2},
    {"subtitle", 2}, {"subs", 2}, {"legendas", 2}, {"legenda", 2}
  };
  for (unsigned k = 0; k < sizeof rotulos / sizeof *rotulos; k++) {
    size_t n = strlen(rotulos[k].rotulo);
    if ((size_t)(fim - p) < n || strncasecmp(p, rotulos[k].rotulo, n)) continue;
    const char *q = faixasEspacos(p + n, fim);
    if (q < fim && *q == ':') { *tipo = rotulos[k].tipo; return q + 1; }
  }
  const char *portugues = faixasRotuloPortuguesTexto(p, fim, tipo);
  return portugues != p ? portugues : faixasRotuloSubsTexto(p, fim, tipo);
}
static const char *faixaIdiomaEmoji(const char *p, const char *fim) {
  // Inversa APENAS dos valores univocos do formatador AIOStreams, dentro de
  // uma secao ja declarada. Nao e uma tabela de paises para inferir idiomas:
  // 🇮🇳 representa sete idiomas e deve continuar desconhecido.
  // https://github.com/Viren070/AIOStreams/blob/main/packages/core/src/utils/languages.ts
  static const struct { const char *emoji, *idioma; } idiomas[] = {
    {"🇬🇧", "English"}, {"🇯🇵", "Japanese"}, {"🇨🇳", "Chinese"},
    {"🇷🇺", "Russian"}, {"🇸🇦", "Arabic"}, {"🇵🇹", "Portuguese"},
    {"🇧🇷", "Portuguese (Brazil)"}, {"🇪🇸", "Spanish"}, {"🇫🇷", "French"},
    {"🇩🇪", "German"}, {"🇮🇹", "Italian"}, {"🇰🇷", "Korean"},
    {"🇧🇩", "Bengali"}, {"🇵🇰", "Punjabi"}, {"🇹🇭", "Thai"},
    {"🇻🇳", "Vietnamese"}, {"🇮🇩", "Indonesian"}, {"🇹🇷", "Turkish"},
    {"🇮🇱", "Hebrew"}, {"🇮🇷", "Persian"}, {"🇺🇦", "Ukrainian"},
    {"🇬🇷", "Greek"}, {"🇱🇹", "Lithuanian"}, {"🇱🇻", "Latvian"},
    {"🇪🇪", "Estonian"}, {"🇵🇱", "Polish"}, {"🇨🇿", "Czech"},
    {"🇸🇰", "Slovak"}, {"🇭🇺", "Hungarian"}, {"🇷🇴", "Romanian"},
    {"🇧🇬", "Bulgarian"}, {"🇷🇸", "Serbian"}, {"🇭🇷", "Croatian"},
    {"🇸🇮", "Slovenian"}, {"🇳🇱", "Dutch"}, {"🇩🇰", "Danish"},
    {"🇫🇮", "Finnish"}, {"🇸🇪", "Swedish"}, {"🇳🇴", "Norwegian"},
    {"🇲🇾", "Malay"}, {"💃🏻", "Latino"}
  };
  size_t n = (size_t)(fim - p);
  for (unsigned k = 0; k < sizeof idiomas / sizeof *idiomas; k++)
    if (strlen(idiomas[k].emoji) == n && !memcmp(p, idiomas[k].emoji, n)) return idiomas[k].idioma;
  return NULL;
}
static size_t faixasSeparadorTexto(const char *p, const char *fim) {
  if (p >= fim) return 0;
  if (*p == ',' || *p == '|' || *p == ';' || *p == '/') return 1;
  return (size_t)(fim - p) >= strlen("•") && !memcmp(p, "•", strlen("•")) ? strlen("•") : 0;
}
static size_t faixasMarcadorTexto(const char *p, const char *fim, int *tipo) {
  static const struct { const char *emoji; int tipo; } marcadores[] = {
    {"🌎", 3}, {"🌐", 1}, {"🗣", 1}, {"⚑", 1}, {"📝", 2}
  };
  for (unsigned k = 0; k < sizeof marcadores / sizeof *marcadores; k++) {
    size_t n = strlen(marcadores[k].emoji);
    if ((size_t)(fim - p) < n || memcmp(p, marcadores[k].emoji, n)) continue;
    // 🌎 tambem e o valor "Multi" do AIO. Sua secao so aceita nomes, como
    // GDrive; nao transformar uma lista generica "🌎 / 🇬🇧" em audio.
    *tipo = marcadores[k].tipo;
    if ((size_t)(fim - p - n) >= strlen("️") && !memcmp(p + n, "️", strlen("️"))) n += strlen("️");
    return n;
  }
  if (p >= fim || (unsigned char)*p < 0x80) return 0;
  // Qualquer outro icone tecnico encerra a secao (🎧/🔊 sao codec/canais).
  // Indicadores regionais e o valor Latino permanecem valores, nao marcadores.
  if ((size_t)(fim - p) >= strlen("💃🏻") && !memcmp(p, "💃🏻", strlen("💃🏻"))) return 0;
  const unsigned char *q = (const unsigned char *)p;
  unsigned cp; size_t n;
  if (*q >= 0xC2 && *q <= 0xDF) { cp = *q & 31; n = 2; }
  else if (*q >= 0xE0 && *q <= 0xEF) { cp = *q & 15; n = 3; }
  else if (*q >= 0xF0 && *q <= 0xF4) { cp = *q & 7; n = 4; }
  else return 0;
  if ((size_t)(fim - p) < n) return 0;
  for (size_t k = 1; k < n; k++) cp = (cp << 6) | (q[k] & 63);
  if (!((cp >= 0x1F000 && cp <= 0x1FAFF && !(cp >= 0x1F1E6 && cp <= 0x1F1FF) &&
         !(cp >= 0x1F3FB && cp <= 0x1F3FF)) ||
        (cp >= 0x2600 && cp <= 0x27BF))) return 0;
  *tipo = 0;
  if ((size_t)(fim - p - n) >= strlen("️") && !memcmp(p + n, "️", strlen("️"))) n += strlen("️");
  return n;
}
static int faixasIndicadoresTexto(const char *p, const char *fim) {
  int indicador = 0;
  while ((p = faixasEspacos(p, fim)) < fim) {
    // O prefixo inline "Subs /" do Torrentio/AIOStreams pode conter apenas
    // indicadores regionais; "No Subs / English" nao declara uma faixa.
    if (fim - p < 4 || memcmp(p, "\xF0\x9F\x87", 3) ||
        (unsigned char)p[3] < 0xA6 || (unsigned char)p[3] > 0xBF) return 0;
    indicador = 1;
    p += 4;
  }
  return indicador;
}
static void faixasLinhaTexto(const char *p, const char *fim, char *audio, unsigned na,
                             char *subs, unsigned ns) {
  int tipo = 0;
  while (p < fim) {
    p = faixasEspacos(p, fim);
    size_t n = faixasSeparadorTexto(p, fim);
    if (n) { p += n; continue; }
    n = faixasMarcadorTexto(p, fim, &tipo);
    if (n) {
      p = faixasEspacos(p + n, fim);
      if (p < fim && *p == ':') p++;
      continue;
    }
    const char *rotulado = faixasRotuloTexto(p, fim, &tipo);
    if (rotulado != p) { p = rotulado; continue; }
    const char *q = p;
    for (; q < fim; q++) {
      int proximo;
      // Uma URL e um valor inteiro: suas barras nao separam idiomas nem
      // permitem reiniciar um rotulo no caminho, como /subtitles:fr.
      if (fim - q >= 3 && !memcmp(q, "://", 3)) {
        tipo = 0;
        q += 3;
        while (q < fim && !isspace((unsigned char)*q) && *q != '|' && *q != ',' && *q != ';') q++;
        break;
      }
      if (faixasSeparadorTexto(q, fim) || faixasMarcadorTexto(q, fim, &proximo)) break;
      // Subs / pode vir imediatamente apos os indicadores sem separador.
      if ((q == p || isspace((unsigned char)q[-1])) && faixasRotuloSubsTexto(q, fim, &proximo) != q) {
        const char *ate = q;
        while (ate > p && isspace((unsigned char)ate[-1])) ate--;
        if (q == p || faixasIndicadoresTexto(p, ate)) break;
      }
    }
    const char *ini = faixasEspacos(p, q), *ate = q;
    while (ate > ini && isspace((unsigned char)ate[-1])) ate--;
    // Outro campo encerra a secao; nao emprestar os valores seguintes.
    if (memchr(ini, ':', (size_t)(ate - ini))) tipo = 0;
    if (tipo) {
      char *dst = tipo == 2 ? subs : audio;
      unsigned tam = tipo == 2 ? ns : na;
      const char *idioma = tipo != 3 ? faixaIdiomaEmoji(ini, ate) : NULL;
      if (idioma) faixaIdiomaAdicionar(dst, tam, idioma, idioma + strlen(idioma));
      else if (faixaIdiomaTextoConhecido(ini, ate)) faixaIdiomaAdicionar(dst, tam, ini, ate);
    }
    p = q;
  }
}
static void faixasCampoTexto(const char *obj, const char *fim, const char *campo, Stream *s) {
  if (s->audioIdiomas[0] && s->legendaIdiomas[0]) return;
  const char *v = valorRaiz(obj, fim, campo), *f;
  char texto[2048], json[sizeof texto + 16];
  char audio[sizeof s->audioIdiomas] = "", subs[sizeof s->legendaIdiomas] = "";
  if (!v || !(f = faixasFimTexto(v, fim)) || (size_t)(f - v - 2) >= sizeof texto) return;
  const char *apos = faixasEspacos(f, fim);
  if (apos >= fim || (*apos != ',' && *apos != '}')) return;
  // A descricao permite quebras, mas nunca NUL, controles ocultos ou um valor
  // cortado cujo prefixo tenha virado um idioma aparentemente completo.
  for (const char *q = v + 1; q < f - 1; q++) if (*q == '\\') {
    q++;
    if (*q == 'b' || *q == 'f') return;
    if (*q == 'u') {
      unsigned cp = faixasHex4(q + 1);
      if ((cp < 0x20 && cp != '\n' && cp != '\r' && cp != '\t') || cp == 0x7F) return;
      q += 4;
    }
  }
  // Decodificar somente o valor validado e ancorado, preservando as quebras.
  // Um objeto temporario pequeno usa o decodificador JSON existente sem
  // permitir que uma chave igual em headers/outro objeto substitua a fonte.
  size_t n = (size_t)(f - v);
  memcpy(json, "{\"v\":", 5);
  memcpy(json + 5, v, n);
  json[n + 5] = '}'; json[n + 6] = 0;
  // O decodificador geral descarta \\r. Aqui uma quebra CR tambem delimita
  // metadados; normalizar so esse escape, sem alterar um \\\\r literal.
  for (size_t k = 6; k < n + 4; k++) if (json[k] == '\\') {
    if (json[++k] == 'r') json[k] = 'n';
  }
  if (!js_texto_linhas(json, json + n + 6, "v", texto, sizeof texto) ||
      !faixasUtf8((const unsigned char *)texto)) return;
  for (const char *p = texto; *p;) {
    const char *q = p;
    while (*q && *q != '\n' && *q != '\r') q++;
    faixasLinhaTexto(p, q, audio, sizeof audio, subs, sizeof subs);
    p = *q ? q + 1 : q;
  }
  if (!s->audioIdiomas[0]) memcpy(s->audioIdiomas, audio, sizeof audio);
  if (!s->legendaIdiomas[0]) memcpy(s->legendaIdiomas, subs, sizeof subs);
}
static void metadadosFaixas(const char *obj, const char *fim, Stream *s) {
  const char *cr = objetoRaiz(obj, fim, "clientResolve");
  const char *st = cr ? objetoRaiz(cr, js_fim(cr), "stream") : NULL;
  const char *raw = st ? objetoRaiz(st, js_fim(st), "raw") : NULL;
  const char *parsed = raw ? objetoRaiz(raw, js_fim(raw), "parsed") : NULL;
  if (!parsed && st) parsed = objetoRaiz(st, js_fim(st), "parsed");
  if (!parsed && cr) parsed = objetoRaiz(cr, js_fim(cr), "parsed");
  if (!raw) raw = objetoRaiz(obj, fim, "raw");
  if (!parsed && raw) parsed = objetoRaiz(raw, js_fim(raw), "parsed");
  const char *parsedRaiz = objetoRaiz(obj, fim, "parsed");
  const char *data = objetoRaiz(obj, fim, "streamData");
  const char *parsedFile = data ? objetoRaiz(data, js_fim(data), "parsedFile") : NULL;
  const char *objetos[] = {parsedFile, parsed, parsedRaiz, raw, st, cr, obj};
  static const char *const audio[] = {"audioLanguages", "audio_languages", "audioTracks", "audio_tracks"};
  static const char *const subs[] = {"subtitleLanguages", "subtitle_languages", "subtitleTracks", "subtitle_tracks"};
  for (unsigned k = 0; k < sizeof objetos / sizeof *objetos; k++) if (objetos[k]) {
    const char *f = js_fim(objetos[k]);
    for (unsigned j = 0; j < sizeof audio / sizeof *audio; j++)
      faixasCampo(objetos[k], f, audio[j], s->audioIdiomas, sizeof s->audioIdiomas, j >= 2);
    for (unsigned j = 0; j < sizeof subs / sizeof *subs; j++)
      faixasCampo(objetos[k], f, subs[j], s->legendaIdiomas, sizeof s->legendaIdiomas, j >= 2);
    const char *tracks = objetoRaiz(objetos[k], f, "tracks");
    if (tracks) {
      const char *tf = js_fim(tracks);
      faixasCampo(tracks, tf, "audio", s->audioIdiomas, sizeof s->audioIdiomas, 1);
      faixasCampo(tracks, tf, "subtitles", s->legendaIdiomas, sizeof s->legendaIdiomas, 1);
      faixasCampo(tracks, tf, "subtitle", s->legendaIdiomas, sizeof s->legendaIdiomas, 1);
    }
  }
  // AIOStreams separa explicitamente languages (audio) de subtitles no
  // ParsedFile. So estes caminhos documentados autorizam o nome generico.
  // https://github.com/Viren070/AIOStreams/blob/main/packages/core/src/db/schemas.ts
  if (parsedFile) {
    faixasCampo(parsedFile, js_fim(parsedFile), "languages", s->audioIdiomas, sizeof s->audioIdiomas, 0);
    faixasCampo(parsedFile, js_fim(parsedFile), "subtitles", s->legendaIdiomas, sizeof s->legendaIdiomas, 0);
  }
  if (parsed) {
    faixasCampo(parsed, js_fim(parsed), "languages", s->audioIdiomas, sizeof s->audioIdiomas, 0);
    faixasCampo(parsed, js_fim(parsed), "subtitles", s->legendaIdiomas, sizeof s->legendaIdiomas, 0);
  }
  // Stremio stream.subtitles sao legendas anexadas a ESTA fonte. Nao buscar
  // no objeto de resposta/titulo, nem afirmar que estas faixas sao embutidas.
  faixasCampo(obj, fim, "subtitles", s->legendaIdiomas, sizeof s->legendaIdiomas, 1);
  // Valores estruturados continuam prioritarios, por tipo. Na falta deles,
  // ler somente secoes declaradas no texto DESTA fonte, nunca seu name,
  // provedor, filename, filtro de idiomas ou faixas descobertas pelo player.
  faixasCampoTexto(obj, fim, "description", s);
  faixasCampoTexto(obj, fim, "title", s);
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
  // Os do #487 (upstream) que faltavam aqui.
  { "malayalam", "ml" }, { "malay", "ms" }, { "bengali", "bn" }, { "bangla", "bn" },
  { "persian", "fa" }, { "farsi", "fa" }, { "catalan", "ca" }, { "catala", "ca" },
  { "estonian", "et" }, { "latvian", "lv" }, { "lithuanian", "lt" }, { "icelandic", "is" },
  { "filipino", "tl" }, { "tagalog", "tl" }, { "gujarati", "gu" }, { "burmese", "my" },
  { "bosnian", "bs" }, { "punjabi", "pa" }, { "urdu", "ur" }, { "kannada", "kn" },
  { "marathi", "mr" }, { "macedonian", "mk" }, { "albanian", "sq" }, { "georgian", "ka" },
  { "swahili", "sw" }, { "armenian", "hy" }, { "kazakh", "kk" }, { "nepali", "ne" },
  { "sinhala", "si" }, { "khmer", "km" }, { "mongolian", "mn" }, { "cantonese", "zh" },
};
// ABREVIATURAS DE RELEASE EM ESPANHOL, capitalizadas ("Lat-Cast-Ing", "Subt
// Forz-Esp-Ing" do Peerflix). Valem em caixa alta ou so com a inicial maiuscula;
// em minuscula sao palavras. Fora "Cat", "Jap" e "Por": titulo/palavra comum.
static const IdiomaTermo IDIOMA_ABREV[] = {
  { "cast", "es" }, { "esp", "es" }, { "lat", "lat" }, { "ing", "en" },
  { "ita", "it" }, { "fra", "fr" }, { "ale", "de" },
};
// Entre a marca de legenda e os idiomas dela ("Subt Forz-Esp-Ing", "Subs
// Forced English"): nao e idioma nem quebra a sequencia.
static const char *const IDIOMA_TRANSPARENTES[] = {
  "forz", "forzado", "forzados", "forzada", "forzadas", "forced", "forcados", "sdh", "full",
  "comp", "completo", "completos", "completa", "completas",
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
  { "ml", "ml" }, { "mal", "ml" }, { "ms", "ms" }, { "may", "ms" }, { "msa", "ms" },
  { "bn", "bn" }, { "ben", "bn" }, { "fa", "fa" }, { "per", "fa" }, { "fas", "fa" },
  { "ca", "ca" }, { "cat", "ca" }, { "et", "et" }, { "est", "et" }, { "lv", "lv" },
  { "lav", "lv" }, { "lt", "lt" }, { "lit", "lt" }, { "is", "is" }, { "ice", "is" },
  { "isl", "is" }, { "tl", "tl" }, { "tgl", "tl" }, { "fil", "tl" }, { "gu", "gu" },
  { "guj", "gu" }, { "my", "my" }, { "bur", "my" }, { "mya", "my" }, { "bs", "bs" },
  { "bos", "bs" }, { "pa", "pa" }, { "pan", "pa" }, { "ur", "ur" }, { "urd", "ur" },
  { "kn", "kn" }, { "kan", "kn" }, { "mr", "mr" }, { "mar", "mr" }, { "mk", "mk" },
  { "mkd", "mk" }, { "sq", "sq" }, { "sqi", "sq" }, { "ka", "ka" }, { "kat", "ka" },
  { "sw", "sw" }, { "swa", "sw" }, { "hy", "hy" }, { "hye", "hy" }, { "kk", "kk" },
  { "kaz", "kk" }, { "ne", "ne" }, { "nep", "ne" }, { "si", "si" }, { "sin", "si" },
  { "km", "km" }, { "khm", "km" }, { "mn", "mn" }, { "mon", "mn" }, { "yue", "zh" },
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
  "sub", "subs", "subt", "subbed", "subtitle", "subtitles", "subtitulo", "subtitulos",
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
  int capital;     // comeca com maiuscula ASCII e tem 3+ letras ("Cast", "Ing")
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
      int maiusc = 1, letras = 0, inicialMaiusc = isupper(*p) != 0;
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
      tk->capital = inicialMaiusc && letras >= 3;
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
  if (tk->tipo != TK_PALAVRA) return NULL;
  // "x265", "1080p", "5.1": numero nunca e idioma, e pula as tabelas.
  for (c = tk->norm; *c; c++) if (isdigit((unsigned char)*c)) return NULL;
  if (!strcmp(tk->norm, "br") && !strcmp(anterior, "pt")) return "ptbr";   // "PT-BR"
  // SIGLA E ABREVIATURA valem tambem antes do marco: o Peerflix escreve
  // "Animales Lat-Cast-Ing ... 1080p", com a resolucao so no fim. Titulo quase
  // nunca e sigla; "Cast:" com dois pontos e rotulo de elenco, nao idioma.
  if (tk->caixaAlta) {
    char up[24];
    size_t k;
    for (k = 0; tk->norm[k] && k + 1 < sizeof up; k++) up[k] = (char)toupper((unsigned char)tk->norm[k]);
    up[k] = 0;
    if ((c = idiomaTabela(IDIOMA_SIGLAS, IDIOMA_N(IDIOMA_SIGLAS), up))) return c;
  }
  if ((tk->caixaAlta || tk->capital) && !tk->doisPontos &&
      (c = idiomaTabela(IDIOMA_ABREV, IDIOMA_N(IDIOMA_ABREV), tk->norm))) return c;
  if (!depoisDoTitulo) return NULL;
  return idiomaTabela(IDIOMA_NOMES, IDIOMA_N(IDIOMA_NOMES), tk->norm);
}

static int idiomaTransparente(const IdiomaTk *tk) {
  return tk->tipo == TK_PALAVRA &&
         idiomaNaLista(IDIOMA_TRANSPARENTES, IDIOMA_N(IDIOMA_TRANSPARENTES), tk->norm);
}
// A MARCA DE LEGENDA APONTA PARA FRENTE OU PARA TRAS? "Ing Subt Forz-Esp-Ing":
// os idiomas depois de "Subt" sao a legenda e o "Ing" de antes e audio. "ENG
// SUBS" sem nada depois: o anterior era a legenda. Olha o proximo token da
// clausula, pulando os transparentes ("Forz", "Forced", "SDH").
static int idiomaAdiante(const char *q) {
  IdiomaTk t;
  for (;;) {
    idiomaToken(&q, &t);
    if (t.tipo == TK_FIM || t.tipo == TK_QUEBRA) return 0;
    if (idiomaTransparente(&t)) continue;
    if (t.tipo == TK_BANDEIRA || t.tipo == TK_PALAVRA)
      return idiomaDoToken(&t, 1, "") != NULL ||
             (t.tipo == TK_PALAVRA && strlen(t.norm) == 3 &&
              idiomaTabela(IDIOMA_CODIGOS, IDIOMA_N(IDIOMA_CODIGOS), t.norm) != NULL);
    return 0;
  }
}
// O proximo token (pulando os transparentes) e idioma ou marca de audio/legenda?
// E o que faz um NOME antes do marco do titulo valer: "Castellano-Ingles-Subt"
// e uma lista de idiomas; "The Italian Job" nao.
static int listaAdiante(const char *q) {
  IdiomaTk t;
  for (;;) {
    idiomaToken(&q, &t);
    if (t.tipo == TK_FIM || t.tipo == TK_QUEBRA) return 0;
    if (idiomaTransparente(&t)) continue;
    if (t.tipo == TK_EMOJI_LEG || t.tipo == TK_EMOJI_AUDIO || t.tipo == TK_BANDEIRA) return 1;
    if (t.tipo != TK_PALAVRA) return 0;
    return idiomaDoToken(&t, 1, "") != NULL ||
           idiomaNaLista(MARCAS_LEGENDA, IDIOMA_N(MARCAS_LEGENDA), t.norm) ||
           idiomaNaLista(MARCAS_AUDIO, IDIOMA_N(MARCAS_AUDIO), t.norm);
  }
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
    int legenda = 0, prevLeg = 0, prevPlano = 0, primeiro = 1, iPal = 0, prevIdioma = 0;
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
      if (idiomaTransparente(&tk)) continue;
      // "VOSE"/"VOSTFR" (original com legenda em espanhol/frances), "ESub".
      if (tk.tipo == TK_PALAVRA && (!strcmp(tk.norm, "vose") || !strcmp(tk.norm, "esub") ||
                                    !strcmp(tk.norm, "esubs") || !strcmp(tk.norm, "vostfr"))) {
        idiomaPor(s->idiomasLegenda, &s->nIdiomasLegenda,
                  tk.norm[0] == 'v' ? (tk.norm[3] == 'e' ? "es" : "fr") : "en");
        SOLTA(); prevLeg = prevPlano = 0;
      } else if (marcaLeg) {
        // "ENG SUBS": o anterior era a legenda; "Ing Subt Esp": era audio.
        if (pend) { if (!idiomaAdiante(q)) pendLeg = 1; SOLTA(); }
        if (primeiro || tk.doisPontos || tk.tipo == TK_EMOJI_LEG) legenda = 1;
        prevLeg = 1; prevPlano = 0; prevIdioma = 1;
      } else if (marcaAud && (primeiro || tk.doisPontos || tk.tipo == TK_EMOJI_AUDIO)) {
        SOLTA();
        legenda = prevLeg = prevPlano = 0; prevIdioma = 1;
      } else if ((cod = idiomaDoToken(&tk, depois, anterior)) ||
                 // Nome antes do marco, mas dentro de uma LISTA de idiomas.
                 (!depois && (prevIdioma || listaAdiante(q)) && (cod = idiomaDoToken(&tk, 1, anterior))) ||
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
        // Depois da marca a SEQUENCIA e legenda ("Subt Esp-Ing"): prevLeg segue
        // ligado ate uma palavra que nao seja idioma.
        if (prevLeg) idiomaPor(s->idiomasLegenda, &s->nIdiomasLegenda, cod);
        else {
          pend = cod; pendLeg = legenda;
          pendTitulo = tk.tipo == TK_PALAVRA && marco < 0 && prevPlano && !tk.caixaAlta;
        }
        prevPlano = 0; prevIdioma = 1;
      } else if (tk.tipo == TK_PALAVRA) {
        if (pend && pendTitulo) pend = NULL;   // palavra comum dos dois lados: titulo
        SOLTA();
        prevLeg = 0; prevPlano = 1; prevIdioma = 0;
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
    char lang[32] = "";
    if (!ef || ef > fim) break;
    js_texto_raiz_em(e, ef, "lang", lang, sizeof lang);
    const char *cod = stream_idioma_codigo(lang);
    if (cod && strcmp(cod, "multi")) idiomaPor(s->idiomasLegenda, &s->nIdiomasLegenda, cod);
    e = js_prox(ef);
  }
}

// Etiqueta de idioma de qualquer origem ("spa", "pt-BR", "es-419", "Spanish",
// ou o proprio codigo da fonte "lat"/"ptbr") -> codigo da fonte, ou NULL.
// "en-US"/"fr-CA" sem entrada propria caem na familia antes do tracinho.
const char *stream_idioma_codigo(const char *etiqueta) {
  char low[32];
  size_t k;
  const char *cod;
  if (!etiqueta || !etiqueta[0]) return NULL;
  for (k = 0; etiqueta[k] && k + 1 < sizeof low; k++) {
    char c = (char)tolower((unsigned char)etiqueta[k]);
    low[k] = c == '_' ? '-' : c;
  }
  low[k] = 0;
  if (!strcmp(low, "lat") || !strcmp(low, "ptbr") || !strcmp(low, "multi")) {
    static const char *const PROPRIOS[] = { "lat", "ptbr", "multi" };
    for (k = 0; k < 3; k++) if (!strcmp(low, PROPRIOS[k])) return PROPRIOS[k];
  }
  if ((cod = idiomaTabela(IDIOMA_CODIGOS, IDIOMA_N(IDIOMA_CODIGOS), low))) return cod;
  if ((cod = idiomaTabela(IDIOMA_NOMES, IDIOMA_N(IDIOMA_NOMES), low))) return cod;
  { char *h = strchr(low, '-');
    if (h && h > low) { *h = 0; return idiomaTabela(IDIOMA_CODIGOS, IDIOMA_N(IDIOMA_CODIGOS), low); } }
  return NULL;
}

// Um valor DECLARADO pelo #487 ("English", "Spanish (Latin America)", "pt-BR",
// "Chinese (Simplified)") -> codigo da fonte. Qualificador entre parenteses:
// o latino e o Brasil mudam o idioma; o resto ("Simplified") cai na familia.
static const char *codigoDeclarado(const char *raw) {
  char low[64], *par;
  size_t k;
  const char *c;
  if ((c = stream_idioma_codigo(raw))) return c;
  for (k = 0; raw[k] && k + 1 < sizeof low; k++) low[k] = (char)tolower((unsigned char)raw[k]);
  low[k] = 0;
  if ((strstr(low, "spanish") || strstr(low, "espa")) && (strstr(low, "latin") || strstr(low, "419")))
    return "lat";
  if (strstr(low, "portug") && (strstr(low, "bra") || strstr(low, "(br)"))) return "ptbr";
  if ((par = strstr(low, " (")) != NULL) { *par = 0; return stream_idioma_codigo(low); }
  return NULL;
}
// O QUE O #487 LEU (campos estruturados do addon: streamData.parsedFile do
// AIOStreams, audioLanguages, tracks; e secoes rotuladas "Audio: ...") vem
// PRIMEIRO na lista: e declaracao do addon, nao deducao do texto. O que o
// texto deduziu completa, sem repetir.
static void idiomasDosMetadados(Stream *s) {
  for (int leg = 0; leg < 2; leg++) {
    const char *p = leg ? s->legendaIdiomas : s->audioIdiomas;
    char (*lista)[6] = leg ? s->idiomasLegenda : s->idiomasAudio;
    unsigned char *n = leg ? &s->nIdiomasLegenda : &s->nIdiomasAudio;
    char novo[STREAM_IDIOMAS][6];
    unsigned char nn = 0;
    if (!*p) continue;
    while (*p) {
      const char *f = strchr(p, ','), *ate = f ? f : p + strlen(p);
      char item[64];
      while (p < ate && isspace((unsigned char)*p)) p++;
      while (ate > p && isspace((unsigned char)ate[-1])) ate--;
      snprintf(item, sizeof item, "%.*s", (int)(ate - p), p);
      const char *cod = item[0] ? codigoDeclarado(item) : NULL;
      if (cod && strcmp(cod, "multi")) idiomaPor(novo, &nn, cod);
      if (!f) break;
      p = f + 1;
    }
    for (unsigned k = 0; k < *n; k++) idiomaPor(novo, &nn, lista[k]);
    memcpy(lista, novo, sizeof novo);
    *n = nn;
  }
}
static void idiomasLer(Stream *s, const char *titulo, const char *obj, const char *fim) {
  idiomasDoCampo(s, s->rotulo);
  idiomasDoCampo(s, s->descricao);
  if (strcmp(titulo, s->descricao)) idiomasDoCampo(s, titulo);
  idiomasDoCampo(s, s->arquivo);
  idiomasDasLegendas(s, obj, fim);
  idiomasDosMetadados(s);
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
      metadadosFaixas(p, fim, &s);
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
