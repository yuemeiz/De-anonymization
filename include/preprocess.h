#include "utility.h"

// G1: crawled; G2: anonymized
vector< vector<int> > G1;
vector< vector<int> > G2;

// Number of nodes
int n1 = 0, n2 = 0;

// Graph preprocess: read graph and add neighbors to the vector
void PreprocessGraph();
