#!/bin/bash
# Applies the Switch 2 changes to the LOVE sources in third_party/.
set -e
R="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$R/third_party/love-11.5"
for p in "$R"/patches/*.patch; do
    if git apply --check --ignore-whitespace "$p" 2>/dev/null; then
        git apply --ignore-whitespace "$p"
        echo "applied  $(basename "$p")"
    else
        echo "already applied or does not fit: $(basename "$p")"
    fi
done
