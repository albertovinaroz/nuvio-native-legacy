#!/bin/bash
export TMPDIR="${TMPDIR:-/tmp}"
TMPDIR="${TMPDIR%/}"
# Home nos tres layouts (Moderna, Padrao, Dinamica), em BMP e PNG, sem rede
# (artes de deploy/app/art). Nao entra na suite (testa-tudo.sh pula *_shot.sh):
# precisa de janela GL e de olho humano.
#
#   bash tests/homelayouts_shot.sh /tmp/nv-home-layouts-shots [012]
#
# Cada BMP vira PNG (sips) e o BMP e apagado — sao 8 MB por captura.
set -eu
cd "$(dirname "$0")/.."

saida="${1:-${TMPDIR:-/tmp}/nv-home-layouts-shots}"
mkdir -p "$saida"
NUVIO_DADOS=$(mktemp -d "${TMPDIR:-/tmp}/nuvio-homelayouts-shot.XXXXXX")
export NUVIO_DADOS
trap 'rm -rf "$NUVIO_DADOS"' EXIT

sources=()
for source in src/*.c src/dts/*.c; do
  case "$source" in src/main.c) continue;; esac
  sources+=("$source")
done
# Nunca executar um binario velho se o compilador falhar. O filtro anterior
# terminava em `|| true` e escondia inclusive erros de link.
if ! cc "${sources[@]}" tests/homelayouts_shot.c -Isrc -o "$NUVIO_DADOS/shot" \
  -O1 -g ${NV_CFLAGS:-} -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 \
  -L/opt/homebrew/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lz -framework OpenGL \
  -Wall -Wextra -Wno-deprecated-declarations -Wno-macro-redefined \
  >"$NUVIO_DADOS/build.log" 2>&1; then
  cat "$NUVIO_DADOS/build.log" >&2
  exit 1
fi
grep -E "homelayouts_shot|error|home\.c" "$NUVIO_DADOS/build.log" || true
"$NUVIO_DADOS/shot" "$saida/h" "${2:-012}"
[ -n "${NV_KEEP:-}" ] && exit 0   # deixa os BMP, para comparar byte a byte
for f in "$saida"/h-*.bmp; do
  [ -e "$f" ] || continue
  sips -s format png "$f" --out "${f%.bmp}.png" >/dev/null 2>&1 && rm -f "$f"
done
