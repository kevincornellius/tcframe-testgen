// Boilerplate: problem with subtasks + test groups.
// Test files emitted as <slug>_<group>_<case>.in/out.
#include <bits/stdc++.h>
#include <tcframe/spec.hpp>

using namespace std;
using namespace tcframe;

const int MAXN_SMALL = 1000;
const int MAXN = 200000;
const int MAXV = 1000000000;

class ProblemSpec : public BaseProblemSpec {
protected:
    int N, Q;
    vector<int> A;

    long long result;

    void InputFormat() {
        LINE(N, Q);
        LINE(A % SIZE(N));
    }

    void OutputFormat() {
        LINE(result);
    }

    void GradingConfig() {
        TimeLimit(2);
        MemoryLimit(256);
    }

    // Global constraints (apply to every subtask).
    void Constraints() {
        CONS(1 <= N && N <= MAXN);
        CONS(1 <= Q && Q <= MAXN);
        CONS(eachElementOf(A).isBetween(1, MAXV));
    }

    void Subtask1() {
        Points(20);
        CONS(N <= 10);
        CONS(Q <= 10);
    }

    void Subtask2() {
        Points(30);
        CONS(N <= MAXN_SMALL);
        CONS(Q <= MAXN_SMALL);
    }

    void Subtask3() {
        Points(50);
        // No extra constraints — full constraints.
    }
};

class TestSpec : public BaseTestSpec<ProblemSpec> {
protected:
    void SampleTestCase1() {
        Subtasks({1, 2, 3});
        Input({
            "3 1",
            "1 2 3"
        });
        Output({
            "6"
        });
    }

    void BeforeTestCase() {
        A.clear();
    }

    // Test groups should mirror the Venn diagram of subtasks.
    // Each closed region in the Venn diagram = one TestGroup.
    // Below: 3 nested subtasks (1 ⊂ 2 ⊂ 3) ⇒ 3 closed regions.

    void TestGroup1() {
        Subtasks({1, 2, 3});
        // Tiny: must be valid for ALL subtasks.
        CASE(N = 1, Q = 1, A = {1});
        CASE(N = 10, Q = 10, randomA(1, MAXV));
        CASE(N = 10, Q = 10, A = vector<int>(10, MAXV));
    }

    void TestGroup2() {
        Subtasks({2, 3});
        // Medium: violates Subtask1 (N > 10) but fits Subtask2/3.
        CASE(N = 11, Q = 11, randomA(1, MAXV));
        CASE(N = MAXN_SMALL, Q = MAXN_SMALL, randomA(1, MAXV));
        CASE(N = MAXN_SMALL, Q = 1, randomA(1, MAXV));
    }

    void TestGroup3() {
        Subtasks({3});
        // Large: only fits Subtask3.
        CASE(N = MAXN, Q = MAXN, randomA(1, MAXV));
        CASE(N = MAXN, Q = MAXN, randomA(1, MAXV));
        CASE(N = MAXN, Q = MAXN, fillSame(MAXV));
        CASE(N = MAXN, Q = 1, fillSame(1));
    }

private:
    void randomA(int lo, int hi) {
        for (int i = 0; i < N; i++) A.push_back(rnd.nextInt(lo, hi));
    }
    void fillSame(int v) { for (int i = 0; i < N; i++) A.push_back(v); }
};
