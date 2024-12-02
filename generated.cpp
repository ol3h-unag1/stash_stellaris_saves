#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <algorithm>
#include <execution> // For execution policies
#include <chrono>

class Monitor {
public:
    Monitor(std::size_t portion_size = 50) 
    : _stop(false)
    , _portion_size(portion_size) {

        // Start a background thread
        _monitorThread = std::thread(&Monitor::monitorFunction, this);
    }

    ~Monitor() {
        // Signal the monitor thread to stop and join it
        _stop.store(true);
        _cv.notify_all();
        if (_monitorThread.joinable()) {
            _monitorThread.join();
        }
    }

    // Add a filename to the vector
    void addFilename(const std::string& filename) {
        std::lock_guard<std::mutex> lock(_mtx);
        _filenames.push_back(filename);
        _cv.notify_all();
    }xza

private:
    std::size_t _portion_size;
    std::vector<std::string> _filenames; // Vector to store filenames
    std::mutex _mtx;                     // Mutex for synchronization
    std::condition_variable _cv;         // Condition variable for signaling
    std::thread _monitorThread;          // Background thread
    std::atomic<bool> _stop;             // Atomic stop flag

    // Background thread function
    void monitorFunction() {
        while (true) {
            std::unique_lock<std::mutex> lock(_mtx);
            // Wait for condition: vector size > 5 or stop flag
            _cv.wait(lock, [this] { return _filenames.size() > _portion_size || _stop.load(); });

            if (_stop.load()) {
                break; // Exit thread if stop flag is set
            }

            // Output the sorted vector contents using execution policy
            std::cout << "Set contains more than 5 elements:\n";
            std::sort(std::execution::par, _filenames.begin(), _filenames.end());
            for (const auto& filename : _filenames) {
                std::cout << filename << "\n";
            }
            _filenames.clear(); // Clear the vector after printing
        }
    }
};

int main() {
    Monitor monitor;

    // Simulate adding filenames to the Monitor
    monitor.addFilename("file01.txt");
    monitor.addFilename("file02.txt");
    monitor.addFilename("file03.txt");
    monitor.addFilename("file04.txt");
    monitor.addFilename("file05.txt");
    monitor.addFilename("file06.txt"); // This triggers output

    // Give time for the background thread to process
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Add more filenames to demonstrate further functionality
    monitor.addFilename("file07.txt");
    monitor.addFilename("file08.txt");
    monitor.addFilename("file09.txt");
    monitor.addFilename("file10.txt");
    monitor.addFilename("file11.txt");
    monitor.addFilename("file12.txt"); // This triggers another output

    // Give time for the background thread to process
    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
