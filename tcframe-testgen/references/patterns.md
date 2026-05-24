# tcframe data-structure cookbook & edge-case checklist

Snippets are partial; drop them into `TestSpec`'s `private:` section and call
from inside `CASE(...)`. All input variables must be declared in `ProblemSpec`.

> Remember: `CASE(...)` arguments are comma-separated **assignments**. To run a
> helper that mutates input variables, write `CASE(N = 10, buildTree())`. Use
> `BeforeTestCase()` to clear vectors between cases.

---

## Random number generator (`rnd`) — ALWAYS use this, never `std::mt19937` / `std::rand`

tcframe ships a random generator `rnd` available inside `TestSpec`.
**Use it for every random value.** Do not introduce
`std::mt19937`, `std::random_device`, `std::rand`, or any other RNG.
Reasons:

- `./runner --seed=N` reseeds `rnd` reproducibly. A stray `mt19937 rng(42)`
  ignores that flag, so test regeneration drifts when others rerun the
  package — defeating the point of the framework.
- Other tcframe authors reading the spec expect `rnd`; a bespoke RNG
  makes the spec harder to audit.
- `rnd.shuffle` is the canonical replacement for `std::random_shuffle`
  (which is deprecated/removed in C++17).

API:

- `rnd.nextInt(lo, hi)` — inclusive both ends.
- `rnd.nextInt(maxEx)` — `[0, maxEx)`.
- `rnd.nextLongLong(lo, hi)`, `rnd.nextDouble(lo, hi)`.
- `rnd.shuffle(begin, end)`.

The seed defaults to 0; override with `./runner --seed=N`.

---

## Vectors / arrays

```cpp
// Random fill (declare A as vector<int> in ProblemSpec)
void randomA(int lo, int hi) {
    for (int i = 0; i < N; i++) A.push_back(rnd.nextInt(lo, hi));
}

// All distinct values in [1..M]
void distinctA(int M) {
    vector<int> pool(M);
    iota(pool.begin(), pool.end(), 1);
    rnd.shuffle(pool.begin(), pool.end());
    for (int i = 0; i < N; i++) A.push_back(pool[i]);
}

// Strictly increasing
void increasingA(int lo, int hi) {
    randomA(lo, hi);
    sort(A.begin(), A.end());
}
```

**Array edge cases to always cover:**
- `N = 1`, `N = 2`
- All identical (min value, max value)
- Strictly increasing / decreasing
- Already-sorted vs reverse-sorted
- All distinct vs all duplicates
- Max `N` with max values (stress + overflow)
- Boundary values (`1`, `MAXV`) at start, middle, end

---

## Trees (input as parent array or edge list)

```cpp
// In ProblemSpec input format usually: LINES(U, V) % SIZE(N - 1);
// Declare in TestSpec:  vector<int> parent;  (private helper representation)

void randomTree() {
    // parent[i] in [0..i-1], 1-indexed nodes
    U.clear(); V.clear();
    for (int i = 2; i <= N; i++) {
        int p = rnd.nextInt(1, i - 1);
        U.push_back(p);
        V.push_back(i);
    }
}

void linearTree() {       // path: 1-2-3-...-N (worst-case depth)
    U.clear(); V.clear();
    for (int i = 2; i <= N; i++) { U.push_back(i - 1); V.push_back(i); }
}

void starTree() {         // root 1 connected to all others
    U.clear(); V.clear();
    for (int i = 2; i <= N; i++) { U.push_back(1); V.push_back(i); }
}

void caterpillarTree(int spineLen) {
    U.clear(); V.clear();
    for (int i = 2; i <= spineLen; i++) { U.push_back(i - 1); V.push_back(i); }
    for (int i = spineLen + 1; i <= N; i++) {
        U.push_back(rnd.nextInt(1, spineLen)); V.push_back(i);
    }
}
```

**Tree edge cases:** path, star, balanced binary, caterpillar, broom; randomize node labels via `rnd.shuffle` of a permutation to avoid solutions exploiting node ids.

---

## General graphs

```cpp
// Connected graph: spanning tree + random extra edges
void connectedGraph() {
    U.clear(); V.clear();
    for (int i = 2; i <= N; i++) {
        U.push_back(rnd.nextInt(1, i - 1));
        V.push_back(i);
    }
    while ((int)U.size() < M) {
        int u = rnd.nextInt(1, N), v = rnd.nextInt(1, N - 1);
        if (v >= u) v++;
        U.push_back(u); V.push_back(v);
    }
}
```

**Graph edge cases:** disconnected, single vertex, complete (M = N(N-1)/2), self-loops if allowed, multi-edges if allowed, bipartite, dense vs sparse, two SCCs joined by one bridge.

---

## Strings

```cpp
// Random over a small alphabet (declare string S in ProblemSpec)
void randomS(int len, char lo = 'a', char hi = 'z') {
    S.clear();
    for (int i = 0; i < len; i++) S.push_back((char)rnd.nextInt(lo, hi));
}
```

**String edge cases:** single char, all same char, palindrome, anti-palindrome, periodic (e.g. `abcabcabc`), only-extremes alphabet (only `a` and `z`), max length.

---

## Grids

```cpp
// MATRIX G of chars (declare in ProblemSpec): GRID(G) % SIZE(R, C)
void randomGrid(string alphabet) {
    G.assign(R, vector<char>(C));
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            G[i][j] = alphabet[rnd.nextInt((int)alphabet.size())];
}
```

**Grid edge cases:** 1×1, 1×C, R×1, all same char, checkerboard, all walls / no walls.

---

## Numeric / overflow

- Always include a case at `MAXV` to catch missing `long long`.
- Include the case that maximizes the **sum** or **product** of inputs.
- For modular arithmetic, throw in inputs that hit modulus boundaries.

---

## Constraints — best-practice patterns

- Prefer built-in validators where possible:
  ```cpp
  CONS(valueOf(N).isBetween(1, MAXN));
  CONS(eachElementOf(A).isBetween(1, MAXV));
  CONS(elementsOf(A).areUnique());
  CONS(elementsOf(A).areNonDescending());
  ```
- For complex predicates, write a `private bool` method in `ProblemSpec`
  and `CONS(graphIsTree())`.
- Constraints **must depend only on input variables** — never output vars.
- For multi-case ICPC style, put per-case constraints in `Constraints()` and
  per-file constraints in `MultipleTestCasesConstraints()`.

---

## Test groups & subtasks — Venn diagram rule

A test case **must** be assigned to a subtask iff it satisfies that subtask's
constraints. Draw a Venn diagram of the subtasks; **one TestGroup per closed
region.** Generators in `TestGroupX` must produce cases inside that region
only.

Common pitfall: a `TestGroup` assigned to `Subtasks({2, 3})` whose generator
happens to produce a case that also fits Subtask 1 — tcframe will fail
generation. Force the case out of Subtask 1 explicitly (e.g. ensure `N > 10`).

---

## Lifecycle reminder

```
BeforeTestCase()  →  CASE(...) body executes  →  AfterTestCase()  →  format & print
```

- Clear all vector input variables in `BeforeTestCase()`.
- Use `AfterTestCase()` if your generator's natural representation differs from
  the I/O format (e.g. you build a parent[] but the input format is edges).
- `BeforeOutputFormat()` runs before the produced output is parsed — useful
  when the output shape depends on the input (e.g. number of queries).
