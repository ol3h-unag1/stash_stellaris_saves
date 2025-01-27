#include <iostream>      // std::cout, std::endl
#include <format>        // std::format
#include <filesystem>    // std::filesystem::exists, std::filesystem::is_directory, std::filesystem::directory_iterator
#include <vector>        // std::vector
#include <algorithm>     // std::transform, std::find
#include <cctype>        // std::toupper

#include <cstdlib>       // getenv
#include <unistd.h>      // getuid
#include <pwd.h>         // getpwuid, passwd

#ifdef _WIN32
    #include <windows.h> // GetUserNameA
#elif __linux__
    #include <fstream>   // std::ifstream
    #include <sstream>   // std::getline
#endif

#include "util.hpp"
#include "plat_id.hpp"

namespace StashSaves::Util
{

bool v1::is_directory_exists(const std::string& path) {

    return fs::exists(path) && fs::is_directory(path);
}

std::vector<fs::path> v1::get_flat_subdirectories(const fs::path& dir) {

    std::vector<fs::path> subdirectories;
    // Use directory_iterator for non-recursive traversal
    for (const auto& entry : fs::directory_iterator(dir)) 
    {
        if (entry.is_directory()) 
        { // Check if the entry is a directory
            subdirectories.push_back(entry.path());
        }
    }
    
    return subdirectories;
}

} // end namespace StashSaves::Util
