#include <fstream>
#include <string>
#include <expected>
#include <iostream>

std::expected<std::string, std::string> load_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        return std::unexpected("Error: File not found");
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    if (content.empty()) {
        return std::unexpected("Error: File is empty or corrupted");
    }

    return content;
}

int main() {

    auto result = load_file("data.txt");
    if(result) {
        std::cout << "File content " << result.value() << '\n';
    }else {
        std::cout << result.error() << '\n';
    }
}