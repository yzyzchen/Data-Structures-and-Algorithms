#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <limits>

using namespace std;


int knapsack(int cap, vector<int> value, vector<int> weight)
{
    if (value.size() == 0) return 0;
    //initialize
    vector<vector<int>> memo;
    memo.resize(value.size() + 1, vector<int>(cap, 0));
    //
    for (size_t i = 1; i < cap; i++){
        for (size_t j = 1; i < cap; i++){
            if(j - weight[i - 1] > 0){
                memo[i][j] = max(memo[i - 1][j], memo[i - 1][j - weight[i - 1]] + value[i - 1]);
            }
        }
    }
    return memo[value.size()][cap];
    //buliding dp table

};