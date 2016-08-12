#include "utility.h"

void PrintMatrix(const SimMat &sim_score) {
  for (int i = 1; i <= n1; i++) {
    for (int j = 1; j <= n2; j++) {
      printf("%.2f ", sim_score[i][j]);
    }
    printf("\n");
  }
}
