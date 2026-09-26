#!/usr/bin/env bash
# ===============================================
# Lints our own code (needs: apt install clang-format clang-tidy)
# Usage: bash tools/ci/lint.sh [Debug|Release]   (run build.sh <config> linux first; clang-tidy reads its compile_commands.json)
# Reports: build/reports/clang-format.txt, build/reports/clang-tidy.txt
#   clang-format - fails if any file does not match .clang-format
#                  (fix locally with: clang-format -i <file>)
#   clang-tidy   - fails on bugprone-* / clang-analyzer-* findings (likely bugs, see .clang-tidy);
#                  other findings are reported
# Both checks always run so one report shows every problem
# ===============================================
set -euo pipefail

CONFIG="${1:-Debug}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$ROOT/build/ci-$CONFIG"
REPORT_DIR="$ROOT/build/reports"

mkdir -p "$REPORT_DIR"

echo "==============================================="
echo "  Atelier3D lint (clang-format, clang-tidy)"
echo "==============================================="
clang-format --version
clang-tidy --version

FAILED=0

# Only our code is linted; third-party code under Source/libraries is skipped
mapfile -t FILES < <(cd "$ROOT" && git ls-files -- 'Source/projects/*.cpp' 'Source/projects/*.hpp' 'Source/projects/*.h' \
                                                   'Source/tests/*.cpp' 'Source/tests/*.hpp' 'Source/tests/*.h')

echo "[1/2] clang-format (${#FILES[@]} files)..."
if (cd "$ROOT" && clang-format --dry-run --Werror "${FILES[@]}") > "$REPORT_DIR/clang-format.txt" 2>&1; then
    echo "clang-format: all files formatted"
else
    # File names only: the full diagnostics are in the report (Jenkins shows them on the "clang-format" page)
    grep -oE '^[^:]+:[0-9]+' "$REPORT_DIR/clang-format.txt" | cut -d: -f1 | sort -u | sed 's/^/  needs formatting: /'
    echo "ERROR: files above do not match .clang-format (run: clang-format -i <file>)" >&2
    FAILED=1
fi

echo "[2/2] clang-tidy..."
if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
    echo "ERROR: $BUILD_DIR/compile_commands.json not found (run: bash tools/ci/build.sh $CONFIG linux)" >&2
    exit 1
fi

# The regex picks our .cpp files out of the compile database (headers are covered via HeaderFilterRegex)
if run-clang-tidy -p "$BUILD_DIR" -quiet -j "$(nproc)" '/Source/(projects|tests)/.*\.cpp$' \
        > "$REPORT_DIR/clang-tidy.txt" 2>&1; then
    TIDY_OK=1
else
    TIDY_OK=0
fi

# Readable findings in the console log, as "file:line [severity] message [check]"
# (not in compiler format, so the GCC warnings parser does not count them again; Jenkins reads the report file)
grep -E ': (warning|error): ' "$REPORT_DIR/clang-tidy.txt" | sort -u \
    | sed -E -e "s#^$ROOT/##" -e 's#^([^:]+:[0-9]+):[0-9]+: (warning|error): #  \1 [\2] #' || true

WARNINGS=$(grep -E ': warning: ' "$REPORT_DIR/clang-tidy.txt" | sort -u | wc -l || true)
ERRORS=$(grep -E ': error: ' "$REPORT_DIR/clang-tidy.txt" | sort -u | wc -l || true)
echo "clang-tidy findings: $WARNINGS warnings, $ERRORS errors"

if [ "$TIDY_OK" -ne 1 ]; then
    # Non-zero exit without an error line means clang-tidy itself failed; show the full log
    [ "$ERRORS" -eq 0 ] && cat "$REPORT_DIR/clang-tidy.txt"
    echo "ERROR: clang-tidy found likely bugs (see .clang-tidy WarningsAsErrors)" >&2
    FAILED=1
fi

echo "Reports: $REPORT_DIR/clang-format.txt, $REPORT_DIR/clang-tidy.txt"
exit "$FAILED"
