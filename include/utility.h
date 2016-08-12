#pragma once

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <assert.h>

#define ITER_NUM 5

using namespace std;

// G1: crawled; G2: anonymized
extern vector< vector<int> > G1;
extern vector< vector<int> > G2;

// Similarity matrix
typedef vector< vector<double> > SimMat;
extern SimMat sim_score[2];

// Number of nodes
extern int n1, n2;

// Graph preprocess: read graph and add neighbors to the vector
extern void PreprocessGraph();

// Calculate similarity matrix
extern void CalcSimilarity();

// Match two graphs based on similarity score
extern void MatchGraph();