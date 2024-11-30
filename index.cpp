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
                std::cout << "File created: " << event->name << '\n';
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

[[nodiscard]] fs::path get_executable_directory() {
    char exe_path[1024];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        throw std::runtime_error("Unable to determine executable path.");
    }
    exe_path[len] = '\0'; // Null-terminate the path
    return fs::path(exe_path).parent_path();
}


int main() 
{
    
    auto directory = get_executable_directory();

    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) 
    {
        std::cerr << "Error: Directory does not exist or is not a directory: " << directory << '\n';
        return 1;
    }

    monitor_directory(directory);

    return 0;
}
