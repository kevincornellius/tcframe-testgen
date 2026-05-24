# Top-K Sum

You are given N positive integers and an integer K. Output the sum of
the K largest values (if K ≥ N, output the sum of all of them).

## Input
- Line 1: N K
- Line 2: A[0] A[1] … A[N-1]

## Output
- A single line with the sum.

## Subtasks
- **Subtask 1 (20 points):** N ≤ 10, K ≤ 10.
- **Subtask 2 (30 points):** N ≤ 1000, K ≤ 1000.
- **Subtask 3 (50 points):** N ≤ 100,000, K ≤ 100,000.

Global: 1 ≤ N, K ≤ 100,000 and 1 ≤ A[i] ≤ 10^9.

## Sample
```
5 2
1 2 3 4 5
```
Output:
```
9
```
