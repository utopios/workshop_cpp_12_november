#include <iostream>
#include <iostream>
#include <vector>
#include <ranges>

//import math_operations;

//1. concepts
template <typename T>
concept Integral = std::is_integral_v<T>;


template <Integral T>
T add(T a, T b) {
    return a+b;
}

//2. spaceship operator
struct Point {
    int x, y;
    auto operator<=>(const Point& point) const  {
        if(point.x < x) {
            return std::partial_ordering::greater;
        } else if(point.x > x){
            return std::partial_ordering::less;
        } else {
            return std::partial_ordering::equivalent;
        }
    };
};


//3. constexpr function
constexpr int square(int x) {
    return x * x;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    auto result = add(10, 30);
    std::cout << result << '\n';
    //2. spaceship operator
    Point p1{x:20, y:30};
    Point p2{x:20, y:30};

    std::cout << (p1 < p2) << '\n';

    constexpr int value = square(30);
    std::cout << value << '\n';

    //4.ranges
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto even_square = numbers
                       | std::views::filter([](int x) {return x % 2 == 0;})
                       | std::views::transform([](int x)  {return  x*x;});

    for(const int& value : even_square) {
        std::cout << value << '\n';
    }
    return 0;
}

