#!/usr/bin/env bash
# ===============================================
# Publishes a build's packages as public downloads (run after package.sh)
# Usage: bash tools/ci/publish-downloads.sh <publish_dir> <build_number>
#   publish_dir - served by Jenkins, e.g. $JENKINS_HOME/userContent/atelier3d
#                 (<jenkins>/userContent/ needs only Overall/Read, so guests can download
#                  without seeing the job page, stage view or trends)
# Output:
#   <publish_dir>/builds/<build_number>/   packages of this build (the newest KEEP_BUILDS builds are kept)
#   <publish_dir>/latest/                  packages of the newest published build
# ===============================================
set -euo pipefail

PUBLISH_DIR="${1:?usage: publish-downloads.sh <publish_dir> <build_number>}"
BUILD_NUMBER="${2:?usage: publish-downloads.sh <publish_dir> <build_number>}"
KEEP_BUILDS="${KEEP_BUILDS:-20}"
if ! [[ "$BUILD_NUMBER" =~ ^[0-9]+$ ]]; then
    echo "ERROR: build number must be a number, got '$BUILD_NUMBER'" >&2
    exit 1
fi
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
DIST_DIR="$ROOT/apps/desktop/build/dist"

shopt -s nullglob
PACKAGES=("$DIST_DIR"/*.zip "$DIST_DIR"/*.tar.gz)
if [ ${#PACKAGES[@]} -eq 0 ]; then
    echo "ERROR: no packages in $DIST_DIR (run package.sh first)" >&2
    exit 1
fi

BUILD_DIR="$PUBLISH_DIR/builds/$BUILD_NUMBER"
mkdir -p "$BUILD_DIR"
cp "${PACKAGES[@]}" "$BUILD_DIR/"

# Swap in the new latest/ folder in one step, so a download never sees it half-copied
mkdir -p "$PUBLISH_DIR/latest.new"
cp "${PACKAGES[@]}" "$PUBLISH_DIR/latest.new/"
echo "$BUILD_NUMBER" > "$PUBLISH_DIR/latest.new/BUILD"
rm -rf "$PUBLISH_DIR/latest.old"
[ -d "$PUBLISH_DIR/latest" ] && mv "$PUBLISH_DIR/latest" "$PUBLISH_DIR/latest.old"
mv "$PUBLISH_DIR/latest.new" "$PUBLISH_DIR/latest"
rm -rf "$PUBLISH_DIR/latest.old"

# Keep disk use bounded: only the newest KEEP_BUILDS builds stay downloadable
# (their rows stay in the history page, marked "expired")
mapfile -t OLD_BUILDS < <(find "$PUBLISH_DIR/builds" -mindepth 1 -maxdepth 1 -type d -printf '%f\n' \
                          | grep -E '^[0-9]+$' | sort -n | head -n -"$KEEP_BUILDS")
for old in "${OLD_BUILDS[@]}"; do
    rm -rf "${PUBLISH_DIR:?}/builds/$old"
done

echo "Published build #$BUILD_NUMBER to $PUBLISH_DIR:"
ls -la "$BUILD_DIR"
