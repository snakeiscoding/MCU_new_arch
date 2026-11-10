#!/bin/bash

set -euo pipefail

if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <target> <mode>" >&2
    exit 1
fi

TARGET="$1"
MODE="$2"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"
CLONE_DIR="${BITBUCKET_CLONE_DIR:-$(pwd)}"
BUILD_NUMBER="${BITBUCKET_BUILD_NUMBER:-local}"
BUILD_DIR="build_${TARGET}_${MODE}"
# Staged per pipeline run; the date is applied only by deploy_FTP.sh
OUT_DIR="${CLONE_DIR}/ci_artifacts/${BUILD_NUMBER}/${TARGET}"

export PATH=/opt/toolchains/gcc-arm-none-eabi-10.3-2021.10/bin:$PATH

# Remove stale output first so a failed build never leaves old files behind
rm -rf "${OUT_DIR}"

make clean TARGET="${TARGET}" MODE="${MODE}"
make -j"${JOBS}" TARGET="${TARGET}" MODE="${MODE}"

mkdir -p ${OUT_DIR}

cp "${BUILD_DIR}"/*.hex "${OUT_DIR}/"
cp "${BUILD_DIR}"/*.bin "${OUT_DIR}/"
cp "${BUILD_DIR}"/*.elf "${OUT_DIR}/"
cp "${BUILD_DIR}"/*.map "${OUT_DIR}/"
