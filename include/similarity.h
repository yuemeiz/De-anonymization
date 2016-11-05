#include "utility.h"

#define BATCH 100

using namespace std;

class init_sort{
public:
  double degreeSum;
  double x, y;
  int id;

  init_sort(double v1, double v2, double v3, int v4){
      degreeSum = v1;
      x = v2;
      y = v3;
      id = v4;
  }

  init_sort(double v1){
      degreeSum = v1;
  }

  init_sort(){

  }

  bool operator < (const init_sort &a) const {
    return degreeSum < a.degreeSum;
  }
  
};

class alpha_thread_params{
public:
  int id;
  const SSimMat *sim_score;
  SSimMat *new_score;
};

class baseline_thread_params{
public:
  int id;
  const SimMat *sim_score;
  SimMat *new_score;
};

class rolesimplus_thread_params{
public:
  int id;
  const SimMat *sim_score;
  SimMat *new_score;
};


SimMat sim_score[2];
SSimMat ssim_score[2];
vector<int> seed_set;
pthread_t threads[MAX_THREAD];
vector<int> initCount;
vector<int> initIter;
vector<init_sort> sortedArray;
pthread_mutex_t alpha_mutex = PTHREAD_MUTEX_INITIALIZER;
int parallel_progress;


static void InitBaseline();
static void InitRoleSim();
static void InitRoleSimPlus();
static void InitAlphaRoleSim();
static void InitRoleSimSeed();

static double MaxMatch(int x, int y, const SimMat &sim_score, const Graph &G1, const Graph &G2);
static double MaxMatchInit(int x, int y, const Graph &G1, const Graph &G2);
static double MaxMatchAlpha(int x, int y, const SSimMat &sim_score, const Graph &G1, const Graph &G2);

static void IterateBaseline(const SimMat &sim_score, SimMat &new_score);
static void IterateRoleSim(const SimMat &sim_score, SimMat &new_score);
static void IterateRoleSimPlus(const SimMat &sim_score, SimMat &new_score);
static void IterateAlphaRoleSim(const SSimMat &sim_score, SSimMat &new_score);
static void IterateRoleSimSeed(const SimMat &sim_score, SimMat &new_score);

// Calculate similarity matrix
void CalcSimilarity(algo_iter ai);
