#!/bin/bash
# Captura da folha de fontes com as bandeiras de audio e de legenda.
# Nao entra na suite: precisa de janela GL e de olho humano para julgar.
#
#   bash tests/fontes_idiomas_shot.sh /tmp/nuvio-fontes-idiomas
set -eu
cd "$(dirname "$0")/.."
NUVIO_DADOS=$(mktemp -d /tmp/nuvio-fontes-idiomas-dados.XXXXXX)
export NUVIO_DADOS
trap 'rm -rf "$NUVIO_DADOS"' EXIT
sources=()
for source in src/*.c src/dts/*.c; do
  if [ "$source" != src/main.c ]; then sources+=("$source"); fi
done
cc "${sources[@]}" tests/fontes_idiomas_shot.c -Isrc -o /tmp/nuvio-fontes-idiomas-shot \
  -O1 -g -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 \
  -L/opt/homebrew/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lz -framework OpenGL \
  -Wno-deprecated-declarations -Wno-macro-redefined
/tmp/nuvio-fontes-idiomas-shot "$@"
