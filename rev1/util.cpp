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

fs::path get_current_username() {

#ifdef _WIN32
    // Windows-specific code
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserNameA(username, &username_len)) {
#ifdef DEBUG_GET_CURRENT_USERNAME
        std::cout << std::format("Line {}: Windows username found: {}\n", __LINE__, username);
#endif
        return std::string(username);
    } else {
#ifdef DEBUG_GET_CURRENT_USERNAME
        std::cout << std::format("Line {}: Failed to get Windows username\n", __LINE__);
#endif
        return "";
    }
#elif __linux__

    // clues for if we working under WSL 
    std::vector<std::string> clues_words = {"MICROSOFT", "WSL"};

    // Check for WSL by reading /proc/version
    std::ifstream proc_version("/proc/version");
    std::string proc_line_content;
    if (proc_version.is_open()) {
        std::getline(proc_version, proc_line_content);
        
        std::transform(proc_line_content.begin(), proc_line_content.end(), proc_line_content.begin(), [](auto const& symbol) { 
            return std::toupper(symbol);
        });

#ifdef DEBUG_GET_CURRENT_USERNAME
        std::cout << std::format("Line {}: /proc/version content: {}\n", __LINE__, proc_line_content);
#endif
        proc_version.close();

        std::vector<std::string> words;
        std::string word;
        for (char c : proc_line_content) {
            if (std::isalpha(c)) {
                word += c;
            } else if (!word.empty()) {
                words.push_back(word);
                word.clear();
            } else {

            }
        }
        if (!word.empty()) {
            words.push_back(word);
        }

        auto it = std::find_first_of(words.begin(), words.end(), clues_words.begin(), clues_words.end());
        bool is_microsoft_wsl = (it != words.end());

        std::cout << "Words: ";
        for (const auto& w : words) {
            std::cout << w << " ";
        }
        std::cout << std::endl;

        std::cout << "Clues words: ";
        for (const auto& clue : clues_words) {
            std::cout << clue << " ";
        }
        std::cout << std::endl;

        if (is_microsoft_wsl) {
            // Running under WSL, determine Windows username
            const char* windows_username = getenv("USERPROFILE");
            if (windows_username != nullptr) {
                std::string user_profile(windows_username);
                auto pos = user_profile.find("\\Users\\");
                if (pos != std::string::npos) {
                    std::string wsl_username = user_profile.substr(pos + 7, user_profile.find("\\", pos + 7) - (pos + 7));
#ifdef DEBUG_GET_CURRENT_USERNAME
                    std::cout << std::format("Line {}: WSL Windows username found: {}\n", __LINE__, wsl_username);
#endif
                    return wsl_username;
                } else {
#ifdef DEBUG_GET_CURRENT_USERNAME
                    std::cout << std::format("Line {}: Unknown Windows user under WSL\n", __LINE__);
#endif
                    return "Unknown_Windows_User";
                }
            } else {
#ifdef DEBUG_GET_CURRENT_USERNAME
                std::cout << std::format("Line {}: Failed to get USERPROFILE under WSL\n", __LINE__);
#endif
                return "Unknown_Windows_User";
            }
        } else {
#ifdef DEBUG_GET_CURRENT_USERNAME
            std::cout << std::format("Line {}: Not running under WSL\n", __LINE__);
#endif
        }
    } else {
#ifdef DEBUG_GET_CURRENT_USERNAME
        std::cout << std::format("Line {}: Failed to open /proc/version\n", __LINE__);
#endif
    }
    // Native Linux
    if (const char* username = getenv("USER"); username != nullptr) {
#ifdef DEBUG_GET_CURRENT_USERNAME
        std::cout << std::format("Line {}: Native Linux username found: {}\n", __LINE__, username);
#endif
        return std::string(username);
    } else if (struct passwd* pw = getpwuid(getuid()); pw != nullptr) {
#ifdef DEBUG_GET_CURRENT_USERNAME
        std::cout << std::format("Line {}: Native Linux username found from passwd: {}\n", __LINE__, pw->pw_name);
#endif
        return std::string(pw->pw_name);
    } else {
#ifdef DEBUG_GET_CURRENT_USERNAME
        std::cout << std::format("Line {}: Failed to get Linux username\n", __LINE__);
#endif
        return "";
    }
#else
#ifdef DEBUG_GET_CURRENT_USERNAME
    std::cout << std::format("Line {}: Unsupported platform\n", __LINE__);
#endif
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
