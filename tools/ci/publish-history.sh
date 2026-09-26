#!/usr/bin/env bash
# ===============================================
# Records a build in the public build history page (run at the end of every build, pass or fail)
# Usage: bash tools/ci/publish-history.sh <publish_dir> <build_number> <result>
#   publish_dir - same folder as publish-downloads.sh, e.g. $JENKINS_HOME/userContent/atelier3d
#   result      - SUCCESS, FAILURE, UNSTABLE or ABORTED (Jenkins' currentBuild.currentResult)
# Output: <publish_dir>/index.html and style.css, rebuilt from <publish_dir>/history.tsv
# The page is plain HTML + a CSS file with no scripts or inline styles, because Jenkins serves
# userContent with a Content-Security-Policy that blocks them.
# ===============================================
set -euo pipefail

PUBLISH_DIR="${1:?usage: publish-history.sh <publish_dir> <build_number> <result>}"
BUILD_NUMBER="${2:?usage: publish-history.sh <publish_dir> <build_number> <result>}"
RESULT="${3:?usage: publish-history.sh <publish_dir> <build_number> <result>}"
if ! [[ "$BUILD_NUMBER" =~ ^[0-9]+$ ]]; then
    echo "ERROR: build number must be a number, got '$BUILD_NUMBER'" >&2
    exit 1
fi
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
HISTORY="$PUBLISH_DIR/history.tsv"
MAX_ROWS=200

mkdir -p "$PUBLISH_DIR"

# One tab-separated line per build: number, date, commit, subject, result
COMMIT="$(git -C "$ROOT" log -1 --format=%h 2>/dev/null || echo "-")"
SUBJECT="$(git -C "$ROOT" log -1 --format=%s 2>/dev/null | tr '\t\r' '  ' || true)"
DATE="$(date -u '+%Y-%m-%d %H:%M UTC')"

touch "$HISTORY"
# A re-run of the same build number replaces its row
grep -v -P "^${BUILD_NUMBER}\t" "$HISTORY" > "$HISTORY.tmp" || true
printf '%s\t%s\t%s\t%s\t%s\n' "$BUILD_NUMBER" "$DATE" "$COMMIT" "${SUBJECT:--}" "$RESULT" >> "$HISTORY.tmp"
mv "$HISTORY.tmp" "$HISTORY"

html_escape() {
    sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g' -e 's/>/\&gt;/g' -e 's/"/\&quot;/g'
}

# Download links for one folder of packages ("Windows" for .zip, "Linux" for .tar.gz)
package_links() {
    local dir="$1" href="$2" links="" file name label
    for file in "$dir"/*.zip "$dir"/*.tar.gz; do
        [ -f "$file" ] || continue
        name="$(basename "$file")"
        case "$name" in
            *.zip) label="Windows" ;;
            *) label="Linux" ;;
        esac
        links+="<a class=\"dl\" href=\"$href/$name\" title=\"$name\">$label</a> "
    done
    printf '%s' "$links"
}

result_badge() {
    case "$1" in
        SUCCESS) echo '<span class="badge pass">Passed</span>' ;;
        FAILURE) echo '<span class="badge fail">Failed</span>' ;;
        UNSTABLE) echo '<span class="badge warn">Unstable</span>' ;;
        *) echo "<span class=\"badge other\">$(printf '%s' "$1" | html_escape)</span>" ;;
    esac
}

LATEST_HTML='<p class="muted">No build has passed yet.</p>'
if [ -f "$PUBLISH_DIR/latest/BUILD" ]; then
    latest_number="$(tr -dc '0-9' < "$PUBLISH_DIR/latest/BUILD")"
    LATEST_HTML="<p>Build #$latest_number &nbsp; $(package_links "$PUBLISH_DIR/latest" "latest")</p>"
fi

ROWS=""
while IFS=$'\t' read -r number date commit subject result; do
    [ -n "$number" ] || continue
    if [ -d "$PUBLISH_DIR/builds/$number" ]; then
        downloads="$(package_links "$PUBLISH_DIR/builds/$number" "builds/$number")"
    elif [ "$result" = "SUCCESS" ]; then
        downloads='<span class="muted">expired</span>'
    else
        downloads='<span class="muted">&mdash;</span>'
    fi
    ROWS+="<tr><td class=\"num\">#$(printf '%s' "$number" | html_escape)</td>"
    ROWS+="<td>$(printf '%s' "$date" | html_escape)</td>"
    ROWS+="<td><code>$(printf '%s' "$commit" | html_escape)</code> $(printf '%s' "$subject" | html_escape)</td>"
    ROWS+="<td>$(result_badge "$result")</td><td>${downloads:-<span class=\"muted\">&mdash;</span>}</td></tr>"$'\n'
done < <(sort -t $'\t' -k1,1nr "$HISTORY" | head -n "$MAX_ROWS")

cat > "$PUBLISH_DIR/style.css" <<'CSS'
:root { --bg: #ffffff; --fg: #1f2328; --muted: #656d76; --line: #d0d7de; --head: #f6f8fa;
        --pass: #1a7f37; --fail: #cf222e; --warn: #9a6700; --link: #0969da; }
@media (prefers-color-scheme: dark) {
    :root { --bg: #0d1117; --fg: #e6edf3; --muted: #8d96a0; --line: #30363d; --head: #161b22;
            --pass: #3fb950; --fail: #f85149; --warn: #d29922; --link: #4493f8; }
}
body { margin: 0 auto; max-width: 960px; padding: 24px 16px; background: var(--bg); color: var(--fg);
       font: 15px/1.5 system-ui, -apple-system, "Segoe UI", sans-serif; }
h1 { font-size: 24px; margin: 0 0 4px; }
h2 { font-size: 17px; margin: 28px 0 8px; }
a { color: var(--link); }
.muted { color: var(--muted); }
.table-wrap { overflow-x: auto; }
table { border-collapse: collapse; width: 100%; }
th, td { text-align: left; padding: 8px 10px; border-bottom: 1px solid var(--line); vertical-align: top; }
th { background: var(--head); font-weight: 600; }
td.num { font-variant-numeric: tabular-nums; white-space: nowrap; }
code { font-size: 13px; }
.badge { display: inline-block; padding: 1px 8px; border-radius: 10px; font-size: 13px; font-weight: 600;
         border: 1px solid currentColor; white-space: nowrap; }
.pass { color: var(--pass); } .fail { color: var(--fail); } .warn { color: var(--warn); } .other { color: var(--muted); }
a.dl { margin-right: 8px; white-space: nowrap; }
CSS

cat > "$PUBLISH_DIR/index.html.tmp" <<HTML
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Atelier3D Builds</title>
<link rel="stylesheet" href="style.css">
</head>
<body>
<h1>Atelier3D Builds</h1>
<p class="muted">Automated builds of the main branch. Updated $(printf '%s' "$DATE" | html_escape).</p>
<h2>Latest passing build</h2>
$LATEST_HTML
<h2>History</h2>
<div class="table-wrap">
<table>
<thead><tr><th>Build</th><th>Date</th><th>Commit</th><th>Result</th><th>Downloads</th></tr></thead>
<tbody>
$ROWS</tbody>
</table>
</div>
</body>
</html>
HTML
mv "$PUBLISH_DIR/index.html.tmp" "$PUBLISH_DIR/index.html"

echo "Build history updated: $PUBLISH_DIR/index.html (build #$BUILD_NUMBER: $RESULT)"
