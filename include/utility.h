#pragma once

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <set>
#include <assert.h>

#define ITER_NUM 5
#define BETA 0.15
#define ALPHA 0.85

using namespace std;

enum algo {
  BASELINE,       // 0
  ROLESIM,        // 1
  ROLESIM_PLUS,   // 2 Extend RoleSim to directed graph
  ALPHA_ROLESIM,  // 3 Threshold sieved RoleSim++
  ROLESIM_SEED,   // 4
  PERCOLATE       // 5
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
typedef vector< vector<double> > SimMat;
extern SimMat sim_score[2];

// Number of nodes
extern int n1, n2;

// Graph preprocess: read graph and add neighbors to the vector
extern void PreprocessGraph(algo a);

// Calculate similarity matrix
extern void CalcSimilarity(algo a);

// Match two graphs based on similarity score
extern void MatchGraph(algo a);

void PrintMatrix(const SimMat &sim_score);
void OutputMatrix(const SimMat &sim_score);
