#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
LIB_DIR="${ROOT_DIR}/lib"
mkdir -p "${LIB_DIR}"

detect_javafx_classifier() {
  local os arch
  os="$(uname -s)"
  arch="$(uname -m)"

  case "${os}" in
    Darwin)
      if [[ "${arch}" == "arm64" || "${arch}" == "aarch64" ]]; then
        echo "mac-aarch64"
      else
        echo "mac"
      fi
      ;;
    Linux)
      if [[ "${arch}" == "aarch64" || "${arch}" == "arm64" ]]; then
        echo "linux-aarch64"
      else
        echo "linux"
      fi
      ;;
    MINGW*|MSYS*|CYGWIN*|Windows_NT)
      if [[ "${arch}" == "aarch64" || "${arch}" == "arm64" ]]; then
        echo "win-aarch64"
      else
        echo "win"
      fi
      ;;
    *)
      echo "Unsupported OS for JavaFX classifier: ${os}" >&2
      exit 1
      ;;
  esac
}

download_jar() {
  local group="$1"
  local artifact="$2"
  local version="$3"
  local classifier="${4:-}"
  local group_path file url out

  group_path="$(echo "${group}" | tr '.' '/')"
  if [[ -n "${classifier}" ]]; then
    file="${artifact}-${version}-${classifier}.jar"
  else
    file="${artifact}-${version}.jar"
  fi

  out="${LIB_DIR}/${file}"
  if [[ -f "${out}" ]]; then
    return
  fi

  url="https://repo1.maven.org/maven2/${group_path}/${artifact}/${version}/${file}"
  echo "Downloading ${file}"
  curl -fsSL "${url}" -o "${out}"
}

JAVAFX_VERSION="21.0.2"
JAVAFX_CLASSIFIER="$(detect_javafx_classifier)"

download_jar "org.fxmisc.richtext" "richtextfx" "0.11.7"
download_jar "org.reactfx" "reactfx" "2.0-M5"
download_jar "org.fxmisc.undo" "undofx" "2.1.1"
download_jar "org.fxmisc.flowless" "flowless" "0.7.4"
download_jar "org.fxmisc.wellbehaved" "wellbehavedfx" "0.3.3"

download_jar "org.openjfx" "javafx-base" "${JAVAFX_VERSION}"
download_jar "org.openjfx" "javafx-graphics" "${JAVAFX_VERSION}"
download_jar "org.openjfx" "javafx-controls" "${JAVAFX_VERSION}"

download_jar "org.openjfx" "javafx-base" "${JAVAFX_VERSION}" "${JAVAFX_CLASSIFIER}"
download_jar "org.openjfx" "javafx-graphics" "${JAVAFX_VERSION}" "${JAVAFX_CLASSIFIER}"
download_jar "org.openjfx" "javafx-controls" "${JAVAFX_VERSION}" "${JAVAFX_CLASSIFIER}"

echo "Dependencies are available in ${LIB_DIR}"
