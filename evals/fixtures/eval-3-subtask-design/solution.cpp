#include <bits/stdc++.h>
using namespace std;

int main() {
    int N, K; cin >> N >> K;
    vector<long long> A(N);
    for (auto &x : A) cin >> x;
    sort(A.begin(), A.end(), greater<long long>());
    long long sum = 0;
    for (int i = 0; i < min(N, K); i++) sum += A[i];
    cout << sum << "\n";
}
