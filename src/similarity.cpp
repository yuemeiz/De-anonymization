#include "similarity.h"

using namespace std;

static void InitBaseline() {
  sim_score[0].resize(n1 + 1);
  sim_score[1].resize(n1 + 1);
  for (int i = 1; i <= n1; i++) {
    sim_score[0][i].resize(n2 + 1);
    sim_score[1][i].resize(n2 + 1);
    for (int j = 1; j <= n2; j++) {
      sim_score[0][i][j] = 1;
    }
  }
}

static void InitRoleSim() {
  sim_score[0].resize(n1 + 1);
  sim_score[1].resize(n1 + 1);
  for (int i = 1; i <= n1; i++) {
    sim_score[0][i].resize(n2 + 1);
    sim_score[1][i].resize(n2 + 1);
    for (int j = 1; j <= n2; j++) {
      sim_score[0][i][j] = min((double)G1[i].size(), (double)G2[j].size())
                         / max((double)G1[i].size(), (double)G2[j].size())
                         * (1 - BETA) + BETA;
    }
  }
}

static void InitRoleSimPlus() {
  sim_score[0].resize(n1 + 1);
  sim_score[1].resize(n1 + 1);
  for (int i = 1; i <= n1; i++) {
    sim_score[0][i].resize(n2 + 1);
    sim_score[1][i].resize(n2 + 1);
    for (int j = 1; j <= n2; j++) {
      sim_score[0][i][j] = (min((double)G1[i].size(), (double)G2[j].size())
                         + min((double)RG1[i].size(), (double)RG2[j].size()))
                         / (max((double)G1[i].size(), (double)G2[j].size())
                         + max((double)RG1[i].size(), (double)RG2[j].size()))
                         * (1 - BETA) + BETA;
    }
  }
}

static void InitRoleSimSeed() {
  sim_score[0].resize(n1 + 1);
  sim_score[1].resize(n1 + 1);
  for (int i = 1; i <= n1; i++) {
    sim_score[0][i].resize(n2 + 1);
    sim_score[1][i].resize(n2 + 1);
    if (seed_set[i] != 0) {
      for (int j = 1; j <= n2; j++)
        sim_score[0][i][j] = BETA;
      sim_score[0][i][seed_set[i]] = 1;
      continue;
    }
    for (int j = 1; j <= n2; j++) {
      sim_score[0][i][j] = (min((double)G1[i].size(), (double)G2[j].size())
                         + min((double)RG1[i].size(), (double)RG2[j].size()))
                         / (max((double)G1[i].size(), (double)G2[j].size())
                         + max((double)RG1[i].size(), (double)RG2[j].size()))
                         * (1 - BETA) + BETA;
    }
  }
}

static double MaxMatch(int x, int y, const SimMat &sim_score,
                       const Graph &G1, const Graph &G2) {
  size_t nx = G1[x].size();
  size_t ny = G2[y].size();
  SimMat nb_score;
  nb_score.resize(nx);
  for (int i = 0; i < nx; i++) {
    nb_score[i].resize(ny);
    int nbx = G1[x][i];
    for (int j = 0; j < ny; j++) {
      int nby = G2[y][j];
      nb_score[i][j] = sim_score[nbx][nby];
    }
  }

  vector<bool> matched_x(nx, 0);
  vector<bool> matched_y(ny, 0);
  vector<int> top(nx + 1, 0);
  for (int i = 0; i < nx; i++) {
    double tmp_max = 0;
    for (int j = 0; j < ny; j++) {
      if (nb_score[i][j] > tmp_max) {
        tmp_max = nb_score[i][j];
        top[i] = j;
      }
    }
  }

  int cnt = 0;
  double res = 0;
  while (cnt < nx && cnt < ny) {
    int arg = 0;
    double tmp_max = 0;
    for (int i = 0; i < nx; i++) {
      if (nb_score[i][top[i]] > tmp_max
          && matched_x[i] == 0 && matched_y[top[i]] == 0) {
        tmp_max = nb_score[i][top[i]];
        arg = i;
      }
    }
    matched_x[arg] = 1;
    matched_y[top[arg]] = 1;
    res += nb_score[arg][top[arg]];
    cnt++;
    for (int i = 0; i < nx; i++)
      if (top[i] == top[arg] && i != arg && matched_x[i] == 0) {
        double tmp = 0;
        for (int j = 0; j < ny; j++) {
          if (nb_score[i][j] > tmp && matched_y[j] == 0) {
            tmp = nb_score[i][j];
            top[i] = j;
          }
        }
      }
  }
  return res;
}

static void IterateBaseline(const SimMat &sim_score, SimMat &new_score) {
  for (int i = 1; i <= n1; i++) {
    for (int j = 1; j <= n2; j++) {
      new_score[i][j] = MaxMatch(i, j, sim_score, G1, G2);
    }
  }
  //Normalization
  double tmp_max = 0;
  for (int i = 1; i <= n1; i++)
    for (int j = 1; j <= n2; j++)
      if (new_score[i][j] > tmp_max)
        tmp_max = new_score[i][j];
  if (tmp_max > 0)
    for (int i = 1; i <= n1; i++)
      for (int j = 1; j <= n2; j++)
        new_score[i][j] /= tmp_max;
}

static void IterateRoleSim(const SimMat &sim_score, SimMat &new_score) {
  for (int i = 1; i <= n1; i++) {
    for (int j = 1; j <= n2; j++) {
      if (G1[i].size() > 0 && G2[j].size() > 0)
        new_score[i][j] = MaxMatch(i, j, sim_score, G1, G2)
                        / max((double)G1[i].size(), (double)G2[j].size())
                        * (1 - BETA) + BETA;
      else
        new_score[i][j] = BETA;
    }
  }
}

static void IterateRoleSimPlus(const SimMat &sim_score, SimMat &new_score) {
  for (int i = 1; i <= n1; i++) {
    for (int j = 1; j <= n2; j++) {
      if (G1[i].size() > 0 && G2[j].size() > 0)
        new_score[i][j] = (MaxMatch(i, j, sim_score, G1, G2)
                        + MaxMatch(i, j, sim_score, RG1, RG2))
                        / (max((double)G1[i].size(), (double)G2[j].size())
                        + max((double)RG1[i].size(), (double)RG2[j].size()))
                        * (1 - BETA) + BETA;
      else
        new_score[i][j] = BETA;
    }
  }
}

static void IterateAlphaRoleSim(const SimMat &sim_score, SimMat &new_score) {
  for (int i = 1; i <= n1; i++) {
    // Find highest score
    double tmp_max = 0;
    for (int j = 1; j <= n2; j++)
      if (sim_score[i][j] > tmp_max)
        tmp_max = sim_score[i][j];

    double theta = tmp_max * ALPHA;
    for (int j = 1; j <= n2; j++) {
      if (sim_score[i][j] < theta) {
        new_score[i][j] = sim_score[i][j];
        continue;
      }
      if (G1[i].size() > 0 && G2[j].size() > 0)
        new_score[i][j] = (MaxMatch(i, j, sim_score, G1, G2)
                        + MaxMatch(i, j, sim_score, RG1, RG2))
                        / (max((double)G1[i].size(), (double)G2[j].size())
                        + max((double)RG1[i].size(), (double)RG2[j].size()))
                        * (1 - BETA) + BETA;
      else
        new_score[i][j] = BETA;
    }
  }
}

static void IterateRoleSimSeed(const SimMat &sim_score, SimMat &new_score) {
  for (int i = 1; i <= n1; i++) {
    if (seed_set[i] != 0)
      continue;
      // Find highest score
    double tmp_max = 0;
    for (int j = 1; j <= n2; j++)
      if (sim_score[i][j] > tmp_max)
        tmp_max = sim_score[i][j];

    double theta = tmp_max * ALPHA;
    for (int j = 1; j <= n2; j++) {
      if (sim_score[i][j] < theta) {
        new_score[i][j] = sim_score[i][j];
        continue;
      }
      if (G1[i].size() > 0 && G2[j].size() > 0)
        new_score[i][j] = (MaxMatch(i, j, sim_score, G1, G2)
                        + MaxMatch(i, j, sim_score, RG1, RG2))
                        / (max((double)G1[i].size(), (double)G2[j].size())
                        + max((double)RG1[i].size(), (double)RG2[j].size()))
                        * (1 - BETA) + BETA;
      else
        new_score[i][j] = BETA;
    }
  }
}

// Calculate similarity matrix
void CalcSimilarity(algo a) {
  switch (a) {
    case BASELINE:
      InitBaseline();
      //PrintMatrix(sim_score[0]);
      for (int i = 0; i < ITER_NUM; i++) {
        int old = i & 0x1;
        printf("iteration %d\n", i);
        IterateBaseline(sim_score[old], sim_score[1 - old]);
        //PrintMatrix(sim_score[1 - old]);
      }
      break;
    case ROLESIM: {
      InitRoleSim();
      //PrintMatrix(sim_score[0]);
      for (int i = 0; i < ITER_NUM; i++) {
        int old = i & 0x1;
        printf("iteration %d\n", i);
        IterateRoleSim(sim_score[old], sim_score[1 - old]);
        //PrintMatrix(sim_score[1 - old]);
      }
      break;
    }
    case ROLESIM_PLUS: {
      InitRoleSimPlus();
      //PrintMatrix(sim_score[0]);
      for (int i = 0; i < ITER_NUM; i++) {
        int old = i & 0x1;
        printf("iteration %d\n", i);
        IterateRoleSimPlus(sim_score[old], sim_score[1 - old]);
        //PrintMatrix(sim_score[1 - old]);
      }
      break;
    }
    case ALPHA_ROLESIM: {
      // Initialization is same as RoleSim++
      InitRoleSimPlus();
      //PrintMatrix(sim_score[0]);
      for (int i = 0; i < ITER_NUM; i++) {
        int old = i & 0x1;
        printf("iteration %d\n", i);
        IterateAlphaRoleSim(sim_score[old], sim_score[1 - old]);
        //PrintMatrix(sim_score[1 - old]);
      }
      break;
    }
    case ROLESIM_SEED: {
      InitRoleSimSeed();
      //PrintMatrix(sim_score[0]);
      for (int i = 0; i < ITER_NUM; i++) {
        int old = i & 0x1;
        printf("iteration %d\n", i);
        //IterateAlphaRoleSim(sim_score[old], sim_score[1 - old]);
        IterateRoleSimSeed(sim_score[old], sim_score[1 - old]);
        //PrintMatrix(sim_score[1 - old]);
      }
      break;
    }
    case PERCOLATE:
      InitBaseline();
      break;
    default:
      assert(0);
  }
  //PrintMatrix(sim_score[ITER_NUM & 0x1]);
  if (a != PERCOLATE)
    OutputMatrix(sim_score[ITER_NUM & 0x1]);
}