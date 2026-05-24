#include <bits/stdc++.h>
using namespace std;

int main() {
    int N; cin >> N;
    int best = 0;
    for (int i = 0; i < N; i++) { int x; cin >> x; best = max(best, x); }
    cout << best << "\n";
}
