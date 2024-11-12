#include <iostream>
#include <coroutine>
#include <string>
#include <thread>
#include <chrono>
#include <future>

struct Awaitable {
    int delay;
    std::promise<void> promise;
    std::shared_future<void> future;
    std::string result;

    Awaitable(int d)
            : delay(d),
              future(promise.get_future().share()),
              result("") {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) {
        std::thread([this, handle]() {
            std::this_thread::sleep_for(std::chrono::seconds(delay));
            this->result = std::to_string(delay);
            promise.set_value();
            handle.resume();
        }).detach();
    }

    std::string await_resume() noexcept {
        future.wait();
        return result;
    }
};

struct DataGenerator {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        std::string current_value;
        auto get_return_object() { return DataGenerator{handle_type::from_promise(*this)}; }
        auto initial_suspend() { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        auto yield_value(std::string value) {
            current_value = value;
            return std::suspend_always{};
        }
        void return_void() {}
        void unhandled_exception() { std::exit(1); }
    };

    std::shared_ptr<handle_type> coro;

    DataGenerator(handle_type h) : coro(std::make_shared<handle_type>(h)) {}
    ~DataGenerator() {  if (coro && coro->done()) coro->destroy(); }

    bool next() {
        if (coro) {
            coro->resume();
            return !coro->done();
        }
        return false;
    }

    std::string value() const {
        return coro->promise().current_value;
    }
};

DataGenerator fetch_data_chunks(const std::string& source) {
    for (int i = 1; i <= 3; ++i) {
        Awaitable awaitable_instance{i};
        std::string result = co_await awaitable_instance;
        co_yield "Chunk " + result + " from " + source;
    }
}

int main() {
    auto generator = fetch_data_chunks("Source1");
    while (generator.next()) {
        std::string data = generator.value();
        std::cout << data << std::endl;
    }
    return 0;
}