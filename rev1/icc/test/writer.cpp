#include <iostream>
#include <format>
#include <unistd.h>
#include <mutex>

#include "writer.hpp"

namespace Communication
{

namespace 
{
std::mutex cout_mutex;
}

std::size_t Writer::object_counter = 0u;

Writer::Writer()
{
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        obj_id = object_counter++;
        std::cout << std::format("Writer::Writer: Object counter incremented to {} (Line: {})", object_counter, __LINE__) << std::endl;
    }
}

void Writer::fork_children()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << std::format("Writer object:{} Fork failed with error code: {} (Line: {})", obj_id, pid, __LINE__) << std::endl;
        return;
    }
    else if (pid == 0)
    {
        // Child process
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << std::format("Writer object:{} CHILD PROCESS with PID: {} | THIS CHILD Parent PID: {} (Line: {})", obj_id, getpid(), getppid(), __LINE__) << std::endl;
        }
    }
    else
    {
        // Parent process
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << std::format("Writer object:{} PARENT PROCESS with PID: {} (Line: {})", obj_id, getpid(), __LINE__) << std::endl;
        }
    }
}

}