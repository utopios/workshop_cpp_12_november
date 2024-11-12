#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <coroutine>
#include <thread>
#include <memory>
#include <curl/curl.h>

// Awaitable pour gérer le téléchargement asynchrone
class DownloadAwaitable {
public:
    DownloadAwaitable(const std::string& url, const std::string& output_path)
            : url_(url), output_path_(output_path) {}

    bool await_ready() const noexcept { return false; } // Toujours suspendre la coroutine
    void await_suspend(std::coroutine_handle<> handle) {
        // Lancer le téléchargement dans un thread séparé
        std::thread([this, handle]() {
            bool success = download_file();
            if (!success) {
                std::cerr << "Échec du téléchargement pour l'URL : " << url_ << std::endl;
            }
            handle.resume(); // Reprendre la coroutine après le téléchargement
        }).detach();

        /*bool success = download_file();
        if (!success) {
            std::cerr << "Échec du téléchargement pour l'URL : " << url_ << std::endl;
        }
        handle.resume();*/
    }
    void await_resume() const noexcept {
        // Peut gérer les erreurs ici si nécessaire
    }

private:
    bool download_file() {
        CURL* curl;
        CURLcode res;
        std::ofstream output_file(output_path_, std::ios::binary);

        if (!output_file.is_open()) {
            std::cerr << "Erreur : Impossible d'ouvrir le fichier en écriture : " << output_path_ << std::endl;
            return false;
        }

        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_file);
            res = curl_easy_perform(curl);

            curl_easy_cleanup(curl);

            if (res != CURLE_OK) {
                std::cerr << "Erreur lors du téléchargement : " << curl_easy_strerror(res) << std::endl;
                return false;
            }

            return true;
        }
        return false;
    }

    static size_t write_data(void* ptr, size_t size, size_t nmemb, void* userdata) {
        std::ofstream* stream = static_cast<std::ofstream*>(userdata);
        stream->write(static_cast<char*>(ptr), size * nmemb);
        return size * nmemb;
    }

    std::string url_;
    std::string output_path_;
};

// Classe de gestion de coroutine avec un `promise_type` et `std::shared_ptr`
struct AsyncTask {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    std::shared_ptr<handle_type> coro;

    AsyncTask(handle_type h) : coro(std::make_shared<handle_type>(h)) {}
    ~AsyncTask() {
        if (coro && coro->done()) coro->destroy();
    }

    void start() {
        if (coro && !coro->done()) {
            coro->resume();
        }
    }

    struct promise_type {
        auto get_return_object() {
            return AsyncTask{handle_type::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }

        void return_void() {}
        void unhandled_exception() { std::exit(1); }
    };
};

// Coroutine pour télécharger un fichier de manière asynchrone
AsyncTask download_file_async(const std::string& url, const std::string& output_path) {
    co_await DownloadAwaitable(url, output_path);
    std::cout << "Téléchargement terminé : " << output_path << std::endl;
}

int main() {
    // Initialiser libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    std::vector<std::string> urls = {
            "https://example.com/file1.zip",
            "https://example.com/file2.zip",
            "https://example.com/file3.zip"
    };
    std::vector<std::string> output_paths = {
            "file1.zip",
            "file2.zip",
            "file3.zip"
    };
    std::vector<AsyncTask> tasks;

    // Créer et démarrer les coroutines de téléchargement
    for (size_t i = 0; i < urls.size(); ++i) {
        AsyncTask task = download_file_async(urls[i], output_paths[i]);
        task.start();
        tasks.push_back(std::move(task));
    }



    // Attendre que tous les téléchargements soient terminés
    // Pour cet exemple, on utilise sleep, mais idéalement, vous utiliseriez une synchronisation appropriée
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Nettoyer libcurl
    curl_global_cleanup();

    return 0;
}