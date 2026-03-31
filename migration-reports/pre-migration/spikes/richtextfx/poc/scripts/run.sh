#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
SRC_DIR="${ROOT_DIR}/src"
OUT_DIR="${ROOT_DIR}/out"
LIB_DIR="${ROOT_DIR}/lib"

if [[ ! -f "${OUT_DIR}/RichTextFxSyntaxSpikeApp.class" ]]; then
  "${SCRIPT_DIR}/build.sh"
fi

case "$(uname -s):$(uname -m)" in
  Darwin:arm64|Darwin:aarch64)
    JFX_PLATFORM_SUFFIX="mac-aarch64"
    ;;
  Darwin:*)
    JFX_PLATFORM_SUFFIX="mac"
    ;;
  Linux:aarch64|Linux:arm64)
    JFX_PLATFORM_SUFFIX="linux-aarch64"
    ;;
  Linux:*)
    JFX_PLATFORM_SUFFIX="linux"
    ;;
  MINGW*:arm64|MSYS*:arm64|CYGWIN*:arm64|Windows_NT:arm64)
    JFX_PLATFORM_SUFFIX="win-aarch64"
    ;;
  MINGW*:*|MSYS*:*|CYGWIN*:*|Windows_NT:*)
    JFX_PLATFORM_SUFFIX="win"
    ;;
  *)
    echo "Unsupported platform for JavaFX module path." >&2
    exit 1
    ;;
esac

RICH_CP="${LIB_DIR}/richtextfx-0.11.7.jar:${LIB_DIR}/reactfx-2.0-M5.jar:${LIB_DIR}/undofx-2.1.1.jar:${LIB_DIR}/flowless-0.7.4.jar:${LIB_DIR}/wellbehavedfx-0.3.3.jar"
JFX_MP="${LIB_DIR}/javafx-base-21.0.2.jar:${LIB_DIR}/javafx-graphics-21.0.2.jar:${LIB_DIR}/javafx-controls-21.0.2.jar:${LIB_DIR}/javafx-base-21.0.2-${JFX_PLATFORM_SUFFIX}.jar:${LIB_DIR}/javafx-graphics-21.0.2-${JFX_PLATFORM_SUFFIX}.jar:${LIB_DIR}/javafx-controls-21.0.2-${JFX_PLATFORM_SUFFIX}.jar"

java \
  --module-path "${JFX_MP}" \
  --add-modules javafx.controls \
  --class-path "${RICH_CP}:${OUT_DIR}:${SRC_DIR}" \
  RichTextFxSyntaxSpikeApp
