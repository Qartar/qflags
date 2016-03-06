#!/bin/bash
ROOT_DIR="$(dirname $0)/.."
cmake -G "Unix Makefiles" -H$ROOT_DIR -B$ROOT_DIR/_build_unix $@
