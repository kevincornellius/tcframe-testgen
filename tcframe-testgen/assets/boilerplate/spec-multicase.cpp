// Boilerplate: ICPC-style multiple test cases per file.
// Test files combined into <slug>.in / <slug>_sample.in.
// IMPORTANT: solution must read T itself then loop. InputFormat() must NOT contain T.
#include <bits/stdc++.h>
#include <tcframe/spec.hpp>

using namespace std;
using namespace tcframe;

class ProblemSpec : public BaseProblemSpec {
protected:
    int T;          // test case counter
    int A, B;
    int sum;

    void InputFormat() {
        LINE(A, B);   // single test case shape; T is prepended automatically
    }

    void OutputFormat() {
        LINE(sum);
    }

    void GradingConfig() {
        TimeLimit(1);
        MemoryLimit(64);
    }

    void MultipleTestCasesConfig() {
        Counter(T);
        OutputPrefix("Case #%d: ");   // optional
    }

    void MultipleTestCasesConstraints() {
        CONS(1 <= T && T <= 20);
    }

    void Constraints() {
        CONS(1 <= A && A <= 1000);
        CONS(1 <= B && B <= 1000);
    }
};

class TestSpec : public BaseTestSpec<ProblemSpec> {
protected:
    void SampleTestCase1() {
        Input({
            "4 6"
        });
        Output({
            "10"
        });
    }

    void TestCases() {
        CASE(A = 1, B = 1);
        CASE(A = 1000, B = 1000);
        CASE(A = rnd.nextInt(1, 1000), B = rnd.nextInt(1, 1000));
    }
};
