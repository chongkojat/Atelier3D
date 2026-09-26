#!/usr/bin/env bash
# ===============================================
# Runs the unit tests and writes a JUnit-style report for Jenkins
# Usage: bash tools/ci/test.sh [Debug|Release]   (run build.sh <config> linux first)
# Report: apps/desktop/build/reports/tests.xml
# ===============================================
set -euo pipefail

CONFIG="${1:-Debug}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$ROOT/apps/desktop/build/ci-$CONFIG"
REPORT_DIR="$ROOT/apps/desktop/build/reports"

mkdir -p "$REPORT_DIR"
rm -f "$REPORT_DIR/tests.xml"

echo "==============================================="
echo "  Atelier3D unit tests - $CONFIG"
echo "==============================================="

# GoogleTest's own XML report groups results by test suite in Jenkins
"$BUILD_DIR/bin/AtelierTests" --gtest_output="xml:$REPORT_DIR/tests.xml"
