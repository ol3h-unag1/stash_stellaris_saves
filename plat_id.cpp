#include <iostream>     // std::cout
#include <string>       // std::string
#include <fstream>      // std::ifstream
#include <algorithm>    // std::transform, std::find_first_of
#include <vector>       // std::vector
#include <optional>     // std::optional
#include <cctype>       // std::isalpha

namespace StashSaves::PlatformIdentity {

    enum class Platform {
        Android,
        Emscripten,
        Fuchsia,
        Linux,
        POSIX,
        Unknown,
        WSL,
        WSL2,
        Windows,
        macOS,
        Unsupported
    };

    std::string enum_to_string(Platform platform) {
        switch (platform) {
            case Platform::Android: return "Android";
            case Platform::Emscripten: return "Emscripten";
            case Platform::Fuchsia: return "Fuchsia";
            case Platform::Linux: return "Linux";
            case Platform::POSIX: return "POSIX";
            case Platform::Unknown: return "Unknown";
            case Platform::WSL: return "WSL";
            case Platform::WSL2: return "WSL2";
            case Platform::Windows: return "Windows";
            case Platform::macOS: return "macOS";
            default: return "Unknown";
        }
    }
}

using namespace StashSaves::PlatformIdentity;

struct PlatformId 
{
    Platform platform;
    std::string name;
};

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

}

std::optional<PlatformId> get_platform() { // std::optional

#ifdef _WIN32
    return PlatformId{Platform::Windows, enum_to_string(Platform::Windows)};
#elif __linux__
    if (details::is_running_under_wsl()) {
        return PlatformId{Platform::WSL, enum_to_string(Platform::WSL)};
    }
    return PlatformId{Platform::Linux, enum_to_string(Platform::Linux)};
#else
    return {}; // Unsupported platform
#endif

}

int main() {

    auto platform = get_platform();
    if (platform) {
        std::cout << "Running on platform: " << enum_to_string(platform->platform) << std::endl;
    } else {
        std::cout << "Unsupported platform" << std::endl;
    }
    return 0;
}