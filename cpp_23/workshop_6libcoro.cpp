#include <iostream>
#include <variant>
#include <vector>
#include <string>
#include <thread>
#include <random>
#include <coro/task.hpp>
#include <coro/sync_wait.hpp>
#include <chrono>

// Definition of sensor data types
using SensorData = std::variant<float, int, std::string>;

// Simulates asynchronous retrieval of sensor data using coroutines
coro::task<SensorData> fetchSensorDataAsync(int sensorID) {
    // Simulate delay
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Simulate different types of data based on sensor ID
    if (sensorID % 3 == 0) co_return 25.3f;  // Temperature
    if (sensorID % 3 == 1) co_return 1013;   // Pressure
    co_return std::string("OK");             // Operating state
}

// Function to process sensor data using std::variant and pattern matching
void processSensorData(const SensorData& data) {
    std::visit([](auto&& value) {
        using T = std::remove_cvref_t<decltype(value)>;
        if constexpr (std::is_same_v<T, float>) {
            std::cout << "Temperature: " << value << " °C\n";
        } else if constexpr (std::is_same_v<T, int>) {
            std::cout << "Pressure: " << value << " Pa\n";
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << "Operating state: " << value << "\n";
        } else {
            std::cout << "Unknown type\n";
        }
    }, data);
}

// Main function of the real-time data monitoring system with coroutines
coro::task<void> runSensorTasks(int sensorCount) {
    std::vector<coro::task<SensorData>> sensorTasks;

    // Retrieve sensor data asynchronously using coroutines
    for (int i = 0; i < sensorCount; ++i) {
        sensorTasks.push_back(fetchSensorDataAsync(i));
    }

    // Wait and process the results from all sensors
    for (int i = 0; i < sensorCount; ++i) {
        SensorData data = co_await sensorTasks[i];
        processSensorData(data);
    }
}

int main() {
    const int sensorCount = 10;

    // Run the sensor tasks and wait for their completion
    coro::sync_wait(runSensorTasks(sensorCount));

    std::cout << "Real-time data monitoring system completed.\n";

    return 0;
}