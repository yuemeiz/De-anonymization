#include "utility.h"

using namespace std;

int main(int argv, char *argc[]) {
  assert(argv == 4);
  algo_iter ai = (algo_iter) atoi(argc[1]);
  algo_match am = (algo_match) atoi(argc[2]);
  int do_interm = atoi(argc[3]);
  PreprocessGraph(ai, am);
  CalcSimilarity(ai);
  MatchGraph(am);
  if (do_interm == 1)
    EvalSimilarity(ai);
  return 0;
}