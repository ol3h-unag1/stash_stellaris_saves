#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <algorithm>    // std::transform, std::find_first_of
#include <vector>       // std::vector
#include <optional>     // std::optional
#include <cctype>       // std::isalpha

#include "plat_id.hpp"

namespace StashSaves::PlatformIdentity {

namespace details
{

bool is_running_under_wsl() {

    std::ifstream proc_version("/proc/version"); // std::ifstream
    std::string proc_line_content;
    if (proc_version.is_open()) {
        std::getline(proc_version, proc_line_content); // std::getline
        proc_version.close();
        
        std::transform(proc_line_content.begin(), proc_line_content.end(), proc_line_content.begin(), ::toupper); // std::transform
        
        std::vector<std::string> clues_words = {"MICROSOFT", "WSL"}; // std::vector
        std::vector<std::string> words; // std::vector
        std::string word;
        for (char c : proc_line_content) {
            if (std::isalpha(c)) { // std::isalpha
                word += c;
            } else if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        if (!word.empty()) {
            words.push_back(word);
        }

        auto it = std::find_first_of(words.begin(), words.end(), clues_words.begin(), clues_words.end()); // std::find_first_of
        return (it != words.end());
    }
    return false;
}

std::string enum_to_string(E_Platform platform) {

    switch (platform) {
        case E_Platform::Android: return "Android";
        case E_Platform::Emscripten: return "Emscripten";
        case E_Platform::Fuchsia: return "Fuchsia";
        case E_Platform::Linux: return "Linux";
        case E_Platform::POSIX: return "POSIX";
        case E_Platform::Unknown: return "Unknown";
        case E_Platform::WSL: return "WSL";
        case E_Platform::WSL2: return "WSL2";
        case E_Platform::Windows: return "Windows";
        case E_Platform::macOS: return "macOS";
        default: return "E_Platform::Unknown";
    }

    return "E_Platform::Unknown Unreachable";
}

} // namespace StashSaves::PlatformIdentity::details

std::optional<PlatformId> get_platform() { // std::optional

#ifdef _WIN32
    return PlatformId{E_Platform::Windows, details::enum_to_string(E_Platform::Windows)};
#elif __linux__
    if (details::is_running_under_wsl()) {
        return PlatformId{E_Platform::WSL, details::enum_to_string(E_Platform::WSL)};
    }
    return PlatformId{E_Platform::Linux,  details::enum_to_string(E_Platform::Linux)};
#else
    return {}; // Unsupported platform
#endif

}

} // namespace StashSaves::PlatformIdentity

// int main() {
//     auto platform = StashSaves::PlatformIdentity::get_platform();
//     if (platform) {
//         std::cout << "Running on platform: " << platform->name << std::endl;
//     } else {
//         std::cout << "Unsupported platform" << std::endl;
//     }
//     return 0;
// }