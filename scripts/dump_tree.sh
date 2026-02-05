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
  tree -I "build|.git|*drawio*|*png*" "$DIR"
else
  find "$DIR" -print | sed -e 's;[^/]*/;|   ;g'
fi

echo
echo "=== FILE CONTENTS ==="

find "$DIR" -type f \
  ! -path "*/build/*" \
  ! -path "*/.git/*" \
  ! -path "*/png/*" \
  ! -path "*/drawio/*" \
  ! -name "*.png" \
  ! -name "*.drawio" \
  | sort | while read -r f; do

  echo
  echo "----- $f -----"

  # Skip binary files (grep -Iq: "is this text?")
  if grep -Iq . "$f"; then
    cat "$f"
  else
    echo "[binary file skipped]"
  fi
done