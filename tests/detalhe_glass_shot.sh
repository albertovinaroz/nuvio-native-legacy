#!/bin/bash
export TMPDIR="${TMPDIR:-/tmp}"
TMPDIR="${TMPDIR%/}"
# Capturas da pagina do titulo no Glass UI, para comparar com os mockups
# (design/glass-ilha "Detalhe" e o "detalhe-retomar" do player). Nao entra na
# suite (testa-tudo.sh pula *_shot.sh): precisa de janela GL e de olho.
#
#   bash tests/detalhe_glass_shot.sh <pasta-saida> [id...]
#   NUVIO_SHOT_VIDRO=1 para o material vidro (sem ele, solido).
#
# Os .o dos outros modulos ficam em cache (/tmp/nuvio-detglass-obj) e so
# recompilam quando o .c muda; detail.c entra pelo include do teste.
set -eu
cd "$(dirname "$0")/.."
OBJ=${OBJ:-${TMPDIR:-/tmp}/nuvio-detglass-obj}
mkdir -p "$OBJ" "$1"
NUVIO_DADOS=$(mktemp -d ${TMPDIR:-/tmp}/nuvio-detglass-dados.XXXXXX)
export NUVIO_DADOS
trap 'rm -rf "$NUVIO_DADOS"' EXIT
FL="-O1 -g -Isrc -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -Wall -Wextra -Wno-deprecated-declarations -Wno-macro-redefined"
objs=()
pids=()
for s in src/*.c src/dts/*.c; do
  case "$s" in src/main.c|src/detail.c) continue;; esac
  o="$OBJ/$(basename "$s" .c).o"
  objs+=("$o")
  if [ ! -f "$o" ] || [ "$s" -nt "$o" ] || [ -n "$(find src -name '*.h' -newer "$o" -print -quit)" ]; then
    cc $FL -c "$s" -o "$o" & pids+=($!)
    if [ ${#pids[@]} -ge 8 ]; then wait "${pids[0]}"; pids=("${pids[@]:1}"); fi
  fi
done
wait
cc $FL tests/detalhe_glass_shot.c "${objs[@]}" -o "$OBJ/shot" \
  -L/opt/homebrew/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lz -framework OpenGL
"$OBJ/shot" "$@"
