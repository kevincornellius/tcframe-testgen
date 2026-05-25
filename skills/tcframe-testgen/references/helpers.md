# tcframe private helper snippets

Drop these into the `private:` section of `TestSpec`. All use `rnd` — never `std::mt19937`.

Input variables (`N`, `M`, `A`, `U`, `V`, `S`, `G`, `R`, `C`, etc.) must be declared in `ProblemSpec`.

---

## Arrays

```cpp
// Random array of length n in [lo, hi]
void randomArray(int n, int lo, int hi) {
    for (int i = 0; i < n; i++) A.push_back(rnd.nextInt(lo, hi));
}

// All elements equal to val
void constArray(int n, int val) {
    A.assign(n, val);
}

// Strictly increasing: sorted random values in [lo, hi]
void increasingArray(int n, int lo, int hi) {
    randomArray(n, lo, hi);
    sort(A.begin(), A.end());
}

// Strictly decreasing
void decreasingArray(int n, int lo, int hi) {
    randomArray(n, lo, hi);
    sort(A.rbegin(), A.rend());
}

// All distinct values from [1..M], shuffled
void distinctArray(int n, int M) {
    vector<int> pool(M);
    iota(pool.begin(), pool.end(), 1);
    rnd.shuffle(pool.begin(), pool.end());
    for (int i = 0; i < n; i++) A.push_back(pool[i]);
}

// Alternating lo/hi
void alternatingArray(int n, int lo, int hi) {
    for (int i = 0; i < n; i++) A.push_back(i % 2 == 0 ? lo : hi);
}
```

Usage:
```cpp
CASE(N = 1,     constArray(1, MAXV));
CASE(N = MAXN,  randomArray(MAXN, 1, MAXV));
CASE(N = MAXN,  increasingArray(MAXN, 1, MAXV));
CASE(N = 10,    alternatingArray(10, 1, MAXV));
```

---

## Trees (edge list: `U[]`, `V[]`)

```cpp
// Random tree: each node i connects to a random ancestor in [1..i-1]
void randomTree() {
    for (int i = 2; i <= N; i++) {
        U.push_back(rnd.nextInt(1, i - 1));
        V.push_back(i);
    }
}

// Path: 1-2-3-...-N (worst-case DFS depth)
void linearTree() {
    for (int i = 2; i <= N; i++) { U.push_back(i - 1); V.push_back(i); }
}

// Star: root 1 connected to all others
void starTree() {
    for (int i = 2; i <= N; i++) { U.push_back(1); V.push_back(i); }
}

// Balanced binary tree (0-indexed children: 2i+1, 2i+2)
void binaryTree() {
    for (int i = 1; i < N; i++) { U.push_back(i / 2 + 1); V.push_back(i + 1); }
}

// Caterpillar: spine of length spineLen, remaining nodes hang off spine
void caterpillarTree(int spineLen) {
    for (int i = 2; i <= spineLen; i++) { U.push_back(i - 1); V.push_back(i); }
    for (int i = spineLen + 1; i <= N; i++) {
        U.push_back(rnd.nextInt(1, spineLen));
        V.push_back(i);
    }
}

// Shuffle node labels to avoid solutions exploiting sorted IDs
void shuffleTreeLabels() {
    vector<int> perm(N + 1);
    iota(perm.begin(), perm.end(), 0);
    rnd.shuffle(perm.begin() + 1, perm.end());
    for (auto& u : U) u = perm[u];
    for (auto& v : V) v = perm[v];
}
```

Usage:
```cpp
CASE(N = 2,    linearTree());
CASE(N = MAXN, linearTree());                    // worst-case depth
CASE(N = MAXN, starTree());                      // max degree at root
CASE(N = MAXN, caterpillarTree(MAXN / 2));
CASE(N = MAXN, randomTree(), shuffleTreeLabels());
```

---

## General graphs (edge list: `U[]`, `V[]`, with `M` edges)

```cpp
// Connected graph: spanning tree + (M - N + 1) random extra edges, no multi-edges
void connectedGraph() {
    set<pair<int,int>> used;
    auto addEdge = [&](int u, int v) {
        if (u > v) swap(u, v);
        used.insert({u, v});
        U.push_back(u); V.push_back(v);
    };
    // spanning tree (guarantees connectivity)
    for (int i = 2; i <= N; i++)
        addEdge(rnd.nextInt(1, i - 1), i);
    // extra edges — retry until a fresh pair is found
    int attempts = 0;
    while ((int)U.size() < M && attempts < M * 10) {
        int u = rnd.nextInt(1, N), v = rnd.nextInt(1, N - 1);
        if (v >= u) v++;
        int lo = min(u, v), hi = max(u, v);
        if (!used.count({lo, hi})) addEdge(lo, hi);
        attempts++;
    }
}

// DAG: edges only go from lower to higher index, no multi-edges
void randomDAG() {
    set<pair<int,int>> used;
    int attempts = 0;
    while ((int)U.size() < M && attempts < M * 10) {
        int u = rnd.nextInt(1, N - 1);
        int v = rnd.nextInt(u + 1, N);
        if (!used.count({u, v})) {
            used.insert({u, v});
            U.push_back(u); V.push_back(v);
        }
        attempts++;
    }
}
```

Usage:
```cpp
CASE(N = 1, M = 0);                                // single vertex
CASE(N = MAXN, M = N - 1, connectedGraph());       // tree (sparse)
CASE(N = MAXN, M = MAXM, connectedGraph());        // dense
```

> These helpers never produce self-loops or multi-edges. If the problem explicitly allows them, handle that separately.


---

## Strings

```cpp
// Random string of length len over [lo..hi]
void randomString(int len, char lo = 'a', char hi = 'z') {
    S.clear();
    for (int i = 0; i < len; i++) S.push_back((char)rnd.nextInt(lo, hi));
}

// All same character
void constString(int len, char c) { S.assign(len, c); }

// Palindrome of length len
void palindromeString(int len, char lo = 'a', char hi = 'z') {
    S.resize(len);
    for (int i = 0; i <= (len - 1) / 2; i++) {
        char c = (char)rnd.nextInt(lo, hi);
        S[i] = S[len - 1 - i] = c;
    }
}

// Period-p string, repeated to fill len
void periodicString(int len, int p, char lo = 'a', char hi = 'z') {
    S.resize(p);
    for (int i = 0; i < p; i++) S[i] = (char)rnd.nextInt(lo, hi);
    S.resize(len);
    for (int i = p; i < len; i++) S[i] = S[i % p];
}
```

Usage:
```cpp
CASE(N = 1,    randomString(1));
CASE(N = MAXN, constString(MAXN, 'a'));
CASE(N = MAXN, palindromeString(MAXN));
CASE(N = MAXN, periodicString(MAXN, 3));
CASE(N = MAXN, randomString(MAXN, 'a', 'b'));   // binary alphabet
CASE(N = MAXN, randomString(MAXN));
```

---

## Grids

```cpp
// Random char grid R×C over alphabet
void randomGrid(const string& alphabet) {
    G.assign(R, vector<char>(C));
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            G[i][j] = alphabet[rnd.nextInt((int)alphabet.size())];
}

// Checkerboard
void checkerGrid(char a = '.', char b = '#') {
    G.assign(R, vector<char>(C));
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            G[i][j] = (i + j) % 2 == 0 ? a : b;
}

// All same cell
void constGrid(char c) { G.assign(R, vector<char>(C, c)); }
```

Usage:
```cpp
CASE(R = 1, C = 1,    constGrid('#'));
CASE(R = 1, C = MAXC, randomGrid("#."));
CASE(R = MAXR, C = MAXC, checkerGrid());
CASE(R = MAXR, C = MAXC, randomGrid("#."));
```

---

## Queries (offline: `L[]`, `R[]`)

```cpp
// Random queries [l, r] with 1 <= l <= r <= N
void randomQueries(int q) {
    for (int i = 0; i < q; i++) {
        int l = rnd.nextInt(1, N), r = rnd.nextInt(1, N);
        if (l > r) swap(l, r);
        L.push_back(l); R.push_back(r);
    }
}

// All point queries (l == r)
void pointQueries(int q) {
    for (int i = 0; i < q; i++) {
        int l = rnd.nextInt(1, N);
        L.push_back(l); R.push_back(l);
    }
}

// All full-range queries
void fullQueries(int q) {
    for (int i = 0; i < q; i++) { L.push_back(1); R.push_back(N); }
}
```

Usage:
```cpp
CASE(N = MAXN, Q = MAXQ, randomQueries(MAXQ));
CASE(N = MAXN, Q = MAXQ, pointQueries(MAXQ));
CASE(N = MAXN, Q = 1,    fullQueries(1));
```

---

## Permutations

```cpp
// Random permutation of [1..N]
void randomPerm() {
    A.resize(N);
    iota(A.begin(), A.end(), 1);
    rnd.shuffle(A.begin(), A.end());
}

// Identity permutation
void identityPerm() { A.resize(N); iota(A.begin(), A.end(), 1); }

// Reverse permutation
void reversePerm() { A.resize(N); iota(A.rbegin(), A.rend(), 1); }
```

---

## BeforeTestCase template (always clear vectors)

```cpp
void BeforeTestCase() {
    A.clear();
    U.clear(); V.clear();
    L.clear(); R.clear();
    S.clear();
    G.clear();
}
```

Always clear every vector input variable here. Missing a clear causes the previous case's data to bleed into the next case silently.
