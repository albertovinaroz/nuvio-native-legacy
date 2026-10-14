#!/bin/bash
# BANDEIRAS DOS IDIOMAS DA FOLHA DE FONTES (src/streams.c, bandeiraDe).
#
# A TV nao tem emoji colorido (ver EMOJI_ICONE em streams.c): o 🇪🇸 que o addon
# manda sai como "ES" em texto. A folha desenha a bandeira de cada idioma de
# audio e de legenda com estas imagens, coloridas (GFX_TEXTO preserva o RGB).
#
# Fonte: flagcdn.com, PLANAS com 80 px de largura e a proporcao de cada pais
# (a do Reino Unido e 2:1, a da Espanha 3:2). As de tamanho fixo de la
# (64x48...) sao o desenho "tremulando", que na fileira lia como enfeite. A
# folha desenha todas na mesma ALTURA e a largura sai de tex_aspecto. As
# bandeiras nacionais sao de dominio publico.
#
# Saida: deploy/app/art/bandeiras/<pais>.png (pais ISO 3166 em minusculas).
# Idempotente. Novo idioma: o pais entra aqui E em BANDEIRA_DE em streams.c.
set -euo pipefail
cd "$(dirname "$0")/.."
ART=deploy/app/art/bandeiras
PAISES="gb es mx pt br fr de it jp kr cn ru ua pl tr nl se no dk fi gr cz hu ro
        il th vn id in sa bg hr rs sk si"
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$ART"
for p in $PAISES; do
  curl -fsSL "https://flagcdn.com/w80/$p.png" -o "$TMP/$p.png"
  # So PNG de verdade entra no pacote: uma pagina de erro com 200 nao passa.
  file -b "$TMP/$p.png" | grep -q '^PNG image data, 80 x ' || { echo "bandeiras.sh: $p nao e PNG de 80 px" >&2; exit 1; }
  cp "$TMP/$p.png" "$ART/$p.png"
done
echo "bandeiras.sh: $(ls "$ART" | wc -l | tr -d ' ') bandeiras em $ART"
