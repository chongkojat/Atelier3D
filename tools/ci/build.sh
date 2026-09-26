#!/usr/bin/env bash
# ===============================================
# CI Build Script for Linux (used by Jenkins, runnable locally)
# Usage: bash tools/ci/build.sh [Debug|Release|Package] [linux|windows]
#   linux   - native Linux build (default)
#   windows - Windows .exe cross-compiled with MinGW-w64 (needs: apt install mingw-w64)
# ===============================================
set -euo pipefail

CONFIG="${1:-Release}"
TARGET="${2:-linux}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
# The desktop app is the CMake project
SOURCE_DIR="$ROOT/apps/desktop"

EXTRA_ARGS=()
case "$TARGET" in
    linux)
        BUILD_DIR="$ROOT/apps/desktop/build/ci-$CONFIG"
        ;;
    windows)
        BUILD_DIR="$ROOT/apps/desktop/build/ci-windows-$CONFIG"
        EXTRA_ARGS=(-DCMAKE_TOOLCHAIN_FILE="$SOURCE_DIR/cmake/mingw-w64-x86_64.cmake")
        ;;
    *)
        echo "ERROR: unknown target '$TARGET' (expected linux or windows)" >&2
        exit 1
        ;;
esac

echo "==============================================="
echo "  Atelier3D CI build - $CONFIG ($TARGET)"
echo "==============================================="
cmake --version

# Always start from a clean build directory so every run is repeatable
rm -rf "$BUILD_DIR"

if command -v ninja >/dev/null 2>&1; then
    EXTRA_ARGS+=(-G Ninja)
fi

echo "[1/2] Configuring..."
cmake -S "$SOURCE_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$CONFIG" "${EXTRA_ARGS[@]}"

echo "[2/2] Building..."
cmake --build "$BUILD_DIR" --parallel

echo "Build succeeded: $BUILD_DIR"
