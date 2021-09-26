#!/bin/bash

DATA_DIR="$1"
OUT_DIR="$2"

if [ -z "${DATA_DIR}" ]; then
    DATA_DIR=../../data
fi

if [ -z "${OUT_DIR}" ]; then
    OUT_DIR=../../game/data
fi

# === convert raw samples
SOUND_DIR="${DATA_DIR}/sound"
for file in ${SOUND_DIR}/*.raw; do
    NAME=$(basename $file .raw)
    ./conv_raw ${file} ${OUT_DIR}/snd_${NAME}.h
done
