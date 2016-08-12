#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <assert.h>

using namespace std;

// G1: crawled; G2: anonymized
vector< vector<int> > G1;
vector< vector<int> > G2;

// Number of nodes
int n1 = 0, n2 = 0;

// Graph preprocess: read graph and add neighbors to the vector
void PreprocessGraph() {
  printf("hello\n");
  FILE *fp = nullptr;
  uint idx, idy;

  fp = fopen("./data/graph1.txt", "r");
  assert(fp != nullptr);
  fscanf(fp, "%d", &n1);
  G1.resize(n1 + 1);
  while (fscanf(fp, "%d %d", &idx, &idy) != EOF) {
    G1[idx].push_back(idy);
  }
  fclose(fp);

  fp = fopen("./data/graph2.txt", "r");
  assert(fp != nullptr);
  fscanf(fp, "%d", &n2);
  G2.resize(n2 + 1);
  while (fscanf(fp, "%d %d", &idx, &idy) != EOF) {
    G2[idx].push_back(idy);
  }
  fclose(fp);
}