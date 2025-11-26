#!/usr/bin/env bash
set -euo pipefail

# release_zip.sh
# Collects files for a release into release/ and creates a single zip file
# Usage: release_zip.sh <ref_name>

REF_NAME=${1:-unknown}
OUT_DIR=release
ZIP_NAME=xyjson-${REF_NAME}.zip

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

copy_if_exists() {
  src="$1"
  dst="$OUT_DIR/$(basename "$src")"
  if [ -e "$src" ]; then
    if [ -d "$src" ]; then
      mkdir -p "$OUT_DIR/$(basename "$src")"
      cp -r "$src"/* "$OUT_DIR/$(basename "$src")/" || true
    else
      cp -p "$src" "$dst"
    fi
  fi
}

# Always collect header if present
copy_if_exists include/xyjson.h

# Common project files
copy_if_exists README.md
copy_if_exists README-en.md
copy_if_exists LICENSE
copy_if_exists CMakeLists.txt

# Directories to include
copy_if_exists docs
copy_if_exists utest
copy_if_exists examples
copy_if_exists cmake

# Add a small NOTICE if no assets besides header found
if [ -z "$(ls -A "$OUT_DIR")" ]; then
  echo "No assets found for release $REF_NAME" > "$OUT_DIR/NOTICE.txt"
fi

# Create zip from the contents of release/ (preserve directory structure)
pushd "$OUT_DIR" >/dev/null
zip -r -q "$ZIP_NAME" . || true
popd >/dev/null

echo "Created $OUT_DIR/$ZIP_NAME"
echo "$OUT_DIR/$ZIP_NAME"
