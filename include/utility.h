#pragma once

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <assert.h>

#define ITER_NUM 5
#define BETA 0.15

using namespace std;

enum algo {
  BASELINE,
  ROLESIM,
  ROLESIM_PLUS,  // Extend RoleSim to directed graph
  ALPHA_ROLESIM  // Threshold sieved RoleSim++
};

// G1: crawled; G2: anonymized
typedef vector< vector<int> > Graph;
extern Graph G1, G2;
// Reversed graph
extern Graph RG1, RG2;

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
