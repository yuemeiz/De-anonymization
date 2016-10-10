#pragma once

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
#include <cassert>

#define ITER_NUM 5
#define BETA 0.15
#define ALPHA 0.85
#define MAX_THREAD 16
using namespace std;

enum algo_iter {
  VOID_ITER,      // 0
  BASELINE_ITER,  // 1
  ROLESIM,        // 2 Undiredted
  ROLESIM_PLUS,   // 3 Extend RoleSim to directed graph
  ALPHA_ROLESIM,  // 4 Threshold sieved RoleSim++
  ROLESIM_SEED,   // 5 RoleSim++ with seed
};

enum algo_match {
  VOID_MATCH,     // 0
  BASELINE_MATCH, // 1
  FEEDBACK,       // 2 Increase neighbours
  FEEDBACK_SEED,  // 3
  PERCOLATE       // 4 Graph percolation
};

class node_pair {
 public:
  int id1, id2;
  node_pair(int x, int y) {
    id1 = x;
    id2 = y;
  }
  bool operator < (const node_pair &a) const {
    return (id1 < a.id1 || (id1 == a.id1 && id2 < a.id2));
  }
  bool operator == (const node_pair &a) const {
    return (id1 == a.id1 && id2 == a.id2);
  }
};

// G1: crawled; G2: anonymized
typedef vector< vector<int> > Graph;
extern Graph G1, G2;
// Reversed graph
extern Graph RG1, RG2;

extern vector<int> seed_set;

// Similarity matrix
typedef vector< double > Vec;
typedef vector< Vec > SimMat;
extern SimMat sim_score[2];

// Threshold sieved similarity scores
typedef unordered_map<int, double> SVec;
typedef vector< SVec > SSimMat;

extern SSimMat ssim_score[2];

// Number of nodes
extern int n1, n2;

// Graph preprocess: read graph and add neighbors to the vector
extern void PreprocessGraph(algo_iter ai, algo_match am);

// Calculate similarity matrix
extern void CalcSimilarity(algo_iter ai);

// Analyze intermediate result
void EvalSimilarity(algo_iter ai);

// Match two graphs based on similarity score
extern void MatchGraph(algo_match am, int overlap);

void PrintMatrix(const SimMat &sim_score);
void OutputMatrix(const SimMat &sim_score);
