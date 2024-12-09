#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <filesystem>
#include <sys/inotify.h>
#include <unistd.h>
#include <cstring>

#include "index.hpp"

namespace StashSaves::Component
{

	v1::Index::Index(fs::path dir_to_watch, std::string socket) 
		: _directory(std::move(dir_to_watch))
		, _socket(std::move(socket)) {
	
	}

	void v1::Index::watch_dir() {

		try 
		{
			watch_dir_impl();
		}
		catch(std::exception& e)
		{
			std::cout << "Index::watch_dir_impl Error: " << e.what() << std::endl;
		}
		catch(...)
		{
			throw;
		}

	}

	// inotify implementation
	void v1::Index::watch_dir_impl() {

	    // Initialize inotify
	    int inotify_fd = inotify_init();
	    if (inotify_fd < 0) {
	        throw std::runtime_error("Failed to initialize inotify: " + std::string(strerror(errno)));
	    }

	    // Add a watch for the specified directory
	    int watch_descriptor = inotify_add_watch(inotify_fd, _directory.string().c_str(),
	                                             IN_CREATE | IN_DELETE | IN_MODIFY);
	    if (watch_descriptor == -1) {
	        close(inotify_fd);
	        throw std::runtime_error("Failed to add inotify watch: " + std::string(strerror(errno)));
	    }

	    const size_t buffer_size = 1024 * (sizeof(inotify_event) + 16);
	    std::vector<char> buffer(buffer_size);

	    std::cout << "Monitoring directory: " << _directory << '\n';

	    try {
	        while (true) {
	            int length = read(inotify_fd, buffer.data(), buffer.size());
	            if (length < 0) {
	                throw std::runtime_error("Error reading inotify events: " + std::string(strerror(errno)));
	            }

	            int i = 0;
	            while (i < length) {
	                inotify_event* event = reinterpret_cast<inotify_event*>(&buffer[i]);

	                // Handle different types of events
	                if (event->mask & IN_CREATE) {
	                    std::cout << "File created: " << event->name << '\n';
	                }

	                if (event->mask & IN_DELETE) {
	                    std::cout << "File deleted: " << event->name << '\n';
	                }

	                if (event->mask & IN_MODIFY) {
	                    std::cout << "File modified: " << event->name << '\n';
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


} // end of namespace StashSaves::Component