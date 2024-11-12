#include <iostream>
#include <coroutine>
#include <memory>

// Classe Generator pour produire une séquence de valeurs
template<typename T>
class Generator {
public:
    struct promise_type {
        T current_value;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept { return {}; }

        std::suspend_always final_suspend() noexcept { return {}; }

        std::suspend_always yield_value(T value) noexcept {
            current_value = value;
            return {};
        }

        void return_void() {}

        void unhandled_exception() {
            std::exit(1); // Gérer les exceptions non capturées
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit Generator(handle_type handle) : handle_(handle) {}

    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    Generator(Generator&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    ~Generator() {
        if (handle_) handle_.destroy();
    }

    // Méthode pour obtenir la prochaine valeur générée
    bool next() {
        if (handle_) {
            handle_.resume();
            return !handle_.done();
        }
        return false;
    }

    T value() const {
        return handle_.promise().current_value;
    }

private:
    handle_type handle_;
};

// Fonction générateur utilisant co_yield
Generator<int> my_generator() {
    std::cout << "Début du générateur\n";
    for (int i = 0; i < 5; ++i) {
        co_yield i;
    }
    std::cout << "Fin du générateur\n";
}

int main() {
    auto gen = my_generator();

    while (gen.next()) {
        int value = gen.value();
        std::cout << "Valeur générée : " << value << "\n";
    }

    std::cout << "Générateur épuisé\n";

    return 0;
}