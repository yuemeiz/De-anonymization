#include "preprocess.h"

using namespace std;

// Graph preprocess: read graph and add neighbors to the vector
void PreprocessGraph(algo a) {
  FILE *fp = nullptr;
  int idx, idy;

  fp = fopen("./data/graph1.txt", "r");
  assert(fp != nullptr);
  fscanf(fp, "%d", &n1);
  G1.resize(n1 + 1);
  RG1.resize(n1 + 1);
  while (fscanf(fp, "%d %d", &idx, &idy) != EOF) {
    G1[idx].push_back(idy);
    RG1[idy].push_back(idx);
  }
  fclose(fp);

  fp = fopen("./data/graph2.txt", "r");
  assert(fp != nullptr);
  fscanf(fp, "%d", &n2);
  G2.resize(n2 + 1);
  RG2.resize(n2 + 1);
  while (fscanf(fp, "%d %d", &idx, &idy) != EOF) {
    G2[idx].push_back(idy);
    RG2[idy].push_back(idx);
  }
  fclose(fp);

  // Input seed set
  if (a == ROLESIM_SEED || a == PERCOLATE) {
    fp = fopen("./data/seed.txt", "r");
    assert(fp != nullptr);
    while (fscanf(fp, "%d %d", &idx, &idy) != EOF) {
      seed_set.insert(node_pair(idx, idy));
    }
    fclose(fp);
  }
}