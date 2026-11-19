#!/bin/bash
# Fetches the pinned sources into third_party/: LOVE 11.5 and LuaJIT.
set -e
R="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
mkdir -p "$R/third_party" && cd "$R/third_party"
if [ ! -d love-11.5 ]; then
    curl -fL -o love-src.tar.gz https://github.com/love2d/love/releases/download/11.5/love-11.5-linux-src.tar.gz
    tar -xzf love-src.tar.gz
    rm love-src.tar.gz
fi
[ -d LuaJIT ] || git -c core.autocrlf=false clone https://github.com/LuaJIT/LuaJIT LuaJIT
(cd LuaJIT && git checkout 63c8f1a9b479ba5518cf6a170ff7729178bc3cfd)
