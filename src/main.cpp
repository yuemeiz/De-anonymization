#include "utility.h"

using namespace std;

int main(int argv, char *argc[]) {
  assert(argv == 5);
  algo_iter ai = (algo_iter) atoi(argc[1]);
  algo_match am = (algo_match) atoi(argc[2]);
  int do_interm = atoi(argc[3]);
  int overlap = atoi(argc[4]);
  PreprocessGraph(ai, am);
  printf("PreprocessGraph() finished.\n");
  CalcSimilarity(ai);
  if (do_interm == 1){
    EvalSimilarity(ai);
  }
  
  timer t;
  t.start();
  MatchGraph(am, overlap);
  t.end();
  printf("match phase using %f sec\n", t.delta);
  
  return 0;
}