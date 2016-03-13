#!/usr/bin/env sh
set -evx
env | sort

mkdir _build || true
cd _build
cmake ..
make
