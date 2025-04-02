#include <algorithm>    // std::transform, std::remove, std::remove_if, std::replace
#include <cctype>       // std::isalpha
#include <cstdlib>      // getenv
#include <filesystem>   // std::filesystem::exists
#include <fstream>      // std::ifstream
#include <iostream>     // std::getline
#include <pwd.h>        // getpwuid, struct passwd
#include <sstream>      // std::istringstream
#include <string>       // std::string
#include <format>       // std::format
#include <unistd.h>     // getuid
#include <vector>       // std::vector

#ifdef _WIN32
#include <windows.h>    // GetUserNameA
#endif

#include "platform_identity_data.hpp"

#include "platform_identity.hpp"
#include "path.hpp"

namespace StashSaves::Path
{

namespace {

std::string get_windows_username() {
#ifdef UNLEN
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserNameA(username, &username_len)) {
        return std::string(username);
    }
#endif
    return "";
}
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

std::string get_host_filesystem_root() {

    std::string const wsl_conf("/etc/wsl.conf");
    std::ifstream file(wsl_conf);

    if (!file.is_open())
        return "";
    
    std::string automount_section = "[automount]";
    std::string root_key = "root";

    std::string line;
    bool in_automount_section = false;

    while (std::getline(file, line)) {
        if (line.find(automount_section) != std::string::npos) {
            in_automount_section = true;
            continue;
        }

        if (in_automount_section) {
            if (line.find('[') != std::string::npos) {
                break; // End of [automount] section
            }

            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                if (key == root_key)
                    return value;
            }
        }
    }

    return "";
}

std::string get_powershell_path(auto const& host_root) {

    std::vector<std::string> possible_paths = {
        host_root + "/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe",
        host_root + "/c/Windows/sysnative/WindowsPowerShell/v1.0/powershell.exe",
        host_root + "/c/Windows/System32/WindowsPowerShell/v1.0/pwsh.exe",
        host_root + "/c/Windows/sysnative/WindowsPowerShell/v1.0/pwsh.exe",
    };
    
    for (auto const& path : possible_paths) 
        if (std::filesystem::exists(path))
            return path;

    return "";
}

std::string get_wsl_windows_username() {
    
    auto powershell_exe = get_powershell_path(get_host_filesystem_root());
    if (powershell_exe.empty()) {
        return "";
    }

    std::string command = powershell_exe + " -Command \"[System.Security.Principal.WindowsIdentity]::GetCurrent().Name\"";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "";
    }

    char buffer[128]; // 128 bytes should be enough for username?
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    pclose(pipe);

    // Remove any trailing newline characters
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());

    // Check if the result contains any slashes or backslashes
    size_t pos = result.find_first_of("\\/");
    if (pos != std::string::npos) {
        // Convert to Linux path if needed
        std::replace(result.begin(), result.end(), '\\', '/');
        // Extract the last piece of the path
        result = result.substr(result.find_last_of('/') + 1);
    }

    return result;
}

std::string get_linux_username() {
    if (const char* username = getenv("USER"); username != nullptr) {
        return std::string(username);
    } else if (struct passwd* pw = getpwuid(getuid()); pw != nullptr) {
        return std::string(pw->pw_name);
    }
    return "";
}

std::string get_current_username_impl(E_Plat_ID const& plat_id) {
      
    if (E_Plat_ID::WSL == plat_id) 
    {
        return get_wsl_windows_username();
    }
    else if(E_Plat_ID::Windows == plat_id) 
    {
        return get_windows_username();
    }
    else if(E_Plat_ID::Linux == plat_id) 
    {
        return get_linux_username();
    }
    else
    {
        return std::format("{}: unsupported platform {}", __func__, to_string(plat_id));
    }

    return "get_current_username_impl: Unreachable";
}

} // anonymous namespace


fs::path get_current_username(E_Plat_ID const& plat_id) {

    auto current_user = get_current_username_impl(plat_id);
    current_user.erase(std::remove_if(current_user.begin(), current_user.end(), [](char c) {
        return c == '\\' || c == '/' || c == '\t' || c == '\n' || c == '\r';
    }), current_user.end());
    
    return current_user;
}

const char* G_game_path_linux = ".local/share/Paradox Interactive/Stellaris/save games/";
const char* G_game_path_win = "Documents/Paradox Interactive/Stellaris/save games/";
const char* G_game_path_mock = "StellarisMockDir/SavesMockDir/"; // mock dir

const char* G_user_backup_path = "Documents/stash_saver/Stellaris/save games/";
const char* G_socket_temp_path = "Documents/stash_saver/Stellaris/.sockets/";

auto build_path(auto&& system_root_path, auto&& user_name, auto&& arbitrary_path) {

    fs::path final_path = system_root_path / user_name / arbitrary_path;
    return final_path;
}

fs::path get_save_games_path(E_Plat_ID const& plat_id) {
   
    if (E_Plat_ID::WSL == plat_id) 
    {
        return build_path(get_host_filesystem_root() + "/c/Users/", get_current_username(plat_id), G_game_path_win);
    }
    else if(E_Plat_ID::Windows == plat_id) 
    {
         return build_path("C/Users/", get_current_username(plat_id), G_game_path_win);
    }
    else if(E_Plat_ID::Linux == plat_id) 
    {
        return build_path("/home", get_current_username(plat_id), G_game_path_linux);
    }
    else
    {
        return "get_save_games_path: Unsupported platform";
    }

    return "get_save_games_path: Unreachable";
}

fs::path get_backup_path(E_Plat_ID const& plat_id) {
   
    if (E_Plat_ID::WSL == plat_id) 
    {
        return build_path(get_host_filesystem_root(), get_current_username(plat_id), G_user_backup_path);
    }
    else if(E_Plat_ID::Windows == plat_id) 
    {
        return build_path("C/Users/", get_current_username(plat_id), G_user_backup_path);
    }
    else if(E_Plat_ID::Linux == plat_id) 
    {
        return build_path("/home", get_current_username(plat_id), G_user_backup_path);
    }
    else
    {
        return "get_backup_path: Unsupported platform";
    }

    return "get_backup_path: Unreachable";
}

fs::path get_socket_path(E_Plat_ID const& plat_id) {

    if (E_Plat_ID::WSL == plat_id) 
    {
        return build_path(get_host_filesystem_root(), get_current_username(plat_id), G_socket_temp_path);
    }
    else if(E_Plat_ID::Windows == plat_id) 
    {
        return build_path("C/Users/", get_current_username(plat_id), G_socket_temp_path);
    }
    else if(E_Plat_ID::Linux == plat_id) 
    {
        return build_path("/home", get_current_username(plat_id), G_socket_temp_path);
    }
    else
    {
        return "get_socket_path: Unsupported platform";
    }

    return "get_socket_path: Unreachable";
}



fs::path create_directory(fs::path const& dir);
fs::path create_file(fs::path const& file);


} // end of namespace StashSaves::Path


// int main() {

//     using StashSaves::PlatformIdentity::E_Plat_ID;
//     std::vector<E_Plat_ID> platforms = {E_Plat_ID::WSL, E_Plat_ID::Windows, E_Plat_ID::Linux};

//     for (const auto& plat_id : platforms) {
//         std::cout << "Platform: " << static_cast<int>(plat_id) << std::endl;
//         std::cout << "Current Username: " << StashSaves::Path::get_current_username(plat_id) << std::endl;
//         std::cout << "Save Games Path: " << StashSaves::Path::get_save_games_path(plat_id) << std::endl;
//         std::cout << "Backup Path: " << StashSaves::Path::get_backup_path(plat_id) << std::endl;
//         std::cout << "Socket Path: " << StashSaves::Path::get_socket_path(plat_id) << std::endl;
//         std::cout << "----------------------------------------" << std::endl;
//     }

//     return 0;
// }