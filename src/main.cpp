#include "utility.h"

using namespace std;

int main(int argv, char *argc[]) {
  algo a = (algo) atoi(argc[1]);
  PreprocessGraph(a);
  CalcSimilarity(a);
  MatchGraph(a);
  return 0;
}