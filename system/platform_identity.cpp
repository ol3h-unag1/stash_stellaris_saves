#include <iostream>   // std::cout
#include <format>     // std::format
#include <memory>     // std::shared_ptr, std::make_shared
#include <exception>  // std::exception
#include <fstream>    // std::ifstream
#include <string>     // std::string
#include <vector>     // std::vector
#include <algorithm>  // std::transform, std::find_first_of
#include <cctype>     // std::isalpha

#include "platform_identity.hpp"

namespace impl
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

auto get_platform()
try
{

using E_Platform_ID = StashSaves::PlatformIdentity::E_Platform_ID;

#ifdef _WIN32
    return E_Platform_ID::Windows;
#elif __linux__
    if (is_running_under_wsl()) {
        return E_Platform_ID::WSL;
    }
    return E_Platform_ID::Linux;
#else
    return E_Platform_ID::Unsupported; // Unsupported platform
#endif

}
catch(std::exception& e)
{
    std::cout << "Exception in function " << __func__ << ": " << e.what() << std::endl;
    return StashSaves::PlatformIdentity::E_Platform_ID::Error;
}
catch(...)
{
    std::cout << "Unknown exception in function " << __func__ << std::endl;
    return StashSaves::PlatformIdentity::E_Platform_ID::Error;
}

std::string to_string(StashSaves::PlatformIdentity::E_Platform_ID id) {

    using E_Platform_ID = StashSaves::PlatformIdentity::E_Platform_ID;

    switch (id)
    {
    case E_Platform_ID::Init:
        return "E_Platform_ID::Init";
    case E_Platform_ID::Unsupported:
        return "E_Platform_ID::Unsupported";
    case E_Platform_ID::Windows:
        return "E_Platform_ID::Windows";
    case E_Platform_ID::Linux:
        return "E_Platform_ID::Linux";
    case E_Platform_ID::WSL:
        return "E_Platform_ID::WSL";
    case E_Platform_ID::Error:
        return "E_Platform_ID::Error";
    default:
        return std::format("{} Unknown E_Platform_ID value: {}", __func__, static_cast<std::underlying_type_t<E_Platform_ID>>(id));
    }
}

} // end of impl namespace


namespace StashSaves
{

PlatformIdentity::PlatformIdentity() 
    : _platform_id{ impl::get_platform() } {
    
}

struct PlatformIdentity::Access final : public PlatformIdentity 
{
    Access() = default;
    ~Access() override = default;
};

std::shared_ptr<PlatformIdentity::Access> PlatformIdentity::instance() {

    static std::shared_ptr<PlatformIdentity::Access> instance{ std::make_shared<PlatformIdentity::Access>() };
    return instance;
}

auto PlatformIdentity::get_platform_id() const
{
    return _platform_id;
}

fs::path PlatformIdentity::get_current_username() const
{
    // Implementation to get the current username
    return "get_current_username";
}

fs::path PlatformIdentity::get_save_games_path() const
{
    // Implementation to get the save games path
    return "get_save_games_path";
}

fs::path PlatformIdentity::get_backup_path() const
{
    // Implementation to get the backup path
    return "get_backup_path";
}

fs::path PlatformIdentity::get_socket_path() const
{
    // Implementation to get the socket path
    return "get_socket_path";
}

} // end of namespace StashSaves 

int main()
{
    using namespace StashSaves;
    auto plat_id{ PlatformIdentity::instance() };

    std::cout << "Platform ID: " << impl::to_string(plat_id->get_platform_id()) << std::endl;

    return 0;
}