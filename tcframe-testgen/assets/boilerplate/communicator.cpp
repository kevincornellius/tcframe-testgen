// Communicator (interactor) template for interactive problems.
// argv[1] = test case input file
// stdin  = contestant's output
// stdout = data sent to contestant (FLUSH after every write!)
// stderr = verdict ("AC", "WA", or "OK\n<points>")
#include <bits/stdc++.h>
using namespace std;

int ac() { cerr << "AC" << endl; return 0; }
int wa() { cerr << "WA" << endl; return 0; }

int main(int argc, char* argv[]) {
    ifstream tcIn(argv[1]);

    int N;
    tcIn >> N;   // the hidden number

    int queries = 0;
    while (true) {
        int guess;
        if (!(cin >> guess)) return wa();
        queries++;
        if (queries > 20) return wa();      // too many queries
        if (guess == N) return ac();
        cout << (guess < N ? "HIGHER" : "LOWER") << endl;
        cout.flush();                       // CRITICAL
    }
}
