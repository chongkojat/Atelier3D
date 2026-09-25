#!/usr/bin/env bash
# ===============================================
# CI Build Script for Linux (used by Jenkins, runnable locally)
# Usage: bash tools/ci/build.sh [Debug|Release|Package]
# ===============================================
set -euo pipefail

CONFIG="${1:-Release}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$ROOT/build/ci-$CONFIG"

echo "==============================================="
echo "  Atelier3D CI build - $CONFIG"
echo "==============================================="
cmake --version
c++ --version | head -n 1

# Always start from a clean build directory so every run is repeatable
rm -rf "$BUILD_DIR"

GENERATOR=()
if command -v ninja >/dev/null 2>&1; then
    GENERATOR=(-G Ninja)
fi

echo "[1/2] Configuring..."
cmake -S "$ROOT/Source" -B "$BUILD_DIR" "${GENERATOR[@]}" -DCMAKE_BUILD_TYPE="$CONFIG"

echo "[2/2] Building..."
cmake --build "$BUILD_DIR" --parallel

echo "Build succeeded: $BUILD_DIR"
