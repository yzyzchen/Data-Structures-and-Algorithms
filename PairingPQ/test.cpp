#include <algorithm>
#include <iostream>
#include <functional>
#include <iterator>
#include <vector>

using namespace std;

int main()
{
    std::vector<int> v = {10, 20, 5, 30, 50};

    std::make_heap(v.begin(), v.end());

    // Output the heap
    std::cout << "Heap after make_heap: ";
    for (int num : v) {
        std::cout << num << " ";
    }
    std::cout << "\n";

    return 0;
}