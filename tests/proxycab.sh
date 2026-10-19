#!/bin/bash
# Proxy de cabecalhos do VOD da Samsung (fork). Ver tests/proxycab.c.
set -eu
cd "$(dirname "$0")/.."
d=$(mktemp -d "${TMPDIR:-/tmp}/nuvio-proxycab.XXXXXX")
trap 'kill $srv 2>/dev/null || true; rm -rf "$d"' EXIT
sources=()
for source in src/*.c src/dts/*.c; do case "$source" in src/main.c) continue;; esac; sources+=("$source"); done
cc -O1 -g "${sources[@]}" tests/proxycab.c -Isrc -o "$d/t" -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 \
  -L/opt/homebrew/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lz -framework OpenGL \
  -Wno-deprecated-declarations -Wno-macro-redefined
porta=$((20000 + RANDOM % 20000))
"$d/t" "$porta" "$d/filme.bin" >/dev/null 2>&1 || true   # gera o arquivo
python3 -I tests/proxycab_srv.py "$porta" "$d/filme.bin" & srv=$!
for i in $(seq 1 50); do nc -z 127.0.0.1 "$porta" 2>/dev/null && break; sleep 0.1; done
NUVIO_DADOS="$d" "$d/t" "$porta" "$d/filme.bin"
