#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
INSTALL_PREFIX=${INSTALL_PREFIX:-"$ROOT_DIR/build"}
LIB_BUILD_DIR=${LIB_BUILD_DIR:-"$ROOT_DIR/build"}
SIM_BUILD_DIR=${SIM_BUILD_DIR:-"$ROOT_DIR/sim/build"}

cmake -S "$ROOT_DIR" -B "$LIB_BUILD_DIR" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"
cmake --build "$LIB_BUILD_DIR"
cmake --install "$LIB_BUILD_DIR"

export PKG_CONFIG_PATH="$INSTALL_PREFIX/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"

cmake -S "$ROOT_DIR/sim" -B "$SIM_BUILD_DIR" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"
cmake --build "$SIM_BUILD_DIR"
cmake --install "$SIM_BUILD_DIR"



