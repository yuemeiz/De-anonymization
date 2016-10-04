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

void EvalSimilarity(algo_iter ai) {
  int correct_cnt = 0;
  vector<int> map_node;
  // Initiate map_node
  map_node.resize(n1 + 1);
  for (int i = 1; i <= n1; i++)
    map_node[i] = i;

  if (ai != ALPHA_ROLESIM) {
    const SimMat &score = sim_score[ITER_NUM & 0x1];
    for (int i = 1; i <= n1; i++) {
      double tmp_max = -1;
      int top_id = -1;
      for (int j = 1; j <= n2; j++)
        if (tmp_max < score[i][j]) {
          tmp_max = score[i][j];
          top_id = j;
        }
      if (top_id == map_node[i])
        correct_cnt++;
    }
  }
  else {
    const SSimMat &score = ssim_score[ITER_NUM & 0x1];
    for (int i = 1; i <= n1; i++) {
      double tmp_max = -1;
      int top_id = -1;
      for (auto node : score[i]) {
        if (tmp_max < node.second) {
          tmp_max = node.second;
          top_id = node.first;
        }
      }
      if (top_id == map_node[i])
        correct_cnt++;
    }
  }
  printf("correct intermediate score: %d\n", correct_cnt);
}
