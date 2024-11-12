#include <iostream>
#include <coroutine>
#include <string>
#include <thread>
#include <chrono>
#include <memory>

// Awaitable pour simuler un délai asynchrone
struct Awaitable {
    int delay;
    std::string  source;
    Awaitable(int d, std::string s) : delay(d), source(s) {}

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> handle) const {
        std::thread([handle, delay = this->delay]() {
            //Appel Request
            std::this_thread::sleep_for(std::chrono::seconds(delay));  // Simule un délai
            handle.resume();
        }).detach();
    }
    void await_resume() const noexcept {}
};

// Coroutine de base `fetch_data` pour récupérer des données asynchrones
struct AsyncTask {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    std::shared_ptr<handle_type> coro;

    AsyncTask(handle_type h) : coro(std::make_shared<handle_type>(h)) {}
    ~AsyncTask() { if (coro && coro->done()) coro->destroy(); }

    struct promise_type {
        auto get_return_object() { return AsyncTask{handle_type::from_promise(*this)}; }
        auto initial_suspend() { return std::suspend_never{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        void return_void() {}
        void unhandled_exception() { std::exit(1); }
    };
};

// Coroutine `fetch_data` pour simuler une requête réseau avec délai
AsyncTask fetch_data(const std::string& source) {
    std::cout << "Démarrage de la récupération de données depuis : " << source << std::endl;
    co_await Awaitable{2, source};  // Attente simulée de 2 secondes
    std::cout << "Données reçues de " << source << std::endl;
}

int main() {
    fetch_data("Source1");
    std::this_thread::sleep_for(std::chrono::seconds(3));  // Maintenir le programme actif pour observer l'exécution
    return 0;
}