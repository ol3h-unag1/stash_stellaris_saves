#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

std::string getMountedFilesystemPath() {
    std::ifstream wslConf("/etc/wsl.conf");
    std::string line;
    std::string path;

    if (wslConf.is_open()) {
        while (std::getline(wslConf, line)) {
            if (line.find("root") != std::string::npos) {
                path = line.substr(line.find('=') + 1);
                break;
            }
        }
        wslConf.close();
    }
    return path;
}

std::string getCurrentUserName() {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen("/mnt/host/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe -Command \"[System.Security.Principal.WindowsIdentity]::GetCurrent().Name\"", "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    // Remove any trailing newline characters
    result.erase(result.find_last_not_of(" \n\r\t") + 1);
    return result;
}

int main() {
    std::string path = getMountedFilesystemPath();
    std::string userName = getCurrentUserName();

    std::cout << "Mounted Filesystem Path: " << path << std::endl;
    std::cout << "Current User Name: " << userName << std::endl;

    return 0;
}