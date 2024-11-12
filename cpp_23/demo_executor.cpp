#include <iostream>
#include <variant>
#include <expected>
#include <execution>
#include <thread>
#include <vector>
#include <string>
#include <concepts>
#include <queue>
#include <mutex>
#include <functional>
#include <condition_variable>

namespace std {
    namespace execution {

        class static_thread_pool {
        public:
            explicit static_thread_pool(size_t numThreads) {
                start(numThreads);
            }

            ~static_thread_pool() {
                stop();
            }

            template <typename F>
            void execute(F&& f) {
                std::thread(std::forward<F>(f)).detach();
            }
            void wait() {
                stop();
            }

        private:
            std::vector<std::thread> threads;
            std::queue<std::function<void()>> tasks;
            std::mutex queueMutex;
            std::condition_variable condition;
            bool stopPool = false;

            void start(size_t numThreads) {
                for (size_t i = 0; i < numThreads; ++i) {
                    threads.emplace_back([this] {
                        while (true) {
                            std::function<void()> task;
                            {
                                std::unique_lock<std::mutex> lock(this->queueMutex);
                                this->condition.wait(lock, [this] {
                                    return this->stopPool || !this->tasks.empty();
                                });

                                if (this->stopPool && this->tasks.empty()) {
                                    return;
                                }

                                task = std::move(this->tasks.front());
                                this->tasks.pop();
                            }
                            task();
                        }
                    });
                }
            }

            void stop() {
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    stopPool = true;
                }

                condition.notify_all();

                for (std::thread &thread : threads) {
                    if (thread.joinable()) {
                        thread.join();
                    }
                }
            }
        };
    }
}


int main() {
    std::execution::static_thread_pool pool(4);
    pool.execute([]() {
        std::cout << "Inside pool thread" << '\n';
    })
}
