#include <iostream>
#include <coroutine>
#include <thread>
#include <chrono>
#include <functional>

// Classe Awaitable pour simuler un délai asynchrone
class SimpleAwaitable {
public:
    bool await_ready() const noexcept {
        return false; // Toujours suspendre la coroutine
    }

    void await_suspend(std::coroutine_handle<> handle) const {
        // Lancer un thread séparé pour simuler un délai de 1 seconde
        std::thread([handle]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            handle.resume(); // Reprend la coroutine après le délai
        }).detach();
    }

    int await_resume() const noexcept {
        return 42; // Retourne un résultat fictif
    }
};

// Classe CoroutineTask avec une fonction de rappel (callback) pour notifier la fin
class CoroutineTask {
public:
    struct promise_type {
        // Type de la fonction de rappel
        using Callback = std::function<void(int)>;

        CoroutineTask get_return_object() {
            return CoroutineTask{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; } // Suspend la coroutine à la fin

        void return_value(int value) {
            result_ = value; // Enregistre le résultat
            if (callback) callback(result_); // Appelle le callback si défini
        }

        void unhandled_exception() {
            std::exit(1); // Gestion des exceptions non capturées
        }

        void set_callback(Callback cb) {
            callback = std::move(cb); // Définit le callback
        }

        int result_ = 0;
        Callback callback;
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit CoroutineTask(handle_type handle) : handle_(handle) {}
    ~CoroutineTask() { if (handle_) handle_.destroy(); }

    // Fonction pour définir le callback
    void set_callback(std::function<void(int)> cb) {
        handle_.promise().set_callback(std::move(cb));
    }

    bool resume() {
        if (!handle_.done()) {
            handle_.resume();
        }
        return !handle_.done();
    }

private:
    handle_type handle_;
};

// Fonction coroutine utilisant SimpleAwaitable
CoroutineTask my_coroutine() {
    std::cout << "Début de la coroutine\n";

    // Utilisation de co_await pour suspendre la coroutine
    int result = co_await SimpleAwaitable();

    std::cout << "Reprise de la coroutine avec result = " << result << "\n";

    // Valeur de retour de la coroutine
    co_return result + 10;
}

int main() {
    // Démarrer la coroutine
    CoroutineTask task = my_coroutine();

    // Définir un callback qui sera appelé une fois la coroutine terminée
    task.set_callback([](int result) {
        std::cout << "Résultat final de la coroutine : " << result << std::endl;
    });

    // Laisser le thread principal libre pour d'autres tâches
    std::cout << "Le thread principal continue de travailler...\n";

    // Simuler des travaux du thread principal
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Le thread principal a terminé son travail.\n";

    return 0;
}