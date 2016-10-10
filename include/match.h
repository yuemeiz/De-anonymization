#pragma once

#include "utility.h"

using namespace std;

static SimMat rank_score;    // a copy of score matrix for ranking and updating
static vector<int> top;      // top ranked id for each node
static vector<int> map_node;
static vector<int> node_match, rev_node_match;
static int total_cnt = 0;
static int correct_cnt = 0;

static void Initiate(const SimMat &score);
static int GetMax();
static void MatchNode(int idx, int idy, int overlap);
static void IncreaseNb(int idx, int idy, const SimMat &score);

void MatchGraph(algo_match am);