/* 
 * hungry_squirrel.cpp 
 * Univeristy of Michigan, Ann Arbor
 * EECS 281 F19E2 Q25.
 * SUBMIT ONLY THIS FILE TO GRADESCOPE.
 */

// Common #includes for convience.
// No need to use them. 
// Remove any of them if you want.
#include <algorithm>
#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "hungry_squirrel.h"

using namespace std;

int get_max_nuts(std::vector<std::vector<int>>& diag, int nrow, int ncol) {
  // TODO
  vector<vector<int>> memo;
  memo.resize(nrow, vector<int>(ncol, 0));
  //base case
  for (int i = 0; i < nrow; i++) memo[i][0] = diag[i][0];

  for (int i = 0; i < nrow; i++){
    for (int j = 1; j < ncol; j++){
      int rightUp = 0, rightDown = 0, right =0;
      if (i != nrow) rightUp = memo[i + 1][j - 1] + diag[i][j];
      if (i != 0) rightDown = memo[i - 1][j - 1] + diag[i][j];
      right = memo[i][j - 1] + diag[i][j];
      memo[i][j] = max(max(right, rightUp), rightDown);
    }
  }

  return *max_element(memo.back().begin(), memo.back().end());
}
