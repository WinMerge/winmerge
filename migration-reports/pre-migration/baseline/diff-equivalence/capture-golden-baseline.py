#!/usr/bin/env python3
"""
capture-golden-baseline.py — Portable golden baseline capture using GNU diff.

Produces golden-baseline.json consumed by the JUnit5 parity harness.
Runs on any system with Python 3 and GNU diff (macOS, Linux, Docker).

WinMerge's core diff engine uses the Myers diff algorithm, same as GNU diff.
This script captures:
  - diff exit codes (0 = identical, 1 = different, 2 = error)
  - unified diff output (line-level hunks)
  - binary comparison results via cmp
  - elapsed time and hunk count per case

Usage:
    python3 capture-golden-baseline.py [--corpus-dir DIR] [--output FILE]
"""

import argparse
import json
import os
import platform
import subprocess
import time
from datetime import datetime, timezone
from pathlib import Path


def get_diff_version() -> str:
    try:
        result = subprocess.run(
            ["diff", "--version"], capture_output=True, text=True, timeout=5
        )
        return result.stdout.split("\n")[0] if result.stdout else "unknown"
    except Exception:
        return "unknown"


def capture_text_case(left: Path, right: Path) -> dict:
    """Run GNU diff -u on two text files. Return exit code, comparison, hunks, output."""
    start = time.monotonic()
    result = subprocess.run(
        ["diff", "-u", str(left), str(right)],
        capture_output=True,
        text=True,
        timeout=120,
    )
    elapsed_ms = round((time.monotonic() - start) * 1000, 2)

    exit_code = result.returncode
    diff_output = result.stdout
    if exit_code == 0:
        comparison = "identical"
    elif exit_code == 1:
        comparison = "different"
    else:
        comparison = "error"
        diff_output = result.stderr or result.stdout

    hunk_count = sum(1 for line in diff_output.splitlines() if line.startswith("@@"))

    # Truncate large diff output for JSON manageability
    if len(diff_output) > 4096:
        diff_output = diff_output[:4096] + f"... [truncated, {len(result.stdout)} bytes total]"

    return {
        "exit_code": exit_code,
        "comparison": comparison,
        "hunk_count": hunk_count,
        "elapsed_ms": elapsed_ms,
        "diff_output": diff_output,
    }


def capture_binary_case(left: Path, right: Path) -> dict:
    """Compare two binary files via cmp. Return exit code, comparison, output."""
    start = time.monotonic()

    # Silent comparison first
    result = subprocess.run(
        ["cmp", "-s", str(left), str(right)],
        capture_output=True,
        timeout=30,
    )
    elapsed_ms = round((time.monotonic() - start) * 1000, 2)

    exit_code = result.returncode
    if exit_code == 0:
        comparison = "identical"
        diff_output = ""
    else:
        comparison = "different"
        # Get byte-level differences
        detail = subprocess.run(
            ["cmp", "-l", str(left), str(right)],
            capture_output=True,
            text=True,
            timeout=30,
        )
        diff_output = detail.stdout.strip()

    return {
        "exit_code": 0 if exit_code == 0 else 1,
        "comparison": comparison,
        "hunk_count": 0,
        "elapsed_ms": elapsed_ms,
        "diff_output": diff_output,
    }


def main():
    parser = argparse.ArgumentParser(description="Capture golden baseline for diff equivalence testing")
    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parents[3]

    parser.add_argument(
        "--corpus-dir",
        type=Path,
        default=repo_root / "Testing" / "Data" / "DiffAlgorithmCorpus",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=script_dir / "golden-baseline.json",
    )
    args = parser.parse_args()

    manifest_path = args.corpus_dir / "manifest.json"
    if not manifest_path.exists():
        raise SystemExit(f"ERROR: manifest not found: {manifest_path}")

    with open(manifest_path) as f:
        manifest = json.load(f)

    # Generate large fixtures if missing
    large_left = args.corpus_dir / "cases" / "large_over_1mb.left.txt"
    gen_script = args.corpus_dir / "generate-large-fixtures.sh"
    if not large_left.exists() and gen_script.exists():
        print("Generating large fixtures...")
        subprocess.run(["bash", str(gen_script)], check=True)

    print(f"Capturing golden baseline...")
    print(f"  Corpus: {manifest_path}")
    print(f"  Output: {args.output}")
    print()

    results = []
    for case in manifest["cases"]:
        case_id = case["id"]
        left_path = args.corpus_dir / case["left"]
        right_path = args.corpus_dir / case["right"]
        case_type = case["type"]
        expected = case["expected"]

        if not left_path.exists():
            print(f"  SKIP {case_id}: missing {left_path}")
            continue
        if not right_path.exists():
            print(f"  SKIP {case_id}: missing {right_path}")
            continue

        if case_type == "binary":
            capture = capture_binary_case(left_path, right_path)
        else:
            capture = capture_text_case(left_path, right_path)

        print(
            f"  {case_id}: exit={capture['exit_code']} "
            f"comparison={capture['comparison']} "
            f"hunks={capture['hunk_count']} "
            f"elapsed={capture['elapsed_ms']}ms"
        )

        results.append({
            "id": case_id,
            "left": case["left"],
            "right": case["right"],
            "type": case_type,
            "expected": expected,
            **capture,
        })

    # Assemble golden baseline
    golden = {
        "generated_at": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "status": "captured",
        "tool": "gnu-diff",
        "tool_version": get_diff_version(),
        "platform": f"{platform.system()} {platform.machine()}",
        "notes": (
            "Portable baseline using GNU diff (Myers algorithm). "
            "WinMerge-specific baseline optional — see capture-winmerge-baseline.ps1."
        ),
        "cases": results,
    }

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with open(args.output, "w") as f:
        json.dump(golden, f, indent=2, ensure_ascii=False)
        f.write("\n")

    print()
    print(f"Golden baseline written to: {args.output}")
    print(f"Cases captured: {len(results)}")

    # Validate: every case with expected=different should have comparison=different
    mismatches = [
        r for r in results
        if "different" in r["expected"] and r["comparison"] != "different"
    ]
    if mismatches:
        print(f"\nWARNING: {len(mismatches)} case(s) did not match expected outcome:")
        for m in mismatches:
            print(f"  {m['id']}: expected contains 'different', got '{m['comparison']}'")
    else:
        print("Validation: all cases match expected outcomes.")


if __name__ == "__main__":
    main()
