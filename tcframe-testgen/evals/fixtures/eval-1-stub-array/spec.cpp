#include <bits/stdc++.h>
#include <tcframe/spec.hpp>

using namespace std;
using namespace tcframe;

const int MAXN = 100000;
const int MAXV = 1000000000;

class ProblemSpec : public BaseProblemSpec {
protected:
    int N;
    vector<int> A;

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
        // TODO: add test cases here
    }
};
