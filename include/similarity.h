#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <assert.h>

using namespace std;

// G1: crawled; G2: anonymized
extern vector< vector<int> > G1;
extern vector< vector<int> > G2;

// Number of nodes
extern int n1, n2;

// Similarity matrix
typedef vector< vector<double> > SimMat;
SimMat Sim[2];

void PrintMatrix(const SimMat &Sim) {
  for (int i = 1; i <= n1; i++) {
    for (int j = 1; j <= n2; j++) {
      printf("%lf ", Sim[i][j]);
    }
    printf("\n");
  }
}

void InitSimMatrix() {
  Sim[0].resize(n1 + 1);
  Sim[1].resize(n1 + 1);
  for (int i = 1; i <= n1; i++) {
    Sim[0][i].resize(n2 + 1);
    Sim[1][i].resize(n2 + 1);
    for (int j = 1; j <= n2; j++) {
      Sim[0][i][j] = 1;
    }
  }
}

double MaxMatch(int x, int y, const SimMat &Sim) {
  size_t nx = G1[x].size();
  size_t ny = G2[y].size();
  SimMat NbSim;
  NbSim.resize(nx);
  for (int i = 0; i < nx; i++) {
    NbSim[i].resize(ny);
    int nbx = G1[x][i];
    for (int j = 0; j < ny; j++) {
      int nby = G2[y][j];
      NbSim[i][j] = Sim[nbx][nby];
    }
  }

  vector<bool> matched_x(nx, 0);
  vector<bool> matched_y(ny, 0);
  vector<int> top(nx + 1, 0);
  for (int i = 0; i < nx; i++) {
    double tmp_max = 0;
    for (int j = 0; j < ny; j++) {
      if (NbSim[i][j] > tmp_max) {
        tmp_max = NbSim[i][j];
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
      if (NbSim[i][top[i]] > tmp_max && matched_x[i] == 0 && matched_y[top[i]] == 0) {
        tmp_max = NbSim[i][top[i]];
        arg = i;
      }
    }
    matched_x[arg] = 1;
    matched_y[top[arg]] = 1;
    res += NbSim[arg][top[arg]];
    cnt++;
    for (int i = 0; i < nx; i++) {
      if (top[i] == top[arg] && i != arg && matched_x[i] == 0) {
        double tmp = 0;
        for (int j = 0; j < ny; j++) {
          if (NbSim[i][j] > tmp && matched_y[j] == 0) {
            tmp = NbSim[i][j];
            top[i] = j;
          }
        }
      }
    }
  }
  return res;
}

void Iterate(const SimMat &Sim, SimMat &NewSim) {
  for (int i = 1; i <= n1; i++) {
    for (int j = 1; j <= n2; j++) {
      if (G1[i].size() > 0 && G2[j].size() > 0)
        NewSim[i][j] = MaxMatch(i, j, Sim) / max((double)G1[i].size(), (double)G2[j].size());
      else
        NewSim[i][j] = 0;
    }
  }
}

// Calculate similarity matrix
void CalcSimilarity(int k) {
  InitSimMatrix();
  int old = 0;
  for (int i = 0; i < k; i++) {
    old = i & 0x1;
    printf("ineration %d\n", i);
    Iterate(Sim[old], Sim[1-old]);
  }
  PrintMatrix(Sim[1-old]);
}