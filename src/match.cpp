#include "match.h"
#include "expand_when_stuck.h"

using namespace std;

static void Initiate(const SimMat &score) {
  // Initeate node_match and rev_node_match
  node_match.resize(n1 + 1);
  rev_node_match.resize(n2 + 1);
  for (int i = 1; i <= n1; i++)
    node_match[i] = 0;
  for (int i = 1; i <= n2; i++)
    rev_node_match[i] = 0;

  // Initiate rank_score
  rank_score.resize(n1 + 1);
  for (int i = 1; i <= n1; i++){
    rank_score[i].resize(n2 + 1);
    for (int j = 1; j <= n2; j++)
      rank_score[i][j] = score[i][j];
  }

  // Initiate top[N]
  top.resize(n1 + 1);
  for (int i = 1; i <= n1; i++) {
    double tmp_max = -1;
    for (int j = 1; j <= n2; j++)
      if (tmp_max < rank_score[i][j]) {
        tmp_max = rank_score[i][j];
        top[i] = j;
      }
  }

  // Initiate map_node
  map_node.resize(n1 + 1);
  for (int i = 1; i <= n1; i++)
    map_node[i] = i;
}

// For our compressed storage match
static void InitiateAlpha(SSimMat &score) {
  // Initeate node_match and rev_node_match
  node_match.resize(n1 + 1);
  rev_node_match.resize(n2 + 1);
  for (int i = 1; i <= n1; i++)
    node_match[i] = 0;
  for (int i = 1; i <= n2; i++)
    rev_node_match[i] = 0;
  
  // Initiate top[N]
  top.resize(n1 + 1);
  for (int i = 1; i <= n1; i++) {
    double tmp_max = -1;
    SVec::iterator it;
    for (it = score[i].begin(); it != score[i].end(); it++){
      if (it->second > tmp_max){
        tmp_max = it->second;
        top[i] = it->first;
      }
    }
  }

  // Initiate map_node
  // n1 is the crawled, n2 is the anonymized
  map_node.resize(n1 + 1);
  for (int i = 1; i <= n1; i++)
    map_node[i] = i;
}


static int GetMax() {
  double tmp_max = -1;
  int tmp_x = 0;
  for (int i = 1; i <= n1; i++)
    if (tmp_max < rank_score[i][top[i]] &&
        node_match[i] == 0 && rev_node_match[top[i]] == 0) {
      tmp_max = rank_score[i][top[i]];
      tmp_x = i;
    }
  return tmp_x;
}

static int GetMaxAlpha(SSimMat &score) {
  double tmp_max = -1;
  int tmp_x = 0;
  for (int i = 1; i <= n1; i++)
    if (tmp_max < score[i][top[i]] &&
        node_match[i] == 0 && rev_node_match[top[i]] == 0) {
      tmp_max = score[i][top[i]];
      tmp_x = i;
    }
  return tmp_x;
}

static void MatchNode(int idx, int idy, int overlap) {
  assert(node_match[idx] == 0 && rev_node_match[idy] == 0);
  //printf("match: %d %d\n", idx, idy);
  node_match[idx] = idy;
  rev_node_match[idy] = idx;
  total_cnt++;
  if (idy == map_node[idx] && idx <= overlap)
    correct_cnt++;
  // update top[N] with free nodes
  for (int i = 1; i <= n1; i++)
    // node i cannot be matched with idy anymore, need to find a free node
    if (top[i] == idy && node_match[i] == 0) {
      double tmp_max = -1;
      for (int j = 1; j <= n2; j++)
        if (tmp_max < rank_score[i][j] && rev_node_match[j] == 0) {
          tmp_max = rank_score[i][j];
          top[i] = j;
        }
    }
}


static void MatchNodeAlpha(int idx, int idy, SSimMat &score, int overlap) {
  assert(node_match[idx] == 0 && rev_node_match[idy] == 0);
  //printf("match: %d %d\n", idx, idy);
  node_match[idx] = idy;
  rev_node_match[idy] = idx;
  total_cnt++;
  if (idy == map_node[idx] && idx <= overlap)
    correct_cnt++;
  // update top[N] with free nodes
  for (int i = 1; i <= n1; i++)
    // node i cannot be matched with idy anymore, need to find a free node
    if (top[i] == idy && node_match[i] == 0) {
      double tmp_max = -1;
      SVec::iterator it;
      for (it = score[i].begin(); it != score[i].end(); it++){
        if (it->second > tmp_max && rev_node_match[it->first] == 0){
          tmp_max = it->second;
          top[i] = it->first;
        }
      }
    }
}

static void IncreaseNb(int idx, int idy, const SimMat &score) {
  (void) score;
  int nbx = 0, nby = 0;
  for (int i = 0; i < G1[idx].size(); i++)
    for (int j = 0; j < G2[idy].size(); j++) {
      nbx = G1[idx][i];
      nby = G2[idy][j];
      if (node_match[nbx] == 0 && rev_node_match[nby] == 0) {
        rank_score[nbx][nby] += 1;
        if (rank_score[nbx][nby] > rank_score[nbx][top[nbx]])
          top[nbx] = nby;
      }
    }
}

static void IncreaseNbAlpha(int idx, int idy, SSimMat &score) {

  int nbx = 0, nby = 0;
  for (int i = 0; i < G1[idx].size(); i++)
    for (int j = 0; j < G2[idy].size(); j++) {
      nbx = G1[idx][i];
      nby = G2[idy][j];
      if (node_match[nbx] == 0 && rev_node_match[nby] == 0) {
        if (score[nbx].find(nby) == score[nbx].end()){
          score[nbx][nby] = BETA;
        }
        score[nbx][nby] += 1;
        if (score[nbx][nby] > score[nbx][top[nbx]]){
          top[nbx] = nby;
        }
      }
    }
}



void MatchGraph(algo_match am, int overlap) {
  switch (am) {
    case VOID_MATCH: {
      printf("void match\n");
      break;
    }
    case BASELINE_MATCH: {
      Initiate(sim_score[ITER_NUM & 0x1]);
      while (total_cnt < n1) {
        int idx = GetMax();
        assert(idx != 0);
        MatchNode(idx, top[idx], overlap);
      }
      printf("correct match: %d\n", correct_cnt);
      break;
    }
    case FEEDBACK: {
      //Initiate(sim_score[ITER_NUM & 0x1]);
      InitiateAlpha(ssim_score[ITER_NUM & 0x1]);
      while (total_cnt < n1) {
        //int idx = GetMax();
        int idx = GetMaxAlpha(ssim_score[ITER_NUM & 0x1]);
        if (idx == 0) break;
        assert(idx != 0);
        MatchNodeAlpha(idx, top[idx], ssim_score[ITER_NUM & 0x1], overlap);
        IncreaseNbAlpha(idx, top[idx], ssim_score[ITER_NUM & 0x1]);
      }
      printf("correct match: %d\n", correct_cnt);
      break;
    }
    case FEEDBACK_SEED: {
      Initiate(sim_score[ITER_NUM & 0x1]);
      // Match seeds
      for (int i = 1; i <= n1; i++)
        if (seed_set[i] != 0) {
          MatchNode(i, seed_set[i], overlap);
          IncreaseNb(i, seed_set[i], sim_score[ITER_NUM & 0x1]);
        }
      while (total_cnt < n1) {
        int idx = GetMax();
        assert(idx != 0);
        MatchNode(idx, top[idx], overlap);
        IncreaseNb(idx, top[idx], sim_score[ITER_NUM & 0x1]);
      }
      printf("correct match: %d\n", correct_cnt);
      break;
    }
    case PERCOLATE: {
      Initiate(sim_score[ITER_NUM & 0x1]);
      ExpandWhenStuck();
      break;
    }
    default:
      assert(0);
  }
}