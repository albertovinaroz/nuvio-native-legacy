#!/bin/bash
# Idioma de audio da pessoa na folha de fontes: escolha automatica, filtro
# "Dublado" e rotulo DV na Samsung. Compila streams.c duas vezes: normal e
# -DNV_TPK. Ver tests/fonte_idioma_pref.c.
set -euo pipefail
cd "$(dirname "$0")/.."
dir=$(mktemp -d "${TMPDIR:-/tmp}/nuvio-idpref.XXXXXX")
trap 'rm -rf "$dir"' EXIT
sources=()
for source in src/*.c src/dts/*.c; do
  case "$source" in src/main.c|src/streams.c) continue;; esac
  sources+=("$source")
done
flags=(-O1 -g -Isrc -I/opt/homebrew/include -I/opt/homebrew/include/SDL2
  -Wno-deprecated-declarations -Wno-macro-redefined)
libs=(-L/opt/homebrew/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lz -framework OpenGL)
mkdir -p "$dir/obj"
for s in "${sources[@]}"; do
  o="$dir/obj/$(echo "$s" | tr / _).o"
  cc "${flags[@]}" -c "$s" -o "$o"
done
for tpk in 0 1; do
  extra=(); [ "$tpk" = 1 ] && extra=(-DNV_TPK)
  cc "${flags[@]}" ${extra[@]+"${extra[@]}"} tests/fonte_idioma_pref.c "$dir"/obj/*.o -o "$dir/t$tpk" "${libs[@]}"
  TMPDIR="$dir" "$dir/t$tpk"
done
