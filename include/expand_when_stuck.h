#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <algorithm>
#include "utility.h"
#include "match.h"
using namespace std;

#define N 100000
#define R 2

class hash_func {
 public:
  std::size_t operator () (const node_pair &a) const {
    return (a.id1 * N + a.id2);
  }
};

class rank_pair {
 public:
  int id1, id2;
  int score;
  rank_pair(int x, int y, int s) {
    id1 = x;
    id2 = y;
    score = s;
  }
  bool operator < (const rank_pair &a) const {
    int d1 = max(G1[id1].size(), G2[id2].size())
           - min(G1[id1].size(), G2[id2].size());
    int d2 = max(G1[a.id1].size(), G2[a.id2].size())
           - min(G1[a.id1].size(), G2[a.id2].size());
    return (score < a.score || (score == a.score && d1 > d2));
  }
};

bool expand_flag = 0;
int expand_cnt = 0;

queue<node_pair> seed_queue;
priority_queue<rank_pair> candidates;
unordered_map<node_pair, int, hash_func> pairs, tmp_pairs;
unordered_map<node_pair, int, hash_func>::iterator it1, it2;

void Preprocess() {
  int small_cnt = 0;
  for (int i = 1; i <= n1; i++) {
    if (G1[i].size() < 2 || G2[i].size() < 2)
      small_cnt++;
  }
  printf("%d\n", small_cnt);
}

void Increase(int idx, int idy) {
  assert(node_match[idx] == idy && rev_node_match[idy] == idx);
  int nbx = 0, nby = 0;
  for (int i = 0; i < G1[idx].size(); i++) {
    for (int j = 0; j < G2[idy].size(); j++) {
      nbx = G1[idx][i];
      nby = G2[idy][j];
      if (node_match[nbx] == 0 && rev_node_match[nby] == 0) {
        it1 = pairs.find(node_pair(nbx, nby));
        if (it1 == pairs.end())
          pairs.insert(pair<node_pair, int>(node_pair(nbx, nby), 1));
        else it1->second += 1;
      }
    }
  }
}

void MatchPair(int idx, int idy) {
  if (node_match[idx] != 0 || rev_node_match[idy] != 0)
    return;
  node_match[idx] = idy;
  rev_node_match[idy] = idx;
  total_cnt++;
  if (map_node[idx] == idy)
    correct_cnt++;
  Increase(idx, idy);
}

// initiate seed set with pairs with front-seed_num scores
void InitSeed() {
  FILE *fp = nullptr;
  fp = fopen("./data/seed.txt", "r");
  assert(fp != nullptr);
  int x, y;
  while (fscanf(fp, "%d %d", &x, &y) != EOF) {
    seed_queue.push(node_pair(x, y));
  }
  fclose(fp);
}

// match seed set and spread marks
void MatchSeed() {
  while (!seed_queue.empty()) {
    MatchPair(seed_queue.front().id1, seed_queue.front().id2);
    Increase(seed_queue.front().id1, seed_queue.front().id2);
    seed_queue.pop();
  }
}

void GetCandidates() {
  for (it1 = pairs.begin(); it1 != pairs.end(); it1++) {
    if (it1->second >= R && node_match[it1->first.id1] == 0
        && rev_node_match[it1->first.id2] == 0) {
      candidates.push(rank_pair(it1->first.id1, it1->first.id2, it1->second));
    }
  }
}

void MatchCandidates() {
  while (!candidates.empty()) {
    MatchPair(candidates.top().id1, candidates.top().id2);
    candidates.pop();
  }
  for (it1 = pairs.begin(); it1 != pairs.end();) {
    int x = it1->first.id1;
    int y = it1->first.id2;
    if ((node_match[x] > 0) || (rev_node_match[y] > 0)) {
      it1 = pairs.erase(it1);
    }
    else {
      it1++;
    }
  }
}

inline void IncreaseAndExpand(int idx, int idy) {
  if (node_match[idx] == 0 && rev_node_match[idy] == 0) {
    int nbx = 0, nby = 0;
    for (int i = 0; i < G1[idx].size(); i++) {
      for (int j = 0; j < G2[idy].size(); j++) {
        nbx = G1[idx][i];
        nby = G2[idy][j];
        if (node_match[nbx] == 0 && rev_node_match[nby] == 0) {
          expand_flag = 1;
          it2 = pairs.find(node_pair(nbx, nby));
          if (it2 == pairs.end()) {
            it2 = tmp_pairs.find(node_pair(nbx, nby));
            if (it2 == tmp_pairs.end()) {
              tmp_pairs.insert(pair<node_pair, int>(node_pair(nbx, nby), 1));
            }
            else it2->second += 1;
          }
          else it2->second += 1;
        }
      }
    }
  }
}

void Expand() {
  expand_cnt++;
  printf("expand #%d:\n", expand_cnt);
  printf("correct: %d\n", correct_cnt);
  printf("pairs: %lu\n", pairs.size());
  for (it1 = pairs.begin(); it1 != pairs.end(); it1++)
    IncreaseAndExpand(it1->first.id1, it1->first.id2);
  for (it2 = tmp_pairs.begin(); it2 != tmp_pairs.end(); it2++)
    pairs.insert(pair<node_pair, int>(*it2));
  tmp_pairs.clear();
}

void ExpandWhenStuck() {
  Preprocess();
  InitSeed();
  MatchSeed();
  while (total_cnt < n1) {
    GetCandidates();
    while (candidates.empty()) {
      expand_flag = 0;
      Expand();
      if (expand_flag == 0)
        break;
      GetCandidates();
    }
    if (candidates.empty())
      break;
    MatchCandidates();
  }
  printf("correct count: %d\n", correct_cnt);
  printf("total count: %d\n", total_cnt);
}
