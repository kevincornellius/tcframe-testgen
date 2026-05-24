#include <bits/stdc++.h>
#include <tcframe/spec.hpp>

using namespace std;
using namespace tcframe;

const int MAXN = 1000;
const int MAXV = 1000;

class ProblemSpec : public BaseProblemSpec {
protected:
    int N;
    vector<int> A;
    int maxVal;

    void InputFormat() {
        LINE(N);
        LINE(A % SIZE(N));
    }

    void OutputFormat() {
        LINE(maxVal);
    }

    void GradingConfig() {
        TimeLimit(1);
        MemoryLimit(64);
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
            "1 5 2"
        });
        Output({
            "5"
        });
    }

    // BUG: BeforeTestCase missing — vectors are not cleared between cases.

    void TestCases() {
        CASE(N = 1, A = {1});
        CASE(N = 3, randomA(1, 10));
        CASE(N = MAXN, randomA(1, MAXV + 5));   // BUG: hi out of range
        CASE(N = 5, randomA(1, MAXV));
    }

private:
    void randomA(int lo, int hi) {
        for (int i = 0; i < N; i++) A.push_back(rnd.nextInt(lo, hi));
    }
};
