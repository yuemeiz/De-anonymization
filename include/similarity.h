#include "utility.h"

using namespace std;

SimMat sim_score[2];

static void InitBaseline();
static void InitRoleSim();
static void InitRoleSimPlus();
static double MaxMatch(int x, int y, const SimMat &sim_score,
                       const Graph &G1, const Graph &G2);
static void IterateBaseline(const SimMat &sim_score, SimMat &new_score);
static void IterateRoleSim(const SimMat &sim_score, SimMat &new_score);
static void IterateRoleSimPlus(const SimMat &sim_score, SimMat &new_score);
static void IterateAlphaRoleSim(const SimMat &sim_score, SimMat &new_score);

// Calculate similarity matrix
void CalcSimilarity(algo a);
