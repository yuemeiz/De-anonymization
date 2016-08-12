#include "utility.h"

using namespace std;

int main() {
	algo a = ROLESIM_PLUS;
	PreprocessGraph(a);
	CalcSimilarity(a);
	MatchGraph(a);
	return 0;
}