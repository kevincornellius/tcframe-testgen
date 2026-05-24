#include <bits/stdc++.h>
#include <tcframe/spec.hpp>

using namespace std;
using namespace tcframe;

const int MAXN = 100000;
const int MAXK = 100000;

class ProblemSpec : public BaseProblemSpec {
protected:
    int N, K;
    vector<int> A;

    long long result;

    void InputFormat() {
        LINE(N, K);
        LINE(A % SIZE(N));
    }

    void OutputFormat() {
        LINE(result);
    }

    void GradingConfig() {
        TimeLimit(2);
        MemoryLimit(256);
    }

    // TODO: convert to subtasks per the statement.
    void Constraints() {
        CONS(1 <= N && N <= MAXN);
        CONS(1 <= K && K <= MAXK);
        CONS(eachElementOf(A).isBetween(1, 1000000000));
    }
};

class TestSpec : public BaseTestSpec<ProblemSpec> {
protected:
    void SampleTestCase1() {
        Input({
            "5 2",
            "1 2 3 4 5"
        });
        Output({
            "9"
        });
    }

    void BeforeTestCase() { A.clear(); }

    // TODO: convert TestCases() to TestGroupX() with proper Subtasks({...})
    // assignments per the Venn diagram of the statement's subtasks.
    void TestCases() {
        CASE(N = 1, K = 1, A = {7});
        CASE(N = 10, K = 5, randomA(1, 100));
        CASE(N = MAXN, K = MAXK, randomA(1, 1000000000));
    }

private:
    void randomA(int lo, int hi) {
        for (int i = 0; i < N; i++) A.push_back(rnd.nextInt(lo, hi));
    }
};
