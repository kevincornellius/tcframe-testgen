// Custom scorer template.
// argv[1] = test case input file
// argv[2] = test case official output file (empty if NoOutput)
// argv[3] = contestant's output file
// Print verdict to stdout: "AC", "WA", or "OK\n<points>" / "OK\n<P>%".
#include <bits/stdc++.h>
using namespace std;

int ac() { cout << "AC" << endl; return 0; }
int wa() { cout << "WA" << endl; return 0; }
int partial(double pct) { cout << "OK\n" << pct << "%" << endl; return 0; }

int main(int argc, char* argv[]) {
    ifstream tcIn(argv[1]);
    ifstream tcOut(argv[2]);   // may be empty if NoOutput()
    ifstream conOut(argv[3]);

    // ---- Read input ----
    // int N; tcIn >> N;

    // ---- Read contestant's output ----
    long long ans;
    if (!(conOut >> ans)) return wa();

    // ---- Verify ans is a valid answer to the input ----
    // if (isOptimal(ans)) return ac();
    // if (isFeasible(ans)) return partial(50);
    // return wa();

    return ac();
}
