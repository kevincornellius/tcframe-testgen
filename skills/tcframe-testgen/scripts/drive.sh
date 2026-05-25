#!/usr/bin/env bash
# tcframe driver — compiles solution + scorer/communicator if present,
# builds the runner from spec.cpp, runs it, and exits non-zero on any
# failure so an agent loop can iterate.
#
# Usage (run inside the problem package directory, via WSL on Windows):
#   bash drive.sh                       # generate
#   bash drive.sh grade [--solution=./alt_solution]
#
# Set TCFRAME_SEED to vary the seed (default 0).

set -u
SEED="${TCFRAME_SEED:-0}"
CXX="${CXX:-g++}"
CXXFLAGS="${CXXFLAGS:--O2 -std=c++17}"

if [[ ! -f spec.cpp ]]; then
  echo "ERROR: spec.cpp not found in $(pwd)" >&2
  exit 2
fi

# tcframe alias is defined in interactive .bashrc — source it if running
# non-interactively. Fall back to direct script call.
if ! command -v tcframe >/dev/null 2>&1; then
  if [[ -n "${TCFRAME_HOME:-}" && -x "$TCFRAME_HOME/scripts/tcframe" ]]; then
    tcframe() { "$TCFRAME_HOME/scripts/tcframe" "$@"; }
  else
    # try sourcing user bashrc to pick up alias + env
    shopt -s expand_aliases
    [[ -f "$HOME/.bashrc" ]] && source "$HOME/.bashrc" >/dev/null 2>&1 || true
  fi
fi

if ! type tcframe >/dev/null 2>&1; then
  cat >&2 <<EOF
ERROR: 'tcframe' command not available.
  Set TCFRAME_HOME=/path/to/tcframe and ensure \$TCFRAME_HOME/scripts/tcframe
  exists, or add the alias to ~/.bashrc:
    export TCFRAME_HOME=~/tcframe
    alias tcframe=\$TCFRAME_HOME/scripts/tcframe
EOF
  exit 2
fi

# ---- compile solution if a source file is present and binary missing/stale ----
compile_if() {
  local src="$1" out="$2"
  if [[ -f "$src" ]] && { [[ ! -x "$out" ]] || [[ "$src" -nt "$out" ]]; }; then
    echo ">> $CXX $CXXFLAGS -o $out $src"
    $CXX $CXXFLAGS -o "$out" "$src" || return 1
  fi
}

compile_if solution.cpp     solution     || { echo "FAIL: solution.cpp didn't compile" >&2; exit 3; }
compile_if scorer.cpp       scorer       || { echo "FAIL: scorer.cpp didn't compile" >&2; exit 3; }
compile_if communicator.cpp communicator || { echo "FAIL: communicator.cpp didn't compile" >&2; exit 3; }

# ---- build the runner from spec.cpp ----
echo ">> tcframe build"
tcframe build || { echo "FAIL: spec.cpp didn't compile" >&2; exit 4; }

# ---- run ----
mode="${1:-generate}"
shift || true

if [[ "$mode" == "grade" ]]; then
  echo ">> ./runner grade --seed=$SEED $*"
  ./runner grade --seed="$SEED" "$@"
  rc=$?
  exit $rc
fi

echo ">> ./runner --seed=$SEED $*"
out=$(./runner --seed="$SEED" "$@" 2>&1)
rc=$?
printf '%s\n' "$out"

# Even when ./runner returns 0, individual cases may have FAILED.
# Parse and surface that as a non-zero exit.
if grep -qE "FAILED" <<<"$out"; then
  echo "" >&2
  echo "==== FAILED CASES ====" >&2
  grep -B0 -A4 "FAILED" <<<"$out" >&2
  exit 5
fi

if [[ $rc -ne 0 ]]; then
  exit $rc
fi

echo "==== ALL OK ===="
