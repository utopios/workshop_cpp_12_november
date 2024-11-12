#include <iostream>
#include <string>
#include <variant>

template<typename T>
void displayMessage(const T& value) {
    if constexpr (std::is_same_v<T, int>) {
        std::cout << "It's an integer: " << value << std::endl;
    } else if constexpr (std::is_same_v<T, float>) {
        std::cout << "It's a float: " << value << std::endl;
    } else if constexpr (std::is_same_v<T, std::string>) {
        std::cout << "It's a string: " << value << std::endl;
    } else {
        std::cout << "Unknown type." << std::endl;
    }
}

int main() {
    displayMessage(10);
    displayMessage(10.10f);
    displayMessage(std::string("hello"));
    return 0;
}