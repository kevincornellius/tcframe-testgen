# tcframe-testgen

A Claude Code skill for authoring and validating [tcframe](https://github.com/ia-toki/tcframe) competitive-programming problem packages.

It writes `spec.cpp`, fills in strong test cases, and iterates `drive.sh` until every case passes — handling batch, subtask, ICPC multi-case, interactive, and custom-scorer problems.

## Install

Clone the `tcframe-testgen/` folder into your Claude skills directory:

```bash
git clone https://github.com/YOUR_USER/tcframe-testgen .claude/skills/tcframe-testgen
```

Or if your project uses `.agents/skills/` symlinked from `.claude/`:

```bash
git clone https://github.com/YOUR_USER/tcframe-testgen .agents/skills/tcframe-testgen
```

Claude picks it up automatically on next invocation.

## Requirements

| Requirement | Notes |
|---|---|
| `g++` with C++17 | `sudo apt-get install build-essential` |
| tcframe | See [tcframe setup](#tcframe-setup) below |
| WSL | Windows only — tcframe's wrapper is a bash script |

## What it does

- Reads your `spec.cpp` and `statement.md` to understand the problem
- Fills `TestCases()` (or `TestGroupX()` for subtask problems) with:
  - boundary cases first (N=1, N=MAX, all-same, sorted, reverse)
  - structural cases (path trees, star trees, palindromes, …)
  - random cases using `rnd` (never `std::mt19937`)
- Runs `drive.sh` (compile → `tcframe build` → `./runner`) and loops on errors
- Handles all tcframe problem types:
  - Batch (plain, subtask + test groups, ICPC multi-case)
  - Interactive (`InteractiveEvaluator` + communicator)
  - Custom scorer (`CustomScorer`)
- Stops and asks if the solution's output looks wrong — never silently changes output format

## tcframe setup

```bash
# inside WSL (or Linux/macOS terminal)
git clone https://github.com/ia-toki/tcframe.git ~/tcframe
echo 'export TCFRAME_HOME=~/tcframe'               >> ~/.bashrc
echo 'alias tcframe=$TCFRAME_HOME/scripts/tcframe' >> ~/.bashrc
source ~/.bashrc
sudo apt-get install -y build-essential
```

Verify:
```bash
command -v tcframe && echo OK
```

## Skill layout

```
tcframe-testgen/
├── SKILL.md                     # agent instructions
├── scripts/drive.sh             # compile + build + run harness
├── references/
│   ├── syntax.md                # every tcframe macro, validator, lifecycle hook
│   ├── patterns.md              # edge-case checklist per data structure
│   └── helpers.md               # drop-in private helpers (arrays, trees, graphs, …)
├── assets/boilerplate/          # spec.cpp templates per problem type
└── evals/                       # development evals (kept for transparency, not user-facing)
    ├── evals.json
    ├── trigger-eval.json
    └── fixtures/                # stub problem packages for benchmarking
```

## Using the skill

Point Claude at your problem package:

```
I have a batch problem at problems/my-problem/. spec.cpp has the format
and constraints, TestCases() is empty. solution.cpp is correct. Fill in
strong test cases and run tcframe until all cases pass.
```

Or for fixing a broken spec:

```
My tcframe runner keeps failing with 'Does not satisfy constraints'. Here
is my spec.cpp. Fix it and iterate until drive.sh exits 0.
```

## License

MIT
