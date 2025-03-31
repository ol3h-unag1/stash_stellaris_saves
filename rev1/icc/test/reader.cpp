#include <iostream>
#include <format>
#include <vector>
#include <memory>
#include <mutex>

#include "reader.hpp"
#include "writer.hpp"

namespace Communication
{

namespace
{
std::mutex cout_mutex;
}

Reader::Reader()
{
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << std::format("{:5}Reader::Reader() - Initializing writers.", "") << std::endl;
    }
    init_writers();
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << std::format("{:5}Reader::Reader() - Writers initialized.", "") << std::endl;
    }
}

void Reader::init_writers()
{
    std::vector<std::unique_ptr<Writer>> writers;
    writers.emplace_back(std::make_unique<Writer>()); 
    writers.emplace_back(std::make_unique<Writer>()); 
    writers.emplace_back(std::make_unique<Writer>()); 

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << std::format("{:10}Reader::init_writers() - Before fork_children", "") << std::endl;
    }

    for (auto& writer : writers)
    {
        writer->fork_children();
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << std::format("******************************************************") << std::endl;
        }
        sleep(5);
    }

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << std::format("{:10}Reader::init_writers() - After fork_children", "") << std::endl;
    }

    sleep(1);
}

}

int main()
{
    std::cout << "READER.CPP::main()" << std::endl;

    Communication::Reader r;

    return 0;
}