#!/bin/bash
set -e

DIST_DIR=${1:-client/dist}
RESOURCE_XML_IN=${2:-resources.xml.in}
RESOURCE_XML_OUT="argon.resources.xml"

if [ ! -d "$DIST_DIR" ]; then
    echo "Error: $DIST_DIR not found. Make sure the submodule is initialized."
    exit 1
fi

echo "Generating $RESOURCE_XML_OUT from $DIST_DIR/..."

awk -v dist_dir="$DIST_DIR" '
/@DIST_FILES@/ {
    cmd = "find \"" dist_dir "\" -type f 2>/dev/null"
    while ((cmd | getline result) > 0) {
        sub("^" dist_dir "/?", "", result)
        if (result != "") {
            gsub(/"/, "\\\"", result)
            print "    <file alias=\"" result "\" compressed=\"true\">" result "</file>"
        }
    }
    close(cmd)
    next
}
{ print }
' "$RESOURCE_XML_IN" > "$RESOURCE_XML_OUT"

echo "Generated $RESOURCE_XML_OUT"