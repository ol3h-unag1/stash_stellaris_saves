#include <iostream>
#include <filesystem>
#include <vector>

#include <cstdlib>    // for getenv
#include <unistd.h>   // for getuid
#include <pwd.h>      // for getpwuid

#include "util.hpp"
namespace StashSaves::Util
{


bool v1::is_directory_exists(const std::string& path) {

    return fs::exists(path) && fs::is_directory(path);
}

fs::path v1::get_current_username() {

    if (const char* username = getenv("USER"); username != nullptr) 
    {
        return username;
    }

    if (struct passwd* pw = getpwuid(getuid()); pw != nullptr) 
    {
        return pw->pw_name;
    }
    return "";
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
