#include "similarity.h"

using namespace std;


static void *InitBaselineThread(void *paramIn){
  struct baseline_thread_params *param = (struct baseline_thread_params *)paramIn;
  int id = param->id;
  bool stopFetch = false;
  while (!stopFetch){
    int batch_start, batch_end;
    pthread_mutex_lock(&alpha_mutex);
    if (parallel_progress > n1){
      batch_start = -1;
      batch_end = -1;
      stopFetch = true;
    } else {
      batch_start = parallel_progress;
      batch_end = batch_start + BATCH;
      if (batch_end > n1) batch_end = n1;
      parallel_progress = batch_end + 1;
      if (parallel_progress > n1) {
        stopFetch = true;
      }
    }
    pthread_mutex_unlock(&alpha_mutex);
    if (batch_start == -1){
      break;
    }
    for (int i = batch_start; i <= batch_end; i += 1){
      for (int j = 1; j <= n2; j++){
        sim_score[0][i][j] = 1; 
      }
    }
  }
  return NULL;
}


static void InitBaseline() {
  sim_score[0].resize(n1 + 1);
  sim_score[1].resize(n1 + 1);
  for (int i = 1; i <= n1; i++) {
    sim_score[0][i].resize(n2 + 1);
    sim_score[1][i].resize(n2 + 1);
  }
  struct baseline_thread_params params[MAX_THREAD];
  parallel_progress = 1;

  for (int i = 0; i < MAX_THREAD; i++){
    params[i].id = i + 1;
  }
  
  for (int i = 0; i < MAX_THREAD; i++){
    pthread_create(&threads[i], NULL, InitBaselineThread, (void *)&params[i]);
  }

	for (int i = 0; i < MAX_THREAD; i++) {
    pthread_join(threads[i], NULL);
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

static void *InitRoleSimPlusThread(void *paramIn){
  struct rolesimplus_thread_params *param = (struct rolesimplus_thread_params *)paramIn;
  int id = param->id;
  bool stopFetch = false;
  while (!stopFetch){
    int batch_start, batch_end;
    pthread_mutex_lock(&alpha_mutex);
    if (parallel_progress > n1){
      batch_start = -1;
      batch_end = -1;
      stopFetch = true;
    } else {
      batch_start = parallel_progress;
      batch_end = batch_start + BATCH;
      if (batch_end > n1) batch_end = n1;
      parallel_progress = batch_end + 1;
      if (parallel_progress > n1) {
        stopFetch = true;
      }
    }
    pthread_mutex_unlock(&alpha_mutex);
    if (batch_start == -1){
      break;
    }
    for (int i = batch_start; i <= batch_end; i += 1){
      for (int j = 1; j <= n2; j++){
        sim_score[0][i][j] = (min((double)G1[i].size(), (double)G2[j].size())
                         + min((double)RG1[i].size(), (double)RG2[j].size()))
                         / (max((double)G1[i].size(), (double)G2[j].size())
                         + max((double)RG1[i].size(), (double)RG2[j].size()))
                         * (1 - BETA) + BETA; 
      }
    }
  }
  return NULL;
}

static void InitRoleSimPlus() {
  sim_score[0].resize(n1 + 1);
  sim_score[1].resize(n1 + 1);
  for (int i = 1; i <= n1; i++) {
    sim_score[0][i].resize(n2 + 1);
    sim_score[1][i].resize(n2 + 1);
  }
  struct rolesimplus_thread_params params[MAX_THREAD];
  parallel_progress = 1;

  for (int i = 0; i < MAX_THREAD; i++){
    params[i].id = i + 1;
  }
  
  for (int i = 0; i < MAX_THREAD; i++){
    pthread_create(&threads[i], NULL, InitRoleSimPlusThread, (void *)&params[i]);
  }

	for (int i = 0; i < MAX_THREAD; i++) {
    pthread_join(threads[i], NULL);
  }
}


static void *InitAlphaRoleSimThread(void *paramIn){
  struct alpha_thread_params *param = (struct alpha_thread_params *)paramIn;
  int id = param->id;
  bool stopFetch = false;
  while (!stopFetch){
    int batch_start, batch_end;
    pthread_mutex_lock(&alpha_mutex);
    if (parallel_progress > n1){
      batch_start = -1;
      batch_end = -1;
      stopFetch = true;
    } else {
      batch_start = parallel_progress;
      batch_end = batch_start + BATCH;
      if (batch_end > n1) batch_end = n1;
      parallel_progress = batch_end + 1;
      if (parallel_progress > n1) {
        stopFetch = true;
      }
    }
    pthread_mutex_unlock(&alpha_mutex);
    //printf("%d get [%d-%d]\n", id, batch_start, batch_end);
    if (batch_start == -1){
      break;
    }
    for (int i = batch_start; i <= batch_end; i += 1){
      int iterCount = 0;
      
      double w1 = double(G1[i].size());
      double w2 = double(RG1[i].size());
      double degreeSum = w1 + w2;

      double upperLimit = degreeSum / THETA;
      double lowerLimit = degreeSum * THETA;
      vector<init_sort>::iterator lower, upper, it;
      
      lower = std::lower_bound(sortedArray.begin(), sortedArray.end(), init_sort(lowerLimit)); 
      upper = std::upper_bound(sortedArray.begin(), sortedArray.end(), init_sort(upperLimit));
      if (lower >= upper) continue;
      initIter[i] = upper - lower;
      for (it = lower; it != upper; it ++){
        double x = it->x;
        double y = it->y;
        if (y > ((x + w2) / THETA - w1)) continue;
        if (y < ((x + w2) * THETA - w1)) continue;
        int j = it->id;

        iterCount += 1;
        ssim_score[0][i][j] = (MaxMatchInit(i, j, G1, G2)
                              + MaxMatchInit(i, j, RG1, RG2))
                              / (max((double)G1[i].size(), (double)G2[j].size())
                              + max((double)RG1[i].size(), (double)RG2[j].size()))
                              * (1 - BETA) + BETA;
        
        
      }
      initCount[i] = iterCount;
    }

  }
  return NULL;
}

static void InitAlphaRoleSim() {
  ssim_score[0].resize(n1 + 1);
  ssim_score[1].resize(n1 + 1);
  initCount.resize(n1 + 1);
  initIter.resize(n1 + 1);
  
  
  for (int i = 1; i <= n2; i++){
    double x = double(G2[i].size());
    double y = double(RG2[i].size());
    sortedArray.push_back(init_sort(x + y, x, y, i));
  }

  sort(sortedArray.begin(), sortedArray.end());

  struct alpha_thread_params params[MAX_THREAD];
  parallel_progress = 1;

  for (int i = 0; i < MAX_THREAD; i++){
    params[i].id = i + 1;
  }
  
  for (int i = 0; i < MAX_THREAD; i++){
    pthread_create(&threads[i], NULL, InitAlphaRoleSimThread, (void *)&params[i]);
  }

	for (int i = 0; i < MAX_THREAD; i++) {
    pthread_join(threads[i], NULL);
  }

  int avg = 0;
  for (int i = 1; i <= n1; i++){
    avg += initIter[i];
  }
  avg /= n1;
  printf("average initIter is %d\n", avg);

  avg = 0;
  for (int i = 1; i <= n1; i++){
    avg += initCount[i];
  }
  avg /= n1;
  printf("average initCount is %d\n", avg);

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

static double MaxMatch(int x, int y, const SimMat &sim_score, const Graph &G1, const Graph &G2) {
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

static double MaxMatchInit(int x, int y, const Graph &G1, const Graph &G2) {
  size_t nx = G1[x].size();
  size_t ny = G2[y].size();
  SimMat nb_score;
  nb_score.resize(nx);
  for (int i = 0; i < nx; i++) {
    nb_score[i].resize(ny);
    int nbx = G1[x][i];
    for (int j = 0; j < ny; j++) {
      int nby = G2[y][j];
      nb_score[i][j] = (min((double)G1[nbx].size(), (double)G2[nby].size())
                     + min((double)RG1[nbx].size(), (double)RG2[nby].size()))
                     / (max((double)G1[nbx].size(), (double)G2[nby].size())
                     + max((double)RG1[nbx].size(), (double)RG2[nby].size()))
                     * (1 - BETA) + BETA;
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

static double MaxMatchAlpha(int x, int y, const SSimMat &sim_score, const Graph &G1, const Graph &G2) {
  size_t nx = G1[x].size();
  size_t ny = G2[y].size();
  SimMat nb_score;
  nb_score.resize(nx);
  for (int i = 0; i < nx; i++) {
    nb_score[i].resize(ny);
    int nbx = G1[x][i];
    for (int j = 0; j < ny; j++) {
      int nby = G2[y][j];
      if (sim_score[nbx].find(nby) != sim_score[i].end())
        nb_score[i][j] = sim_score[nbx].find(nby)->second;
      else nb_score[i][j] = BETA;
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

static void *BaselineThread(void *paramIn){
  struct baseline_thread_params *param = (struct baseline_thread_params *)paramIn;
  int id = param->id;
  const SimMat &sim_score = *(param->sim_score);
  SimMat &new_score = *(param->new_score);

  for (int i = id; i <= n1; i += MAX_THREAD){
    for (int j = 1; j <= n2; j++) {
      new_score[i][j] = MaxMatch(i, j, sim_score, G1, G2);
    }    
  }
  return NULL;
}

static void IterateBaseline(const SimMat &sim_score, SimMat &new_score) {
  struct baseline_thread_params params[MAX_THREAD];
  for (int i = 0; i < MAX_THREAD; i++){
    params[i].id = i + 1;
    params[i].sim_score = &sim_score;
    params[i].new_score = &new_score;
  }
  for (int i = 0; i < MAX_THREAD; i++){
    pthread_create(&threads[i], NULL, BaselineThread, (void *)&params[i]);
  }
	for (int i = 0; i < MAX_THREAD; i++) {
    pthread_join(threads[i], NULL);
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

static void *RoleSimPlusThread(void *paramIn){
  struct rolesimplus_thread_params *param = (struct rolesimplus_thread_params *)paramIn;
  int id = param->id;
  const SimMat &sim_score = *(param->sim_score);
  SimMat &new_score = *(param->new_score);

  for (int i = id; i <= n1; i += MAX_THREAD){
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
  return NULL;
}


static void IterateRoleSimPlus(const SimMat &sim_score, SimMat &new_score) {
  struct rolesimplus_thread_params params[MAX_THREAD];
  for (int i = 0; i < MAX_THREAD; i++){
    params[i].id = i + 1;
    params[i].sim_score = &sim_score;
    params[i].new_score = &new_score;
  }
  for (int i = 0; i < MAX_THREAD; i++){
    pthread_create(&threads[i], NULL, RoleSimPlusThread, (void *)&params[i]);
  }
	for (int i = 0; i < MAX_THREAD; i++) {
    pthread_join(threads[i], NULL);
  }
}

static void *AlphaRoleSimThread(void *paramIn){
  struct alpha_thread_params *param = (struct alpha_thread_params *)paramIn;
  int id = param->id;
  const SSimMat &sim_score = *(param->sim_score);
  SSimMat &new_score = *(param->new_score);

  for (int i = id; i <= n1; i += MAX_THREAD){
    new_score[i].clear();
    // Find highest score
    double tmp_max = 0;
    for (SVec::const_iterator node = sim_score[i].begin(); node != sim_score[i].end(); node++){
      if (node->second > tmp_max)
        tmp_max = node->second;
    }

    double theta = tmp_max * ALPHA;
    for (SVec::const_iterator node = sim_score[i].begin(); node != sim_score[i].end(); node++) {
      int j = node->first;
      if (node->second > theta)
        new_score[i][j] = (MaxMatchAlpha(i, j, sim_score, G1, G2)
                        + MaxMatchAlpha(i, j, sim_score, RG1, RG2))
                        / (max((double)G1[i].size(), (double)G2[j].size())
                        + max((double)RG1[i].size(), (double)RG2[j].size()))
                        * (1 - BETA) + BETA;
    }
  }
  return NULL;
}

static void IterateAlphaRoleSim(const SSimMat &sim_score, SSimMat &new_score) {
  struct alpha_thread_params params[MAX_THREAD];
  for (int i = 0; i < MAX_THREAD; i++){
    params[i].id = i + 1;
    params[i].sim_score = &sim_score;
    params[i].new_score = &new_score;
  }
  for (int i = 0; i < MAX_THREAD; i++){
    pthread_create(&threads[i], NULL, AlphaRoleSimThread, (void *)&params[i]);
  }
	for (int i = 0; i < MAX_THREAD; i++) {
    pthread_join(threads[i], NULL);
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
void CalcSimilarity(algo_iter ai) {
  switch (ai) {
    case VOID_ITER: {
      InitBaseline();
      break;
    }
    case BASELINE_ITER: {
      InitBaseline();
      //PrintMatrix(sim_score[0]);
      for (int i = 0; i < ITER_NUM; i++) {
        int old = i & 0x1;
        printf("iteration %d\n", i);
        IterateBaseline(sim_score[old], sim_score[1 - old]);
        //PrintMatrix(sim_score[1 - old]);
      }
      break;
    }
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
      timer t;
      t.start();
      InitAlphaRoleSim();
      t.end();
      printf("Init finished using %f sec\n", t.delta);
      //PrintMatrix(sim_score[0]);
      for (int i = 0; i < ITER_NUM; i++) {
        int old = i & 0x1;
        printf("iteration %d\n", i + 1);
        t.start();
        IterateAlphaRoleSim(ssim_score[old], ssim_score[1 - old]);
        t.end();
        printf("iteration %d finished using %f sec\n", i + 1, t.delta);
        //PrintMatrix(sim_score[1 - old]);
        /*
        for (int j = 1; j <= n1; j++){
          printf("%lu ", ssim_score[1 - old][j].size());
        }
        printf("\n");
        */
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
    default:
      assert(0);
  }
  //PrintMatrix(sim_score[ITER_NUM & 0x1]);
  //if (ai != VOID_ITER)
  //  OutputMatrix(sim_score[ITER_NUM & 0x1]);
}