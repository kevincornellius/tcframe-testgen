# tcframe syntax reference (condensed)

Distilled from the official docs. Upstream:
<https://github.com/ia-toki/tcframe/tree/main/web/docs>. tcframe macros
don't behave like raw C++ — **consult this file before guessing**.

---

## Spec file skeleton

```cpp
#include <bits/stdc++.h>
#include <tcframe/spec.hpp>
using namespace std;
using namespace tcframe;

class ProblemSpec : public BaseProblemSpec {
protected:
    // I/O variables (declared once; used in formats, constraints, AND test cases)
    int N;
    vector<int> A;
    long long result;

    void InputFormat()  { /* I/O segments */ }
    void OutputFormat() { /* I/O segments */ }
    void GradingConfig(){ TimeLimit(2); MemoryLimit(256); }
    void StyleConfig()  { /* optional: CustomScorer / NoOutput / InteractiveEvaluator */ }
    void Constraints()  { /* CONS(...) */ }
    // For subtasked problems, define Subtask1()..Subtask25() instead of / in addition to Constraints()
    // For ICPC-style:  MultipleTestCasesConfig() + MultipleTestCasesConstraints()
};

class TestSpec : public BaseTestSpec<ProblemSpec> {
protected:
    void SampleTestCase1() { Input({"..."}); Output({"..."}); }
    void BeforeTestCase()  { /* clear vectors */ }
    void AfterTestCase()   { /* optional: transform internal rep → IO vars */ }
    void TestCases()       { /* CASE(...) — no subtasks */ }
    // OR: void TestGroup1()...TestGroup25() — with subtasks
};
```

---

## I/O variable types

Allowed: **scalars** (`int`, `long long`, `char`, `float`, `double`,
`std::string`), **vectors** `vector<T>` of scalar `T`, **matrices**
`vector<vector<T>>` of scalar `T`. Nothing else (no raw arrays, no
`char*`, no structs).

Vectors and matrices are **0-indexed**. No way to force 1-indexing.

---

## I/O segments (used inside `InputFormat()` / `OutputFormat()`)

### Tokenized (space-separated)

| Macro | Meaning |
|---|---|
| `LINE(v1, v2, ...)` | One line, space-separated values. Mix scalars and vectors. |
| `LINE(V % SIZE(N))` | A vector printed on one line, length `N`. |
| `LINE(N, A % SIZE(N), C % SIZE(2))` | Mixed scalars + sized vectors. |
| `LINE(X, A)` | Trailing unsized vector (only legal as the last arg). |
| `LINES(V) % SIZE(N)` | `N` lines, one element per line. |
| `LINES(U, V) % SIZE(N)` | `N` lines, each holds one element from each vector. |
| `LINES(op, data) % SIZE(N)` | Jagged: `data` is `vector<vector<T>>`, last arg may be jagged. |
| `GRID(G) % SIZE(R, C)` | Matrix `G` as `R×C`. `char` matrices have no spaces between cells; others are space-separated. |

> `% SIZE(n)` can be **omitted on the last segment** of `LINES` /
> `LINE(... A)` / `RAW_LINES`.

> In tokenized segments, **strings cannot contain whitespace**. Use raw
> segments below if they do.

### Raw

| Macro | Meaning |
|---|---|
| `EMPTY_LINE()` | A blank line. |
| `RAW_LINE(S)` | One line, literal string `S`. |
| `RAW_LINES(V) % SIZE(N)` | `N` lines of literal strings (`V` is `vector<string>`). |

### Constants in I/O format

**Not supported.** If the problem says "the first line is always `BEGIN`",
declare a `string` input variable and initialize it to `"BEGIN"` in every
`CASE`.

### Multiple output formats

Up to 5 simultaneous: `OutputFormat1()` … `OutputFormat5()`. The output
is valid if it matches **any** of them. Common case: one for "impossible"
(single `-1`), one for the structured answer.

### `BeforeOutputFormat()`

Runs right before output is parsed. Useful when output size depends on
input — e.g. set a helper `update_count` then
`LINES(answers) % SIZE(update_count)`.

---

## Constraints

```cpp
void Constraints() {
    CONS(<bool predicate of input vars>);
    CONS(<another>);
}
```

Constraints **must depend only on input variables**. Per-element rules
go through a private predicate method or a built-in validator.

### Built-in validators

```cpp
// Scalars
CONS(valueOf(N).isBetween(1, MAXN));

// Vector elements
CONS(eachElementOf(A).isBetween(1, MAXV));
CONS(eachElementOf(A).satisfies([](int x){ return x % 2 == 0; }));

// Matrix elements
CONS(eachElementOf(G).isBetween(0, 9));

// String characters
CONS(eachCharacterOf(S).isBetween('a', 'z'));

// Whole-vector predicates
CONS(elementsOf(A).areAscending());      // strictly
CONS(elementsOf(A).areDescending());     // strictly
CONS(elementsOf(A).areNonAscending());
CONS(elementsOf(A).areNonDescending());
CONS(elementsOf(A).areUnique());
```

### Custom predicate

```cpp
void Constraints() { CONS(graphIsTree()); }
private:
  bool graphIsTree() { /* ... */ }
```

---

## Subtasks

```cpp
void Constraints() {                  // OPTIONAL — global, apply to every subtask
    CONS(1 <= N && N <= MAXN);
}
void Subtask1() { Points(20); CONS(N <= 10); }
void Subtask2() { Points(30); CONS(N <= 1000); }
void Subtask3() { Points(50); /* no extra */ }
```

Limit: `Subtask1()` … `Subtask25()`.

**Venn-diagram rule.** Once subtasks exist, you use `TestGroupX()` (see
below), and tcframe enforces: a test case **must** be assigned to a
subtask iff its inputs satisfy that subtask's constraints. So design
one test group per closed region of the Venn diagram. Cases in
`TestGroupK` assigned `Subtasks({2,3})` must satisfy Subtask 2 + 3 and
**violate** Subtask 1.

---

## Test cases

### Without subtasks

```cpp
void TestCases() {
    CASE(N = 1, A = {1});
    CASE(N = 100, randomArray());   // helper method that fills A
}
```

### With subtasks → use test groups

```cpp
void TestGroup1() {
    Subtasks({1, 2, 3});            // MUST be first statement
    CASE(...);
}
void TestGroup2() {
    Subtasks({2, 3});
    CASE(...);
}
```

Limit: `TestGroup1()` … `TestGroup25()`.

### `CASE(...)` syntax

Comma-separated **assignments / expressions**, evaluated left-to-right:

```cpp
CASE(N = 100, M = 50);
CASE(N = 100, buildTree());        // call a helper that mutates input vars
CASE(V = {1,2,3}, M = {{1,2},{3,4}});  // brace-init vector / matrix
```

*(You may encounter legacy specs that use semicolons inside `CASE`;
the documented form is commas — stick with commas.)*

Loops are legal but discouraged:
```cpp
for (int i = 1; i <= 10; i++) CASE(N = i);
```

### Sample test cases

```cpp
void SampleTestCase1() {
    Subtasks({2, 3});               // ONLY if subtasks exist
    Input({"3 4", "1 2 3"});        // literal lines
    Output({"42"});                 // optional but recommended
}
```

Sample inputs are **parsed against `InputFormat()`** — mismatches
surface as `Expected: <space> after variable ...`. If `NoOutput()` is
on, omit `Output()`.

Limit: `SampleTestCase1()` … `SampleTestCase25()`.

### Lifecycle (per official test case)

```
BeforeTestCase()  →  CASE(...) body  →  AfterTestCase()  →  print using InputFormat()
                                                          →  run solution
                                                          →  BeforeOutputFormat()
                                                          →  parse using OutputFormat()
```

Vectors are **not auto-cleared** between cases. Always:
```cpp
void BeforeTestCase() { A.clear(); U.clear(); V.clear(); }
```

`AfterTestCase()` is useful when your generator's natural representation
differs from the I/O shape — e.g. you build a `parent[]` array but the
problem reads edges, so you transform `parent[]` into `(U[], V[])` here.

---

## Random number generator (`rnd`)

**Always use `rnd` for randomness inside `TestSpec`. Do not use
`std::mt19937`, `std::random_device`, or `std::rand`** — they ignore
`./runner --seed=N` so test cases drift between reruns.

```cpp
int  rnd.nextInt(lo, hi);          // inclusive
int  rnd.nextInt(maxEx);           // [0, maxEx)
long long rnd.nextLongLong(lo, hi);
long long rnd.nextLongLong(maxEx);
double rnd.nextDouble(lo, hi);
double rnd.nextDouble(max);        // [0, max]
void rnd.shuffle(begin, end);      // use THIS, not std::random_shuffle
```

Default seed is 0; override with `./runner --seed=N`.

---

## Style config

```cpp
void StyleConfig() {
    CustomScorer();         // multiple-valid-output / partial credit
    NoOutput();             // don't generate .out files

    InteractiveEvaluator(); // problem uses a communicator
                            // REQUIRES NoOutput()
}
```

`SampleTestCaseX()` with `NoOutput()` must **not** call `Output()`.

---

## Multiple test cases per file (ICPC style)

```cpp
class ProblemSpec : ... {
protected:
    int T;                         // declared as input var (NOT in InputFormat!)
    int A, B; int sum;

    void InputFormat()  { LINE(A, B); }       // ONE case, no T
    void OutputFormat() { LINE(sum); }

    void MultipleTestCasesConfig() {
        Counter(T);
        OutputPrefix("Case #%d: ");           // optional; "%d" = case index
    }
    void MultipleTestCasesConstraints() {
        CONS(1 <= T && T <= 20);
    }
    void Constraints() { /* per-case */ }
};
```

Solution must read `T` itself then loop. Files are combined into
`<slug>.in` (and `<slug>_<group>.in` for test groups).

---

## Grading config + local grading

```cpp
void GradingConfig() {
    TimeLimit(2);          // seconds
    MemoryLimit(256);      // MB
}
```

Generate, then grade an alternate solution:
```
./runner
./runner grade --solution=./solution_alt
./runner grade --solution=./solution_alt --time-limit=3 --memory-limit=512
./runner grade --solution=./solution_alt --brief    # one-line verdict (for scripts)
```

Per-case verdicts: `Accepted`, `OK [points]`, `Wrong Answer`,
`Runtime Error`, `Time Limit Exceeded`, `Internal Error`. Overall
verdict = worst across all cases.

> Memory limits are silently ignored on macOS (tcframe uses `ulimit`,
> which can't restrict memory there). On Linux/WSL it works.

---

## Helper programs

### Scorer (`StyleConfig() { CustomScorer(); }`)

```
argv[1] = test case input file
argv[2] = test case official output file (may be empty if NoOutput)
argv[3] = contestant's output file
stdout  = verdict
```

Verdict format on stdout:
```
AC                          // full points
WA                          // zero
OK                          // partial points — followed by a value line:
71                          //   absolute score (can be float)
OK
50%                         //   OR percentage of full points
```

The scorer must be compiled to `./scorer` (or pass `--scorer=<cmd>` to
the runner). It's used during sample-output validation in generation
*and* during `./runner grade`.

### Communicator (`StyleConfig() { InteractiveEvaluator(); NoOutput(); }`)

```
argv[1] = test case input file
stdin   = contestant's output
stdout  = data sent to contestant (FLUSH EVERY WRITE)
stderr  = verdict (same format as scorer, but to stderr)
```

The communicator is **only used during `./runner grade`**, never during
generation. Compile to `./communicator` or pass `--communicator=<cmd>`.

---

## Runner CLI

```
./runner [--output=tc] [--solution=./solution] [--scorer=./scorer] [--seed=0]

./runner grade [--solution=./solution] [--scorer=./scorer]
               [--communicator=./communicator]
               [--time-limit=N] [--memory-limit=N]
               [--no-time-limit] [--no-memory-limit]
               [--brief]
```

---

## Common error messages

| Message in `./runner` output | Cause |
|---|---|
| `Does not satisfy constraints, on: <CONS>` | Generator produced a case that violates the listed constraint (or the subtask it's assigned to). Fix the generator, or move the case to a group whose subtasks accept it. |
| `Expected: <token>` / `Expected: <space> after variable X` | `Input({...})` literal doesn't match `InputFormat()`. Check whitespace and line breaks. |
| `does not conform to output format` | The **solution's** output doesn't parse against `OutputFormat()`. Usually means a wrong solution, OR the format is wrong — flag to the user before changing format. |
| Compile failure under `tcframe build` | `spec.cpp` syntax. Read the g++ error; common causes: forgot `% SIZE(...)`, mismatched braces in `CASE`, used unsupported type as I/O variable. |

---

## File naming

- Slug = directory name (with optional `prefix_` stripped). Must match
  `[A-Za-z0-9-]+`.
- Output files: `<slug>_<n>.in`, `<slug>_<n>.out` (no subtasks),
  `<slug>_<group>_<n>.in/.out` (with test groups),
  `<slug>_sample_<n>.in/.out` (samples).
- Multi-case ICPC: `<slug>.in`, `<slug>_<group>.in`, `<slug>_sample.in`.
