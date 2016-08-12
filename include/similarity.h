#include "utility.h"

using namespace std;

SimMat sim_score[2];

static void PrintMatrix(const SimMat &sim_score);
static void InitSimMatrix();
static double MaxMatch(int x, int y, const SimMat &sim_score);
static void Iterate(const SimMat &sim_score, SimMat &new_score);

// Calculate similarity matrix
void CalcSimilarity();
