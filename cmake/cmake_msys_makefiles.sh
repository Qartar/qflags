#!/bin/bash
ROOT_DIR="$(dirname $0)/.."
cmake -G "MSYS Makefiles" -H$ROOT_DIR -B$ROOT_DIR/_build_msys $@
