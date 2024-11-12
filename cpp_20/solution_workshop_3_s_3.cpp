#include <iostream>
#include <coroutine>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>

// Awaitable personnalisé pour simuler une attente asynchrone
struct Awaitable {
    bool await_ready() const noexcept { return false; }  // La coroutine n'est pas prête immédiatement
    void await_suspend(std::coroutine_handle<> handle) const {
        std::thread([handle]() {
            std::this_thread::sleep_for(std::chrono::seconds(2));  // Délai simulé de 2 secondes
            handle.resume();  // Reprend la coroutine après l'attente
        }).detach();
    }
    void await_resume() const noexcept {}  // Ne fait rien lors de la reprise
};

// Classe de gestion de coroutine avec un `promise_type`
struct AsyncTask {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    std::shared_ptr<handle_type> coro;  // Utilisation de shared_ptr pour une gestion de mémoire sécurisée

    AsyncTask(handle_type h) : coro(std::make_shared<handle_type>(h)) {}
    ~AsyncTask() { if (coro && coro->done()) coro->destroy(); }

    std::string get() { return coro->promise().result; }

    struct promise_type {
        std::string result;

        auto get_return_object() { return AsyncTask{handle_type::from_promise(*this)}; }
        auto initial_suspend() { return std::suspend_never{}; }  // Ne suspend pas immédiatement
        auto final_suspend() noexcept { return std::suspend_always{}; }  // Suspend après le retour

        void return_value(std::string value) { result = std::move(value); }
        void unhandled_exception() { std::exit(1); }
    };
};

// Coroutine pour simuler la récupération de données de manière asynchrone
AsyncTask fetch_data(const std::string& request) {
    std::cout << "Traitement de la requête : " << request << std::endl;
    co_await Awaitable{};  // Simule une attente non bloquante
    co_return "Données reçues pour : " + request;  // Retourne le résultat
}

int main() {
    std::vector<std::string> requests = {"Request1", "Request2", "Request3"};
    std::vector<AsyncTask> tasks;

    // Lance une coroutine pour chaque requête et stocke les tâches
    for (const auto& request : requests) {
        tasks.push_back(fetch_data(request));
    }

    // Attente des résultats pour chaque tâche
    for (auto& task : tasks) {
        std::cout << task.get() << std::endl;  // Récupère et affiche le résultat de chaque tâche
    }

    return 0;
}