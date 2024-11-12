#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <execution>
#include <iostream>
#include <vector>

auto now()
{
    return std::chrono::high_resolution_clock::now();
}

auto ms(auto duration)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

auto makeRand()
{
    std::vector<double> v;
    for (size_t i = 0; i < 10000000; ++i)
        v.push_back(rand());
    return v;
}



int main() {
    return 0;
}