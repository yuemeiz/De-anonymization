#include "preprocess.h"

using namespace std;

// Graph preprocess: read graph and add neighbors to the vector
void PreprocessGraph(algo_iter ai, algo_match am) {
  FILE *fp = nullptr;
  int idx, idy, r;

  fp = fopen("./data/crawled.txt", "r");
  assert(fp != nullptr);
  r = fscanf(fp, "%d %d", &n1, &idy);
  G1.resize(n1 + 1);
  RG1.resize(n1 + 1);
  while (fscanf(fp, "%d %d", &idx, &idy) != EOF) {
    G1[idx].push_back(idy);
    RG1[idy].push_back(idx);
  }
  fclose(fp);

  fp = fopen("./data/anonymized.txt", "r");
  assert(fp != nullptr);
  r = fscanf(fp, "%d %d", &n2, &idy);
  G2.resize(n2 + 1);
  RG2.resize(n2 + 1);
  while (fscanf(fp, "%d %d", &idx, &idy) != EOF) {
    G2[idx].push_back(idy);
    RG2[idy].push_back(idx);
  }
  fclose(fp);

  // Input seed set
  if (ai == ALPHA_ROLESIM_SEED || am == PERCOLATE) {
    seed_set.resize(n1 + 1);
    for (int i = 0; i <= n1; i++)
      seed_set[i] = 0;
    fp = fopen("./data/seed.txt", "r");
    //assert(fp != nullptr);
    if (fp != nullptr) {
      while (fscanf(fp, "%d %d", &idx, &idy) != EOF) {
        seed_set[idx] = idy;
      }
    }
    fclose(fp);
  }
}