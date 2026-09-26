#!/usr/bin/env bash
# ===============================================
# Packages a build into a downloadable archive with the editor executable
# Usage: bash tools/ci/package.sh [Debug|Release] [linux|windows]   (run build.sh with the same arguments first)
# Output: apps/desktop/build/dist/Atelier3D-linux-<config>.tar.gz or apps/desktop/build/dist/Atelier3D-windows-<config>.zip
# ===============================================
set -euo pipefail

CONFIG="${1:-Release}"
TARGET="${2:-linux}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
DIST_DIR="$ROOT/apps/desktop/build/dist"
PKG_NAME="Atelier3D-$TARGET-$CONFIG"
PKG_DIR="$DIST_DIR/$PKG_NAME"

case "$TARGET" in
    linux)
        BUILD_DIR="$ROOT/apps/desktop/build/ci-$CONFIG"
        ARCHIVE="$PKG_NAME.tar.gz"
        ;;
    windows)
        BUILD_DIR="$ROOT/apps/desktop/build/ci-windows-$CONFIG"
        ARCHIVE="$PKG_NAME.zip"
        ;;
    *)
        echo "ERROR: unknown target '$TARGET' (expected linux or windows)" >&2
        exit 1
        ;;
esac

# Only replace this target's package so linux and windows packages can sit side by side
rm -rf "$PKG_DIR" "$DIST_DIR/$ARCHIVE"
mkdir -p "$PKG_DIR"

# Executable: the editor (the unit test binary is not shipped); it creates games in projects/ next to itself
EXT=""
[ "$TARGET" = "windows" ] && EXT=".exe"
cp "$BUILD_DIR/bin/AtelierEditor$EXT" "$PKG_DIR/"

if [ "$TARGET" = "windows" ]; then
    (cd "$DIST_DIR" && cmake -E tar cf "$ARCHIVE" --format=zip "$PKG_NAME")
else
    # tar.gz keeps the executable permission on the Linux binaries
    (cd "$DIST_DIR" && tar czf "$ARCHIVE" "$PKG_NAME")
fi

echo "Package created: $DIST_DIR/$ARCHIVE"
ls -la "$PKG_DIR"
