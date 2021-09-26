#!/bin/bash

VGA_SYNC_BITS=0xc0

DATA_DIR="$1"
OUT_DIR="$2"

if [ -z "${DATA_DIR}" ]; then
    DATA_DIR=../../data
fi

if [ -z "${OUT_DIR}" ]; then
    OUT_DIR=../../game/data
fi

# === convert sprites
SPR_DIR="${DATA_DIR}/spr"
for file in ${SPR_DIR}/*.spr; do
    NAME=$(basename $file .spr)
    ./conv_spr -sync ${VGA_SYNC_BITS} -num-frames 64 -name ${NAME} -out ${OUT_DIR}/spr_${NAME}.h ${file}
done

# === convert font
FONT_DIR="${DATA_DIR}/font"
./conv_font -num-chars-x 16 -num-chars-y 6 -name font6x8 -out ${OUT_DIR}/font6x8.h ${FONT_DIR}/font6x8.bmp
