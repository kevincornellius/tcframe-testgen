---
name: tcframe-testgen
description: >
  Author and validate tcframe (C++) competitive-programming problem packages —
  write spec.cpp, design strong test cases for batch/subtask/multi-case/interactive/custom-scorer
  problems, and iteratively re-run tcframe until every case passes.
  Use this skill whenever the user mentions tcframe, spec.cpp, problem packages,
  generating testdata, fixing FAILED test cases, subtasks, test groups,
  interactive problems with communicators, or custom scorers — even if they
  don't name the framework directly. Also use it for any C++ competitive-programming
  task that involves writing or repairing test generators.
  Required: bash + g++ + tcframe (WSL on Windows; PowerShell alone is insufficient).
---

# tcframe test-case generator

You help the user author and validate **tcframe** problem packages. A
package is a directory containing `spec.cpp` (problem spec + test
spec), at least one `solution.cpp`, and optionally `scorer.cpp` /
`communicator.cpp`. The runner emits `.in/.out` files under `tc/`.

**Read [references/syntax.md](references/syntax.md) before writing any
tcframe code.** It is the condensed reference for every macro (`LINE`,
`LINES`, `GRID`, `CASE`, `CONS`, `Counter`, …), built-in validator,
lifecycle hook, and style flag. tcframe macros do not behave like raw
C++ — guessing produces specs that fail to compile or fail to
generate. Upstream authoritative docs live at
<https://github.com/ia-toki/tcframe/tree/main/web/docs> — only fetch
from there if `syntax.md` does not cover what you need.

You **do not write solutions.** If `solution.cpp` is missing, stop and
ask the user to provide one (and compile it). If the solution's output
gets rejected during generation, flag that to the user as a possible
wrong solution — do not silently change the output format to make the
error go away.

## Requirements

- `bash`, `g++` with C++17, tcframe (`$TCFRAME_HOME` set + `tcframe` on
  PATH).
- On Windows: run inside WSL. PowerShell alone will not work because
  the `tcframe` wrapper is a bash script that resolves `$TCFRAME_HOME`
  and a bash alias.

If `tcframe` is missing, see [Installation](#installation-windows--wsl).

## The driver

Every operation goes through [scripts/drive.sh](scripts/drive.sh) run
from inside the problem package. It compiles `solution.cpp` /
`scorer.cpp` / `communicator.cpp` if present, runs `tcframe build`,
runs `./runner`, **re-greps stdout for `FAILED`**, and exits non-zero
on any failure — designed for an agent loop.

```bash
# On Windows, prefix every command with:  wsl -e bash -lc '...'
DRIVE="$(find . -path '*/.claude/skills/tcframe-testgen/scripts/drive.sh' | head -n1)"
cd path/to/problem-package
bash "$DRIVE"                                        # generate
bash "$DRIVE" grade --solution=./alt-solution        # local grading
```

Env vars the driver honors: `TCFRAME_SEED` (default 0), `CXX`, `CXXFLAGS`.

## The iteration loop (main workflow)

When the user asks you to generate / repair test cases for a problem:

1. `ls` the package. Confirm `spec.cpp` + `solution.cpp` are present.
   If `solution.cpp` is missing, **stop and ask** — you will not
   invent solutions.
2. Read `spec.cpp` and any `statement*.md` to understand the problem.
3. Open [references/syntax.md](references/syntax.md) for the macro and
   lifecycle reference; open [references/patterns.md](references/patterns.md)
   for the edge-case checklist and generator templates; open
   [references/helpers.md](references/helpers.md) for drop-in private
   helpers (arrays, trees, graphs, strings, queries, permutations) —
   use these instead of writing your own to avoid `std::mt19937`.
4. Edit `spec.cpp` to add or strengthen `CASE(...)` lines following
   the boundary-first, structural-second, random-last ordering shown
   in [assets/boilerplate/spec-batch.cpp](assets/boilerplate/spec-batch.cpp).
5. Run the driver. On Windows: `wsl -e bash -lc "cd <pkg> && bash <drive>"`.
6. If exit code ≠ 0, parse the `FAILED` block:
   - `Does not satisfy constraints, on: <CONS>` → generator bug. The
     case violates the listed constraint (often a subtask constraint).
     Tighten the generator or move the case to a different test group.
   - `Expected: <token>` / `Expected: <space> after variable X` →
     malformed sample test case literal: `Input({...})` does not match
     `InputFormat()` (whitespace, line breaks).
   - `does not conform to output format` → the **solution's** output
     does not parse against `OutputFormat()`. Likely a wrong solution
     OR the output format is wrong. **Tell the user** before changing
     either.
   - g++ compile error under `tcframe build` → fix `spec.cpp` syntax.
     Common: forgot `% SIZE(...)`, mismatched braces in `CASE(...)`,
     used an unsupported type as an I/O variable.
7. Re-run. Repeat until you see `==== ALL OK ====`.

Keep iterating without consulting the user. Only stop to consult when
(a) the solution itself looks wrong (rejected output), or (b) you've
tried 3+ rounds on the same `FAILED` reason without progress.

## Boilerplates

Copy one of these to a new package's `spec.cpp` and edit:

| Boilerplate | Use when |
|---|---|
| [assets/boilerplate/spec-batch.cpp](assets/boilerplate/spec-batch.cpp) | Plain batch problem, no subtasks |
| [assets/boilerplate/spec-subtask.cpp](assets/boilerplate/spec-subtask.cpp) | Has subtasks + test groups |
| [assets/boilerplate/spec-multicase.cpp](assets/boilerplate/spec-multicase.cpp) | ICPC-style, multiple cases per file |
| [assets/boilerplate/spec-scorer.cpp](assets/boilerplate/spec-scorer.cpp) | Multiple valid outputs / partial credit |
| [assets/boilerplate/spec-interactive.cpp](assets/boilerplate/spec-interactive.cpp) | Interactive (uses a communicator) |
| [assets/boilerplate/scorer.cpp](assets/boilerplate/scorer.cpp) | Custom scorer template |
| [assets/boilerplate/communicator.cpp](assets/boilerplate/communicator.cpp) | Communicator template |

## Best practices (read before writing CASE lines)

These come up in nearly every problem; baking them in saves rounds
of failure.

- **Slug = directory name.** Output files are `<slug>_<n>.in`. Pick a
  short, dash-separated name and don't change it later — it's baked
  into every output filename.
- **Cover edges first.** Smallest input, max input, all-same values,
  sorted, reverse-sorted, single distinct element, alternating values,
  boundary values at start/middle/end. See
  [references/patterns.md](references/patterns.md) for the checklist
  per data structure.
- **Random ≠ strong.** Hand-write structural cases (path graphs, star
  graphs, palindromic strings, monotone arrays) **before** adding
  randoms. Randoms catch unknown unknowns; handmade cases catch known
  unknowns. Use both.
- **Always use `rnd.nextInt(lo, hi)` for randomness — never
  `std::mt19937` or `std::rand`.** tcframe ships `rnd` for a reason:
  `./runner --seed=N` reseeds it reproducibly. Any other RNG ignores
  that flag and drifts when somebody else regenerates the package.
  If you find yourself reaching for `<random>`, stop and use `rnd`.
- **Subtask test groups follow the Venn diagram.** One `TestGroup` per
  closed region of the subtask Venn diagram. A case assigned to
  `Subtasks({2, 3})` **must** satisfy subtasks 2 and 3 *and violate*
  subtask 1, otherwise tcframe rejects generation.
- **Clear vectors in `BeforeTestCase()`.** Input variables persist
  across cases. Forgetting to clear means case 2 starts with case 1's
  leftovers and case 2 silently includes case 1's data.
- **Use built-in validators** where you can:
  `valueOf(N).isBetween(1, MAX)`,
  `eachElementOf(A).isBetween(1, MAXV)`,
  `elementsOf(A).areUnique()`,
  `eachCharacterOf(S).isBetween('a', 'z')`.
  Full list: [references/syntax.md](references/syntax.md).
- **Sample test cases are literal strings parsed against
  `InputFormat()`.** Whitespace and line breaks matter. Mismatches
  surface as `Expected: <space> after variable X`.
- **Multi-case (ICPC):** `InputFormat()` describes **one** case. `T`
  is declared as an input variable but **not** placed in
  `InputFormat()` — `Counter(T)` auto-prepends it.
- **Interactive (`InteractiveEvaluator()`):** **always pair with
  `NoOutput()`.** Sample test cases must call only `Input(...)`, not
  `Output(...)`. The communicator runs only during `./runner grade`,
  not during `./runner` generation.

## Installation (Windows → WSL)

tcframe is a header-only C++ library plus a `scripts/tcframe` bash
wrapper. The wrapper depends on `$TCFRAME_HOME` and a bash alias, so on
Windows you must run it inside WSL.

Quick check from PowerShell:

```powershell
wsl -e bash -lc "type tcframe; echo TCFRAME_HOME=$TCFRAME_HOME"
```

If `tcframe` is not found:

```bash
# inside WSL
git clone https://github.com/ia-toki/tcframe.git ~/tcframe
echo 'export TCFRAME_HOME=~/tcframe'                >> ~/.bashrc
echo 'alias tcframe=$TCFRAME_HOME/scripts/tcframe'  >> ~/.bashrc
source ~/.bashrc
tcframe   # should print: usage: tcframe <command>
sudo apt-get install -y build-essential   # g++ if missing
```

> **Known quirk:** `tcframe version` prints blank when run anywhere
> outside a package directory. It's harmless — `tcframe build` still
> works from inside the package. Don't rely on `tcframe version` for
> install detection; use `command -v tcframe` (which is what
> `drive.sh` does).

## Gotchas seen in real runs

- **`tcframe: command not found` from a non-interactive shell.** The
  install puts the alias in `~/.bashrc`, which only loads in
  interactive shells. `drive.sh` works around this by sourcing
  `.bashrc` and falling back to `$TCFRAME_HOME/scripts/tcframe`
  directly. Invoke from PowerShell with `wsl -e bash -lc` (login
  shell), not plain `wsl bash -c`.
- **CRLF line endings** on `.sh` scripts copied via Windows break
  bash. Save with LF, or `dos2unix drive.sh`.
- **`./runner` prints OK lines and exits 0 even when some cases
  FAILED.** `drive.sh` re-greps for `FAILED` and exits 5 in that
  case — trust the exit code, not the visual output.
- **`tcframe build` must run from the package directory** (upstream
  quirk). Always `cd` into the package first; the driver does this
  for you.
- Solution output files are regenerated every run. If you're
  debugging a stale scorer/communicator, `rm -rf tc/` first.

## Reference docs

In-skill (read these first):

- [references/syntax.md](references/syntax.md) — every macro,
  validator, lifecycle hook, style flag, runner CLI flag, and common
  error message.
- [references/patterns.md](references/patterns.md) — generator
  templates for arrays, trees, graphs, strings, grids; edge-case
  checklist per data structure.
- [references/helpers.md](references/helpers.md) — drop-in private
  helper functions (all using `rnd`) for arrays, trees, graphs,
  strings, grids, queries, permutations. Copy into `TestSpec`'s
  `private:` section; these are the canonical tcframe-safe versions
  that replace any `std::mt19937`-based helpers you may have.

Upstream (only when in-skill refs don't cover it):
<https://github.com/ia-toki/tcframe/tree/main/web/docs>. Raw URL for
WebFetch:
`https://raw.githubusercontent.com/ia-toki/tcframe/main/web/docs/<path>`.
