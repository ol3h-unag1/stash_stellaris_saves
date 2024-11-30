#include <sys/inotify.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <unistd.h>

namespace fs = std::filesystem;

void monitor_directory(const std::string& directory) {
    // Initialize inotify
    int inotify_fd = inotify_init();
    if (inotify_fd < 0) {
        perror("inotify_init");
        return;
    }

    // Add a watch for the specified directory
    int watch_descriptor = inotify_add_watch(inotify_fd, directory.c_str(),
                                             IN_CREATE | IN_DELETE | IN_MODIFY);
    if (watch_descriptor == -1) {
        perror("inotify_add_watch");
        close(inotify_fd);
        return;
    }

    const size_t buffer_size = 1024 * (sizeof(inotify_event) + 16);
    std::vector<char> buffer(buffer_size);

    std::cout << "Monitoring directory: " << directory << '\n';

    while (true) {
        int length = read(inotify_fd, buffer.data(), buffer.size());
        if (length < 0) {
            perror("read");
            break;
        }

        int i = 0;
        while (i < length) {
            inotify_event* event = reinterpret_cast<inotify_event*>(&buffer[i]);

            // Handle different types of events
            if (event->mask & IN_CREATE)
            {
                std::cout << "File created: " << event->name << '\n';
                // Vlad: call index_service_test with filename
                // for example you can reuse execute_terminal_command and pass
                // "./index_service_test usersave001.sav" it's argument 

            }

            if (event->mask & IN_DELETE)
                std::cout << "File deleted: " << event->name << '\n';
            if (event->mask & IN_MODIFY)
                std::cout << "File modified: " << event->name << '\n';

            i += sizeof(inotify_event) + event->len;
        }
    }

    // Clean up
    inotify_rm_watch(inotify_fd, watch_descriptor);
    close(inotify_fd);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <directory_to_monitor> <index_service_test_exe_path>\n";
        return 1;
    }

    std::string directory = argv[1];
    std::string index_service_test_exe_path = argv[2];

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        std::cerr << "Error: Directory does not exist or is not a directory: " << directory << '\n';
        return 1;
    }

    std::cout << "Directory to monitor: " << directory << '\n';
    std::cout << "Output message: " << index_service_test_exe_path << '\n';

    monitor_directory(directory);

    return 0;
}
