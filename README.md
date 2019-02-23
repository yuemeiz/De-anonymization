# De-anonymization


## Getting Started
Please place **anonymized.txt** and **crawled.txt** inside data folder.

## Execution

Iteration phase algorithm(ITER_ALGO_ID):
0. No iteration???
1. Baseline Iteration
2. Undirected RoleSim
3. Directed RoleSim(RoleSim++)
4. Aplha RoleSim
5. Alpha RoleSim with seed

Matching phase algorithm(MATCH_ALGO_ID):
0. No match???
1. Baseline Matching
2. Feedback
3. Feedback Alpha
4. Feedback with seed
5. PERCOLATE??
6. COMPARE_TWO(Compare NeighborMatch && BaselineMatch)

ITER_EVALUATION:
0. Do not perform intermediate matrix analysis
1. Perform intermeidate matrix analysis

In command line, type **./main ITER_ALGO_ID MATCH_ALGO_ID ITER_EVALUATION OVERLAP**.
For example: type **./main 1 1 0 5000** will run the baseline on graph pair which has 5000 nodes overlap.

