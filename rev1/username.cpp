#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

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

    std::cout << "Host root:" << host_root << std::endl;

    std::vector<std::string> possible_paths = {
        host_root + "/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe",
        host_root + "/c/Windows/sysnative/WindowsPowerShell/v1.0/powershell.exe",
        host_root + "/c/Windows/System32/WindowsPowerShell/v1.0/pwsh.exe",
        host_root + "/c/Windows/sysnative/WindowsPowerShell/v1.0/pwsh.exe",
    };
    
    for (auto const& path : possible_paths) {
        std::cout << "Checking path:" << path << std::endl;
        if (std::filesystem::exists(path))
            return path;
    }

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

    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    pclose(pipe);

    // Remove any trailing newline characters
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());

    return result;
}

int main() {
    std::string username = get_wsl_windows_username();
    std::cout << "WSL Windows Username:" << username << std::endl;
    return 0;
}