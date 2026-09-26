#!/usr/bin/env bash
# ===============================================
# Static analysis of our own code with cppcheck (needs: apt install cppcheck)
# Usage: bash tools/ci/static-analysis.sh
# Report: apps/desktop/build/reports/cppcheck.xml
# Fails only on "error" severity findings (likely bugs); other findings are reported
# ===============================================
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
REPORT_DIR="$ROOT/apps/desktop/build/reports"
REPORT="$REPORT_DIR/cppcheck.xml"

mkdir -p "$REPORT_DIR"

echo "==============================================="
echo "  Atelier3D static analysis (cppcheck)"
echo "==============================================="
cppcheck --version

# Only our code is analysed; third-party code under apps/desktop/libraries/ is skipped
#   -UCreateWindow: the "#ifdef CreateWindow / #undef" guards make cppcheck try a config where
#                   CreateWindow is a macro, which never happens in a real build (false syntaxError)
#   functionStatic: "method can be static" is noise for engine APIs designed as instance methods
CPPCHECK_ARGS=(
    --enable=warning,style,performance,portability
    --std=c++20 --language=c++
    --inline-suppr
    --suppress=missingIncludeSystem
    --suppress=unmatchedSuppression
    --suppress=functionStatic
    -UCreateWindow
    -I "$ROOT/apps/desktop/projects/atelier_engine/include"
    -I "$ROOT/apps/desktop/projects/atelier_editor/include"
    -j "$(nproc)"
    --quiet
    "$ROOT/apps/desktop/projects" "$ROOT/tests"
)

# Readable findings in the console log
cppcheck "${CPPCHECK_ARGS[@]}" --template='{file}:{line}: {severity}: {message} [{id}]'

# XML report that Jenkins displays
cppcheck "${CPPCHECK_ARGS[@]}" --xml --output-file="$REPORT"

TOTAL=$(grep -c '<error ' "$REPORT" || true)
ERRORS=$(grep -c 'severity="error"' "$REPORT" || true)
echo "cppcheck findings: $TOTAL total, $ERRORS with severity 'error'"
echo "Report: $REPORT"

if [ "$ERRORS" -gt 0 ]; then
    echo "ERROR: cppcheck found likely bugs (severity 'error')" >&2
    exit 1
fi
