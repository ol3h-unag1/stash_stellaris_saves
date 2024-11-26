#include <iostream>
#include <string>
#include <format>
#include <filesystem>
#include <cstdlib>
#include <pwd.h>
#include <unistd.h>

#include <exception>

namespace fs = std::filesystem;

[[nodiscard]] fs::path get_executable_directory() {
    char exe_path[1024];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        throw std::runtime_error("Unable to determine executable path.");
    }
    exe_path[len] = '\0'; // Null-terminate the path
    return fs::path(exe_path).parent_path();
}

[[nodiscard]] std::string get_current_username() {
    if (const char* username = getenv("USER"); username != nullptr) {
        return username;
    }
    if (struct passwd* pw = getpwuid(getuid()); pw != nullptr) {
        return pw->pw_name;
    }
    return "Unknown";
}

[[nodiscard]] std::string GetUserBackUpDirectory() {
    const std::string username = get_current_username();
    if (username == "Unknown") {
        throw std::runtime_error("Unable to determine current user name");
    }

    const fs::path backup_dir = fs::path("/home") / username / "Stellaris" / "Saves";

    if (!fs::exists(backup_dir)) {
        if (!fs::create_directories(backup_dir)) {
            throw std::runtime_error("Failed to create backup directory: " + backup_dir.string());
        }
    }

    return backup_dir.string();
}

[[nodiscard]] std::size_t CountSaveFiles(const fs::path& directory) {
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        throw std::runtime_error("Invalid directory: " + directory.string());
    }

    std::size_t count = 0;
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".sav") {
            ++count;
        }
    }

    return count;
}


int main() {

try 
{
	auto user_path = GetUserBackUpDirectory();
	auto const total_saves = CountSaveFiles(user_path);
	auto const tostash_saves = CountSaveFiles(get_executable_directory());

	std::cout << std::format(" + + + Stashing to: {}\n"
		       	  	 " + + Saves before stashing: {}\n"
				 " + + Saves to stash: {} \n"  
			,user_path, total_saves, tostash_saves);
	return 0;

}
catch (std::exception& e)
{
	std::cout << std::format(" +++ +++ +++ Somethin happen bae: {}\n ++ ++ ++ I guess I die ",  e.what()) << std::endl;
	return 1;
}
catch (...)
{
	return 2;
}

	return -1;
}
