#include "utility.h"

void PrintMatrix(const SimMat &sim_score) {
  for (int i = 1; i <= n1; i++) {
    for (int j = 1; j <= n2; j++) {
      printf("%.2f ", sim_score[i][j]);
    }
    printf("\n");
  }
}

void OutputMatrix(const SimMat &sim_score) {
  FILE *fp = nullptr;
  fp = fopen("./data/score.txt", "w");
  assert(fp != nullptr);
  for (int i = 1; i <= n1; i++) {
    for (int j = 1; j <= n2; j++) {
      fprintf(fp, "%.2f ", sim_score[i][j]);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}
