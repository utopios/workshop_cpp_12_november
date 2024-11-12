#include <iostream>
#include <filesystem>
#include <optional>
#include <variant>
#include <any>
#include <numeric>
#include <execution>
#include <chrono>
#include <tuple>
#include <vector>
#include <type_traits>
#include "globalvar.h"

// 1. Inline variable (C++17)
//inline const int global_value = 42;

// 2. if constexpr (C++17)
template <int N>
void checkValue() {
    if constexpr (N > 10) {
        std::cout << "N is greater than 10." << std::endl;
    } else {
        std::cout << "N is less than or equal to 10." << std::endl;
    }
}

// 3. std::optional (C++17)
template<typename T>
void withOptional(std::optional<T> element) {
    if (element) {
        std::cout << "Optional contains: " << *element << '\n';
    } else {
        std::cout << "Optional is empty.\n";
    }
}

// 4. std::variant with type trait checking and access functions
void withVariant(std::variant<int, double, std::string> element) {
    // Access and print the value inside the variant using std::visit
    std::visit([](auto&& arg) {
        std::cout << "Variant contains (via std::visit): " << arg << '\n';
        // Type checking using traits
        if constexpr (std::is_same<decltype(arg), int>().value) {
            std::cout << arg << "The type of the variant value is int.\n";
        } else if constexpr (std::is_same_v<decltype(arg), double>) {
            std::cout << "The type of the variant value is double.\n";
        } else if constexpr (std::is_same_v<decltype(arg), std::string>) {
            std::cout << "The type of the variant value is std::string.\n";
        }
    }, element);

    // Using std::get to access the value if you know the type in advance
    if (std::holds_alternative<int>(element)) {
        int value = std::get<int>(element);
        std::cout << "Variant contains (via std::get<int>): " << value << '\n';
    } else if (std::holds_alternative<double>(element)) {
        double value = std::get<double>(element);
        std::cout << "Variant contains (via std::get<double>): " << value << '\n';
    } else if (std::holds_alternative<std::string>(element)) {
        std::string value = std::get<std::string>(element);
        std::cout << "Variant contains (via std::get<std::string>): " << value << '\n';
    }

    // Using std::get_if to safely check and access the value
    if (auto intPtr = std::get_if<int>(&element)) {
        std::cout << "Variant contains (via std::get_if<int>): " << *intPtr << '\n';
    } else if (auto doublePtr = std::get_if<double>(&element)) {
        std::cout << "Variant contains (via std::get_if<double>): " << *doublePtr << '\n';
    } else if (auto stringPtr = std::get_if<std::string>(&element)) {
        std::cout << "Variant contains (via std::get_if<std::string>): " << *stringPtr << '\n';
    }
}

// 5. Fold expressions (C++17)
auto sum(auto... numbers) {
    constexpr std::size_t size = sizeof...(numbers);
    std::cout << "Number of elements: " << size << '\n';
    return (... + numbers); // Fold expression
}

// 6. Structured bindings (C++17)
void structuredBinding() {
    std::tuple<int, double, std::string> myTuple = {1, 3.14, "C++17"};
    auto [myInt, myDouble, myString] = myTuple;
    std::cout << "myInt: " << myInt << ", myDouble: " << myDouble << ", myString: " << myString << '\n';
}

// 7. std::filesystem (C++17)
void checkFileSystem() {
    std::filesystem::path path = "/tmp/test.txt";
    if (!std::filesystem::exists(path)) {
        std::cout << path << " does not exist.\n";
    }
}

// 8. std::any (C++17)
void demoAny() {
    std::any myAny = 10; // Any can hold any type
    try {
        std::cout << "myAny contains: " << std::any_cast<int>(myAny) << '\n';
    } catch (const std::bad_any_cast& e) {
        std::cout << "Bad cast: " << e.what() << '\n';
    }
}

// 9. Parallel algorithms (C++17)
void parallelAlgorithmsDemo() {
    const size_t dataSize = 1'000'000;  // One million elements
    std::vector<int> data(dataSize);

    // Initialize the data vector with values from 1 to dataSize
    std::iota(data.begin(), data.end(), 1);

    // Sequential sum (std::execution::seq)
    auto start = std::chrono::high_resolution_clock::now();
    long long seqSum = std::reduce(std::execution::seq, data.begin(), data.end(), 0LL);
    auto end = std::chrono::high_resolution_clock::now();
    auto seqDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Sequential sum: " << seqSum << " (Time: " << seqDuration << " ms)" << std::endl;

    // Parallel sum (std::execution::par)
    start = std::chrono::high_resolution_clock::now();
    long long parSum = std::reduce(std::execution::par, data.begin(), data.end(), 0LL);
    end = std::chrono::high_resolution_clock::now();
    auto parDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Parallel sum: " << parSum << " (Time: " << parDuration << " ms)" << std::endl;

    // Parallel unsequenced sum (std::execution::par_unseq)
    start = std::chrono::high_resolution_clock::now();
    long long parUnseqSum = std::reduce(std::execution::par_unseq, data.begin(), data.end(), 0LL);
    end = std::chrono::high_resolution_clock::now();
    auto parUnseqDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Parallel unsequenced sum: " << parUnseqSum << " (Time: " << parUnseqDuration << " ms)" << std::endl;
}

int main() {

    /*std::cout << global_var << '\n';

    // Demo 1: if constexpr
    checkValue<5>();  // N = 5, so the second branch runs
    checkValue<15>(); // N = 15, so the first branch runs

    // Demo 2: std::optional
    std::optional<int> myOptional = 34;
    withOptional(myOptional);

    std::optional<int> myOptional2;
    withOptional(myOptional2);
    // Demo 3: std::variant with additional functions
    std::variant<int, double, std::string> element = "C++17 is cool!";
    withVariant(element);
    withVariant(std::string("test"));
    // Demo 4: Fold expression
    /*std::cout << "Sum: " << sum(1, 2, 3, 4, 5) << '\n';

    // Demo 5: Structured binding
    structuredBinding();

    // Demo 6: std::filesystem
    checkFileSystem();

    // Demo 7: std::any
    demoAny();*/

    // Demo 8: Parallel algorithms
    parallelAlgorithmsDemo();



    return 0;
}
