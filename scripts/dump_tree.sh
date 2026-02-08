#!/usr/bin/env bash

set -euo pipefail

DIR="${1:-.}"

if [ ! -d "$DIR" ]; then
    echo "Error: '$DIR' is not a directory"
    exit 1
fi

echo "=== TREE: $DIR ==="
echo

if command -v tree >/dev/null 2>&1; then
    tree -I "build|.git" "$DIR"
else
    find "$DIR" -print | sed -e 's;[^/]*/;|   ;g'
fi

echo
echo "=== FILE CONTENTS ==="

find "$DIR" -type f \
    ! -path "*/build/*" \
    ! -path "*/.git/*" \
    | sort | while read -r f; do

    echo
    echo "----- $f -----"
    cat "$f"
done
