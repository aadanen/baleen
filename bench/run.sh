#!/bin/bash
# Reproduce baleen's dynamic instruction count results on the bril benchmarks.
#
# usage: bench/run.sh [path/to/bril]
#
# The bril checkout defaults to $BRIL_DIR, then ../bril next to this repo.
# Requires bril2json, brili, and brench on PATH:
#   pip install path/to/bril/brench
set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
BRIL=$(cd "${1:-${BRIL_DIR:-$ROOT/../bril}}" && pwd)
SUITES="core float mem mixed long"
OUT=$ROOT/bench/results

for tool in bril2json brili brench python3; do
    if ! command -v "$tool" > /dev/null; then
        echo "error: $tool not found on PATH" >&2
        exit 1
    fi
done

cmake -S "$ROOT" -B "$ROOT/build" > /dev/null
cmake --build "$ROOT/build" --target brilopt -j "$(nproc)" > /dev/null
export BRILOPT=$ROOT/build/brilopt

# one brench run per suite since benchmark names are only unique per suite
mkdir -p "$OUT"
for suite in $SUITES; do
    echo "running $suite..." >&2
    brench -j "$(nproc)" "$ROOT/bench/brench.toml" \
        "$BRIL"/benchmarks/"$suite"/*.bril > "$OUT/$suite.csv"
done

python3 "$ROOT/bench/summarize.py" "$OUT" $SUITES
