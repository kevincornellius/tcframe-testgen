# tcframe-testgen

A skill for authoring and validating [tcframe](https://github.com/ia-toki/tcframe) competitive-programming problem packages.

It writes `spec.cpp`, fills in strong test cases, and iterates `drive.sh` until every case passes — handling batch, subtask, ICPC multi-case, interactive, and custom-scorer problems.

## Install

```
# drag dist/tcframe-testgen.skill into Claude Code
# or install via skills.sh once listed
```

## Requirements

| Requirement | Notes |
|---|---|
| `g++` with C++17 | `sudo apt-get install build-essential` |
| tcframe | See [Installation](#installation) below |
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

## Installation

```bash
# inside WSL
git clone https://github.com/ia-toki/tcframe.git ~/tcframe
echo 'export TCFRAME_HOME=~/tcframe'                >> ~/.bashrc
echo 'alias tcframe=$TCFRAME_HOME/scripts/tcframe'  >> ~/.bashrc
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
└── evals/                       # development evals (not shipped in .skill)
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

## Building from source

```bash
cd tcframe-skill-repo/.agents/skills/skill-creator   # or wherever skill-creator lives
python -m scripts.package_skill ../../tcframe-testgen ../../dist
```

## License

MIT
