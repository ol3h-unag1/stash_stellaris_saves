#include <iostream>
#include <string>
#include <format>
#include <vector>
#include <exception>
#include <filesystem>


#include <sys/inotify.h>
#include <unistd.h>
#include <unistd.h>   // For fork(), _exit(), and close()
#include <signal.h>   // For kill() to terminate processes
#include <cstring>

#include "index.hpp"

namespace StashSaves::Component
{

namespace Index_v1 
{
	Index::Index(fs::path dir_to_watch, std::string socket) 
		: _directory(std::move(dir_to_watch))
		, _socket(std::move(socket))
		, _child_pid(-1) {
	
	}

    Index::~Index() {
        if (_child_pid > 0) {
			std::cerr << std::format("Terminating child process with PID: {} in function {} line {}", _child_pid, __func__, __LINE__) << std::endl;
            if (kill(_child_pid, SIGTERM) == -1) {
				std::cerr << std::format("Failed to terminate child process in function {} line {}", __func__, __LINE__) << std::endl;
            }
        }
    }

	void Index::watch_dir() {

	    _child_pid = fork();

	    if (_child_pid < 0) {
			throw std::runtime_error(std::format("Failed to fork: {} in function {} line {}", std::strerror(errno), __func__, __LINE__));
	    }

	    if (_child_pid == 0) { // Child process
	        try {
	            watch_dir_impl(); // Call the implementation
	            _exit(0);         // Exit successfully
	        } catch (const std::exception& e) {
				std::cerr << std::format("Child process exception: {} in function {} line {}", e.what(), __func__, __LINE__) << std::endl;
	            _exit(1); // Exit with error
	        }
	    } else { // Parent process
	        try {
				std::cerr << std::format("Forked child process with PID: {} in function {} line {}", _child_pid, __func__, __LINE__) << std::endl;
	        } catch (std::exception& e) {
				std::cerr << std::format("Index::watch_dir_impl Error: {} in function {} line {}", e.what(), __func__, __LINE__) << std::endl;
	        }
	    }
	}

	// inotify implementation
	void Index::watch_dir_impl() {

	    // Initialize inotify
	    int inotify_fd = inotify_init();
	    if (inotify_fd < 0) {
			throw std::runtime_error(std::format("Failed to initialize inotify: {} in function {} line {}", std::strerror(errno), __func__, __LINE__));
	    }

	    // Add a watch for the specified directory
	    int watch_descriptor = inotify_add_watch(inotify_fd, _directory.string().c_str(),
	                                             IN_CREATE | IN_DELETE );
	    if (watch_descriptor == -1) {
	        close(inotify_fd);
			throw std::runtime_error(std::format("Failed to add inotify watch: {} in function {} line {}", std::strerror(errno), __func__, __LINE__));
	    }

	    const size_t buffer_size = 1024 * (sizeof(inotify_event) + 16);
	    std::vector<char> buffer(buffer_size);

	    std::cerr << "Index::watch_dir_impl() Monitoring directory: " << _directory << '\n';

	    try {
	        while (true) {
	            int length = read(inotify_fd, buffer.data(), buffer.size());
	            if (length < 0) {
					throw std::runtime_error(std::format("Error reading inotify events: {} in function {} line {}", std::strerror(errno), __func__, __LINE__));
	            }

	            int i = 0;
	            while (i < length) {
	                inotify_event* event = reinterpret_cast<inotify_event*>(&buffer[i]);

					std::cerr << "Index::watch_dir_impl() Event: " << event->wd << " mask: " << event->mask 
						      << " cookie: " << event->cookie << " len: " << event->len << std::endl;
	                
					// Handle different types of events
	                if (event->mask & IN_CREATE) 
					{
	                    std::cerr << "Index::watch_dir_impl() File created: " << event->name << std::endl;
						//throw std::runtime_error("Created");
	                } 
					else if (event->mask & IN_DELETE) 
					{
	                    std::cerr << "Index::watch_dir_impl() File deleted: " << event->name << std::endl;
						//throw std::runtime_error("Deleted");
	                } 
					else
					{
						std::cerr << "Unreachable. Unsupported event, wd: " << event->wd << " mask: " << event->mask << std::endl;
					}

	                i += sizeof(inotify_event) + event->len;
	            }
	        }
	    } catch (...) {
	        // Clean up before propagating the exception
	        inotify_rm_watch(inotify_fd, watch_descriptor);
	        close(inotify_fd);
	        throw; // Re-throw the exception
	    }

	    // Clean up
	    inotify_rm_watch(inotify_fd, watch_descriptor);
	    close(inotify_fd);
	}


} // end of namespace v1

namespace Index_v2 
{

namespace details
{

	void inotify_impl(fs::path dir_to_watch, Index::CallbackType callback) {

	    // Initialize inotify
	    int inotify_fd = inotify_init();
	    if (inotify_fd < 0) {
			throw std::runtime_error(std::format("Failed to initialize inotify: {} in function {} line {}", std::strerror(errno), __func__, __LINE__));
	    }

	    // Add a watch for the specified directory
	    int watch_descriptor = inotify_add_watch(inotify_fd, dir_to_watch.string().c_str(), IN_CREATE /* | IN_DELETE | ... */);
	    if (watch_descriptor == -1) {
	        close(inotify_fd);
			throw std::runtime_error(std::format("Failed to add inotify watch: {} in function {} line {}", std::strerror(errno), __func__, __LINE__));
	    }

	    const size_t buffer_size = 1024 * (sizeof(inotify_event) + 16);
	    std::vector<char> buffer(buffer_size);

	    std::cerr << "Index::watch_dir_impl() Monitoring directory: " << dir_to_watch << '\n';

	    try {
	        while (true) {
	            int length = read(inotify_fd, buffer.data(), buffer.size());
	            if (length < 0) {
					throw std::runtime_error(std::format("Error reading inotify events: {} in function {} line {}", std::strerror(errno), __func__, __LINE__));
	            }

	            int i = 0;
	            while (i < length) {
	                inotify_event* event = reinterpret_cast<inotify_event*>(&buffer[i]);

					std::cerr << "inotify_impl Event: " << event->wd << " mask: " << event->mask 
						      << " cookie: " << event->cookie << " len: " << event->len << std::endl;
	                
					// Handle different types of events
	                if (event->mask & IN_CREATE) 
					{
	                    std::cerr << std::this_thread::get_id() << ": inotify_impl File created: " << event->name << std::endl;
						callback(fs::path{event->name});
						//throw std::runtime_error("Created");
	                } 
					else
					{
						std::cerr << "Unreachable. Unsupported event, wd: " << event->wd << " mask: " << event->mask << std::endl;
					}

	                i += sizeof(inotify_event) + event->len;
	            }
	        }
	    } catch (...) {
	        // Clean up before propagating the exception
	        inotify_rm_watch(inotify_fd, watch_descriptor);
	        close(inotify_fd);
	        throw; // Re-throw the exception
	    }

	    // Clean up
	    inotify_rm_watch(inotify_fd, watch_descriptor);
	    close(inotify_fd);
	}

} // end of namespace details


	Index::Index(fs::path dir_to_watch, CallbackType callback)
		: _directory(std::move(dir_to_watch)) 
		, _callback(std::move(callback)) {
	
	}

	Index::~Index() {

		if (_thread.joinable()) 
		{
			_thread.join();
		}
	}

	
	void Index::watch_dir() {

		std::cout << std::format("watching dir{} at {}:{}", _directory.string(),  __func__, __LINE__) << std::endl;
		_thread = std::thread(details::inotify_impl, _directory, _callback);

	}

} // end of namespace v2

} // end of namespace StashSaves::Component