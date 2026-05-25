// Boilerplate: plain batch problem, no subtasks.
// Slug = directory name. Compile with `tcframe build`, run with `./runner`.
#include <bits/stdc++.h>
#include <tcframe/spec.hpp>

using namespace std;
using namespace tcframe;

const int MAXN = 100000;
const int MAXV = 1000000000;

class ProblemSpec : public BaseProblemSpec {
protected:
    // ---- Input variables ----
    int N;
    vector<int> A;

    // ---- Output variables ----
    long long result;

    void InputFormat() {
        LINE(N);
        LINE(A % SIZE(N));
    }

    void OutputFormat() {
        LINE(result);
    }

    void GradingConfig() {
        TimeLimit(2);
        MemoryLimit(256);
    }

    void Constraints() {
        CONS(1 <= N && N <= MAXN);
        CONS(eachElementOf(A).isBetween(1, MAXV));
    }
};

class TestSpec : public BaseTestSpec<ProblemSpec> {
protected:
    void SampleTestCase1() {
        Input({
            "3",
            "1 2 3"
        });
        Output({
            "6"
        });
    }

    void BeforeTestCase() {
        A.clear();
    }

    void TestCases() {
        // ---- Edge cases: smallest / boundaries ----
        CASE(N = 1, A = {1});
        CASE(N = 1, A = {MAXV});
        CASE(N = 2, A = {1, 1});

        // ---- Small handmade ----
        CASE(N = 5, A = {3, 1, 4, 1, 5});

        // ---- Uniform / pathological ----
        CASE(N = MAXN, fillSame(1));
        CASE(N = MAXN, fillSame(MAXV));

        // ---- Sorted / reverse-sorted ----
        CASE(N = MAXN, fillRange(1, MAXN));
        CASE(N = MAXN, fillRange(MAXN, 1));

        // ---- Random small / medium / max ----
        CASE(N = 10, randomA(1, 10));
        CASE(N = 1000, randomA(1, 1000));
        CASE(N = MAXN, randomA(1, MAXV));
        CASE(N = MAXN, randomA(1, MAXV));
        CASE(N = MAXN, randomA(1, MAXV));
    }

private:
    void randomA(int lo, int hi) {
        for (int i = 0; i < N; i++) A.push_back(rnd.nextInt(lo, hi));
    }
    void fillSame(int v) {
        for (int i = 0; i < N; i++) A.push_back(v);
    }
    void fillRange(int from, int to) {
        int step = from <= to ? 1 : -1;
        for (int i = 0, v = from; i < N; i++, v += step) A.push_back(v);
    }
};
