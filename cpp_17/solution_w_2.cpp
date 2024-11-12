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

void exercise1()
{
    const auto v = std::vector<int>(100000000, 1);
    {
        const auto tic = now();
        for_each(std::execution::seq, v.begin(), v.end(), [](int x){ return x++; });
        const auto toc = now();
        std::cout << ms(toc - tic) << "\n";
    }
    {
        const auto tic = now();
        for_each(std::execution::par, v.begin(), v.end(), [](int x){ return x++; });
        const auto toc = now();
        std::cout << ms(toc - tic) << "\n";
    }
}

void exercise2()
{
    const auto c = makeRand();

    {
        auto v = c;
        const auto tic = now();
        std::sort(std::execution::seq, v.begin(), v.end());
        const auto toc = now();
        std::cout << ms(toc - tic) << "\n";
    }
    {
        auto v = c;
        const auto tic = now();
        std::sort(std::execution::par, v.begin(), v.end());
        const auto toc = now();
        std::cout << ms(toc - tic) << "\n";
    }
}


int main() {
    exercise1();
    exercise2();
    return 0;
}