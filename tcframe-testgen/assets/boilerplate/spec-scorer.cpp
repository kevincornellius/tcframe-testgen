// Boilerplate: problem with custom scorer (multiple valid outputs / partial credit).
// Compile scorer.cpp into ./scorer alongside spec. Pass --scorer to runner if name differs.
#include <bits/stdc++.h>
#include <tcframe/spec.hpp>

using namespace std;
using namespace tcframe;

class ProblemSpec : public BaseProblemSpec {
protected:
    int N;
    vector<int> A;

    void InputFormat() {
        LINE(N);
        LINE(A % SIZE(N));
    }

    void GradingConfig() {
        TimeLimit(2);
        MemoryLimit(256);
    }

    void StyleConfig() {
        CustomScorer();
        NoOutput();   // omit if you still want .out files (then sample Output() is checked via scorer)
    }

    void Constraints() {
        CONS(1 <= N && N <= 100000);
        CONS(eachElementOf(A).isBetween(1, 1000000000));
    }
};

class TestSpec : public BaseTestSpec<ProblemSpec> {
protected:
    void SampleTestCase1() {
        // With NoOutput(), do NOT call Output() — only Input().
        Input({
            "3",
            "1 2 3"
        });
    }

    void BeforeTestCase() { A.clear(); }

    void TestCases() {
        CASE(N = 1, A = {1});
        CASE(N = 5, A = {3, 1, 4, 1, 5});
        CASE(N = 100000, randomA(1, 1000000000));
    }

private:
    void randomA(int lo, int hi) {
        for (int i = 0; i < N; i++) A.push_back(rnd.nextInt(lo, hi));
    }
};
