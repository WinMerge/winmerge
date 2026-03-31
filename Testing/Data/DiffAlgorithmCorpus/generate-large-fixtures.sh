#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cases_dir="$root_dir/cases"
left_file="$cases_dir/large_over_1mb.left.txt"
right_file="$cases_dir/large_over_1mb.right.txt"

mkdir -p "$cases_dir"

: > "$left_file"
: > "$right_file"

for i in $(seq 0 74999); do
  printf 'line:%05d:stable\n' "$i" >> "$left_file"
  if [[ "$i" -eq 74000 ]]; then
    printf 'line:%05d:changed\n' "$i" >> "$right_file"
  else
    printf 'line:%05d:stable\n' "$i" >> "$right_file"
  fi
done

left_size=$(wc -c < "$left_file")
right_size=$(wc -c < "$right_file")

echo "Generated $left_file (${left_size} bytes)"
echo "Generated $right_file (${right_size} bytes)"
