#include <iostream>   // std::cout
#include <format>     // std::format
#include <memory>     // std::shared_ptr, std::make_shared
#include <exception>  // std::exception
#include <fstream>    // std::ifstream
#include <string>     // std::string
#include <vector>     // std::vector
#include <algorithm>  // std::transform, std::find_first_of
#include <cctype>     // std::isalpha

#include "platform_identity_data.hpp"

#include "platform_identity.hpp"
#include "path.hpp"

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

using E_Platform_ID = StashSaves::E_Platform_ID;

#ifdef _WIN32
    return E_Platform_ID::Windows;
#elif __linux__
    if (is_running_under_wsl()) {
        return E_Platform_ID::WSL;
    }
    return E_Platform_ID::Linux;
#else
    return E_Platform_ID::Unknown; // Unsupported platform
#endif

    return E_Platform_ID::Error; // Unreachable
}
catch(std::exception& e)
{
    std::cout << "Exception in function " << __func__ << ": " << e.what() << std::endl;
    return StashSaves::E_Platform_ID::Error;
}
catch(...)
{
    std::cout << "Unknown exception in function " << __func__ << std::endl;
    return StashSaves::E_Platform_ID::Error;
}


} // end of impl namespace


namespace StashSaves
{

#define DEBUG_GET_PLATFORM_ID

// real c-tor
PlatformIdentity::PlatformIdentity() 
    : _platform_id(impl::get_platform()) {

#ifdef DEBUG_GET_PLATFORM_ID
    std::cout << std::format("{}: init block platform_id: {}", "PlatformIdentity::PlatformIdentity()", to_string(_platform_id)) << std::endl;
    std::cout << std::format("{}: body block platform_id: {}", "PlatformIdentity::PlatformIdentity()", to_string(impl::get_platform())) << std::endl;
#endif
}

// mock c-tor
PlatformIdentity::PlatformIdentity(E_Platform_ID platform_id) 
    : _platform_id(platform_id) {
#ifdef DEBUG_GET_PLATFORM_ID
        std::cout << std::format("{}: init block platform_id: {}", "PlatformIdentity::PlatformIdentity()", to_string(_platform_id)) << std::endl;
        std::cout << std::format("{}: body block platform_id: {}", "PlatformIdentity::PlatformIdentity()", to_string(impl::get_platform())) << std::endl;
#endif
}

std::shared_ptr<PlatformIdentity::Access> PlatformIdentity::instance() {

    static std::shared_ptr<PlatformIdentity::Access> instance{ std::make_shared<PlatformIdentity::Access>() };
    return instance;
}

std::shared_ptr<PlatformIdentity::Access> PlatformIdentity::mock_instance() {

    static std::shared_ptr<PlatformIdentity::Access> instance{ std::make_shared<PlatformIdentity::Access>(E_Platform_ID::MockPlarformIdentity) };
    return instance;
}

#ifdef DEBUG_GET_PLATFORM_ID
auto PlatformIdentity::get_platform_id() const {

    return _platform_id;
}
#endif

fs::path PlatformIdentity::get_current_username() const {

    return Path::get_current_username(_platform_id);
}

fs::path PlatformIdentity::get_save_games_path() const {

    return Path::get_save_games_path(_platform_id);
}

fs::path PlatformIdentity::get_backup_path() const {

    return Path::get_backup_path(_platform_id);
}

fs::path PlatformIdentity::get_socket_path() const {

    return Path::get_socket_path(_platform_id);
}

PlatformIdentity::Access::Access(E_Platform_ID platform_id) 
    : PlatformIdentity(platform_id) 
{}
    
} // end of namespace StashSaves 

// int main()
// {
//     using namespace StashSaves;
//     auto plat_id{ PlatformIdentity::instance() };

//     std::cout << "Platform ID: " << to_string(plat_id->get_platform_id()) << std::endl;
//     std::cout << "Current Username: " << plat_id->get_current_username() << std::endl;
//     std::cout << "Save Games Path: " << plat_id->get_save_games_path() << std::endl;
//     std::cout << "Backup Path: " << plat_id->get_backup_path() << std::endl;
//     std::cout << "Socket Path: " << plat_id->get_socket_path() << std::endl;

//     return 0;
// }