// Boilerplate: interactive problem (uses a communicator program).
// The communicator is ONLY used during local grading, not during generation.
// Sample test cases should NOT contain Output() — only Input() (the "secret" data).
#include <bits/stdc++.h>
#include <tcframe/spec.hpp>

using namespace std;
using namespace tcframe;

const int MAXN = 1000000;

class ProblemSpec : public BaseProblemSpec {
protected:
    int N;   // the hidden number the solution must guess

    void InputFormat() {
        LINE(N);
    }

    void GradingConfig() {
        TimeLimit(1);
        MemoryLimit(64);
    }

    void StyleConfig() {
        InteractiveEvaluator();
        NoOutput();    // REQUIRED with InteractiveEvaluator
    }

    void Constraints() {
        CONS(1 <= N && N <= MAXN);
    }
};

class TestSpec : public BaseTestSpec<ProblemSpec> {
protected:
    void SampleTestCase1() {
        Input({"7"});   // no Output()
    }

    void TestCases() {
        CASE(N = 1);
        CASE(N = MAXN);
        CASE(N = MAXN / 2);
        for (int i = 0; i < 10; i++) CASE(N = rnd.nextInt(1, MAXN));
    }
};
