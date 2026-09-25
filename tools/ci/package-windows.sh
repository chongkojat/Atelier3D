#!/usr/bin/env bash
# ===============================================
# Packages the cross-compiled Windows build into a zip that runs on any Windows PC
# Usage: bash tools/ci/package-windows.sh [Debug|Release]   (run build.sh <config> windows first)
# ===============================================
set -euo pipefail

CONFIG="${1:-Release}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$ROOT/build/ci-windows-$CONFIG"
DIST_DIR="$ROOT/build/dist"
PKG_NAME="Atelier3D-windows-$CONFIG"
PKG_DIR="$DIST_DIR/$PKG_NAME"

rm -rf "$DIST_DIR"
mkdir -p "$PKG_DIR/projects"

# Executables
cp "$BUILD_DIR"/projects/*/*.exe "$PKG_DIR/"

# Game project assets (the apps look for projects/<name>/Assets next to the exe)
# Same rule as the editor: a game project is a folder with Assets/Scenes/Default.scene
for project in "$ROOT"/Source/projects/*/; do
    if [ -f "$project/Assets/Scenes/Default.scene" ]; then
        name="$(basename "$project")"
        mkdir -p "$PKG_DIR/projects/$name"
        cp -r "$project/Assets" "$PKG_DIR/projects/$name/"
    fi
done

(cd "$DIST_DIR" && cmake -E tar cf "$PKG_NAME.zip" --format=zip "$PKG_NAME")

echo "Package created: $DIST_DIR/$PKG_NAME.zip"
ls -la "$PKG_DIR"
