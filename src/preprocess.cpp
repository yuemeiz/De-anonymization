#include "preprocess.h"

using namespace std;

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