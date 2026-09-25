#!/usr/bin/env bash
# ===============================================
# Packages a build into a downloadable archive with the executables and game assets
# Usage: bash tools/ci/package.sh [Debug|Release] [linux|windows]   (run build.sh with the same arguments first)
# Output: build/dist/Atelier3D-linux-<config>.tar.gz or build/dist/Atelier3D-windows-<config>.zip
# ===============================================
set -euo pipefail

CONFIG="${1:-Release}"
TARGET="${2:-linux}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
DIST_DIR="$ROOT/build/dist"
PKG_NAME="Atelier3D-$TARGET-$CONFIG"
PKG_DIR="$DIST_DIR/$PKG_NAME"

case "$TARGET" in
    linux)
        BUILD_DIR="$ROOT/build/ci-$CONFIG"
        ARCHIVE="$PKG_NAME.tar.gz"
        ;;
    windows)
        BUILD_DIR="$ROOT/build/ci-windows-$CONFIG"
        ARCHIVE="$PKG_NAME.zip"
        ;;
    *)
        echo "ERROR: unknown target '$TARGET' (expected linux or windows)" >&2
        exit 1
        ;;
esac

# Only replace this target's package so linux and windows packages can sit side by side
rm -rf "$PKG_DIR" "$DIST_DIR/$ARCHIVE"
mkdir -p "$PKG_DIR/projects"

# Executables (the apps under projects/; the unit test binary is not shipped)
if [ "$TARGET" = "windows" ]; then
    cp "$BUILD_DIR"/projects/*/*.exe "$PKG_DIR/"
else
    find "$BUILD_DIR/projects" -mindepth 2 -maxdepth 2 -type f -name 'GAM300*' -perm -u+x \
        -exec cp {} "$PKG_DIR/" \;
fi

# Game project assets (the apps look for projects/<name>/Assets next to the executable)
# Same rule as the editor: a game project is a folder with Assets/Scenes/Default.scene
for project in "$ROOT"/Source/projects/*/; do
    if [ -f "$project/Assets/Scenes/Default.scene" ]; then
        name="$(basename "$project")"
        mkdir -p "$PKG_DIR/projects/$name"
        cp -r "$project/Assets" "$PKG_DIR/projects/$name/"
    fi
done

if [ "$TARGET" = "windows" ]; then
    (cd "$DIST_DIR" && cmake -E tar cf "$ARCHIVE" --format=zip "$PKG_NAME")
else
    # tar.gz keeps the executable permission on the Linux binaries
    (cd "$DIST_DIR" && tar czf "$ARCHIVE" "$PKG_NAME")
fi

echo "Package created: $DIST_DIR/$ARCHIVE"
ls -la "$PKG_DIR"
