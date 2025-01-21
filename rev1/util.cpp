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

namespace StashSaves::Util
{


bool v1::is_directory_exists(const std::string& path) {

    return fs::exists(path) && fs::is_directory(path);
}

namespace v1 {
#define DEBUG_GET_CURRENT_USERNAME 1

namespace {

#ifdef _WIN32
std::string get_windows_username() {
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserNameA(username, &username_len)) {
        return std::string(username);
    }
    return "";
}
#elif __linux__
bool is_running_under_wsl() {
    std::ifstream proc_version("/proc/version");
    std::string proc_line_content;
    if (proc_version.is_open()) {
        std::getline(proc_version, proc_line_content);
        proc_version.close();
        
        std::transform(proc_line_content.begin(), proc_line_content.end(), proc_line_content.begin(), ::toupper);
        
        std::vector<std::string> clues_words = {"MICROSOFT", "WSL"};
        std::vector<std::string> words;
        std::string word;
        for (char c : proc_line_content) {
            if (std::isalpha(c)) {
                word += c;
            } else if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        if (!word.empty()) {
            words.push_back(word);
        }

        auto it = std::find_first_of(words.begin(), words.end(), clues_words.begin(), clues_words.end());
        return (it != words.end());
    }
    return false;
}

std::string get_wsl_windows_username() {
    const char* windows_username = getenv("USERPROFILE");
    if (windows_username != nullptr) {
        std::string user_profile(windows_username);
        auto pos = user_profile.find("\\Users\\");
        if (pos != std::string::npos) {
            return user_profile.substr(pos + 7, user_profile.find("\\", pos + 7) - (pos + 7));
        }
    }
    return "Unknown_Windows_User";
}

std::string get_linux_username() {
    if (const char* username = getenv("USER"); username != nullptr) {
        return std::string(username);
    } else if (struct passwd* pw = getpwuid(getuid()); pw != nullptr) {
        return std::string(pw->pw_name);
    }
    return "";
}
#endif

} // anonymous namespace

fs::path get_current_username() {
#ifdef _WIN32
    return get_windows_username();
#elif __linux__
    if (is_running_under_wsl()) {
        return get_wsl_windows_username();
    }
    return get_linux_username();
#else
    return ""; // Unsupported platform
#endif
}

} // end namespace StashSaves::Util::v1


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
