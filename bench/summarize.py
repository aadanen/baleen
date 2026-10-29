"""Summarize per-suite brench CSVs produced by run.sh.

usage: summarize.py RESULTS_DIR SUITE...

Exits nonzero if any optimized benchmark is incorrect, missing, or timed out.
"""

import csv
import statistics
import sys
from pathlib import Path


def load(path):
    results = {}
    with open(path) as f:
        for row in csv.DictReader(f):
            results.setdefault(row["benchmark"], {})[row["run"]] = row["result"]
    return results


def summarize(name, pairs):
    base = sum(b for b, _ in pairs)
    opt = sum(o for _, o in pairs)
    reductions = [1 - o / b for b, o in pairs]
    geomean = 1 - statistics.geometric_mean([o / b for b, o in pairs])
    improved = sum(r > 0 for r in reductions)
    worse = sum(r < 0 for r in reductions)
    print(
        f"{name:<6} {len(pairs):>5} {improved:>8} {worse:>5} "
        f"{(1 - opt / base) * 100:>9.2f}% {geomean * 100:>8.2f}% "
        f"{max(reductions) * 100:>8.2f}%   {base} -> {opt}"
    )


def main():
    out = Path(sys.argv[1])
    suites = sys.argv[2:]

    failures = []
    pairs = {}
    for suite in suites:
        for bench, runs in load(out / f"{suite}.csv").items():
            base, opt = runs.get("baseline", ""), runs.get("baleen", "")
            if not (base.isdigit() and opt.isdigit()):
                failures.append(f"{suite}/{bench}: baseline={base} baleen={opt}")
                continue
            pairs.setdefault(suite, []).append((int(base), int(opt)))

    print(f"{'suite':<6} {'n':>5} {'improved':>8} {'worse':>5} "
          f"{'aggregate':>10} {'geomean':>9} {'best':>9}")
    for suite in suites:
        if suite in pairs:
            summarize(suite, pairs[suite])
    summarize("all", [p for suite in suites for p in pairs.get(suite, [])])

    if failures:
        print(f"\n{len(failures)} failing benchmarks:")
        for f in failures:
            print(f"  {f}")
        sys.exit(1)


if __name__ == "__main__":
    main()
