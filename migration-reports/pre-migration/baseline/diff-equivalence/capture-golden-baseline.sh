#!/usr/bin/env bash
# capture-golden-baseline.sh — Portable golden baseline capture using GNU diff.
# Delegates to capture-golden-baseline.py for reliable JSON handling.
#
# Usage:  ./capture-golden-baseline.sh [--corpus-dir DIR] [--output FILE]

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

exec python3 "$SCRIPT_DIR/capture-golden-baseline.py" "$@"
