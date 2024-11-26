#include <iostream>
#include <string>
#include <format>
#include <filesystem>
#include <cstdlib>
#include <pwd.h>
#include <unistd.h>

#include <exception>

namespace fs = std::filesystem;

/**
 * @brief Retrieves a vector of filenames with the specified extension from a given directory.
 *
 * This function performs a flat lookup (non-recursive) for files with the specified extension
 * in the given directory and returns their filenames.
 *
 * @param directory The path to the directory to search in.
 * @param extension The file extension to filter by (e.g., ".txt", ".sav").
 * @return std::vector<std::string> A vector containing the filenames matching the extension.
 * @throws std::runtime_error If the directory does not exist or is not a directory.
 */
std::vector<std::string> get_files_with_extension(const fs::path& directory, const std::string& extension) {
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        throw std::runtime_error("Invalid directory: " + directory.string());
    }

    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            files.push_back(entry.path().filename().string());
        }
    }

    return files;
}

/**
 * @brief Executes a terminal command on a Linux Mint system.
 *
 * This function takes a command string, executes it in the terminal,
 * and returns a status code indicating success or failure. It is designed
 * to work on Linux Mint or similar Linux systems.
 *
 * @param command The terminal command to execute as a string.
 * @return int Returns 0 if the command executed successfully, or -1 if it failed.
 *
 * @note The function uses `std::system` to execute the command, which spawns
 *       a shell to run the command. Ensure that the command string is safe
 *       to avoid security risks (e.g., injection attacks).
 */
int execute_terminal_command(const std::string& command) {
    int result = std::system(command.c_str());
    return (result == 0) ? 0 : -1; // Return 0 for success, -1 for failure
}

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

   return -1; // main return
}
