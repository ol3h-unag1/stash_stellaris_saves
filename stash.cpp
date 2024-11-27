#include <iostream>
#include <vector>
#include <string>
#include <format>
#include <filesystem>
#include <algorithm>
#include <ranges>
#include <chrono>
#include <cstdlib>
#include <pwd.h>
#include <unistd.h>

#include <exception>

namespace fs = std::filesystem;

fs::path add_timestamp_to_path(const fs::path& target_path) {

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t_now);

    std::string timestamp = std::format("{:04}-{:02}-{:02}_{:02}-{:02}-{:02}",
                                        tm.tm_year + 1900,
                                        tm.tm_mon + 1,
                                        tm.tm_mday,
                                        tm.tm_hour,
                                        tm.tm_min,
                                        tm.tm_sec);

    return target_path / timestamp;
}

// Function: check_and_create_dir
// Purpose: Ensures the existence of a directory at the specified path.
// Parameters:
//   - path: const std::string& - The path of the directory to check or create.
// Returns: 
//   - bool - true if the directory exists or was successfully created, false otherwise.
// Notes: Uses std::filesystem to handle path validation and directory operations. 
bool check_and_create_dir(const std::string& path) {
    try {
        // If the directory exists or is created successfully, return true
        if (fs::exists(path) || fs::create_directories(path)) {
            return true;
        }
    } catch (...) {
        // Return false if any exception occurs during the process
    }
    return false; // Return false if the directory does not exist and cannot be created
}

/**
 * @brief Returns the last directory name from the given path.
 * E.g., for input "/home/username/test/last4" returns "last4"
 *
 * If the path is empty or invalid, returns an empty string.
 *
 * @param path The input path to evaluate.
 * @return std::string The last directory name, or an empty string if invalid.
 */
[[nodiscard]] constexpr std::string get_this_directory_name(const std::string& path) {
    try {
        fs::path p(path);
        if (p.empty()) {
            return "";
        }
        return p.filename().string();
    } catch (...) {
        return "";
    }
}

/**
 * @brief Returns the parent directory of the given path.
 *
 * If the path has no parent, it returns the passed parameter itself.
 * If the path is invalid, it returns an empty string.
 *
 * @param dir The directory path to evaluate.
 * @return std::string The parent directory path, the input if no parent, or an empty string if invalid.
 */
[[nodiscard]] constexpr std::string get_parent_directory(const std::string& dir) {
    try {
        fs::path path(dir);
        if (!fs::exists(path) || !fs::is_directory(path)) {
            return "";
        }
        auto parent = path.parent_path();
        return parent.empty() ? dir : parent.string();
    } catch (...) {
        return "";
    }
}

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
 * @note The function uses `std::system` to execute the command, which spawns
 *       a shell to run the command. Ensure that the command string is safe
 *       to avoid security risks (e.g., injection attacks).
 */
void execute_terminal_command(const std::string& command) {
    
    int result = std::system(command.c_str());
    if (result != 0)
    {
        throw std::invalid_argument("execute_terminal_command: bad command");
    }
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

[[nodiscard]] auto GetUserBackUpDirectory() {
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

    return backup_dir;
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

void StashSaves() {

	auto const user_path = GetUserBackUpDirectory();
    auto const exe_path = get_executable_directory();
	auto const target_path = user_path / exe_path.filename();
    auto const target_path_timestamp = add_timestamp_to_path(target_path);

    if (not check_and_create_dir(target_path)) 
    {
        static std::string msg;
        msg = std::format("stash_saves: can't create backup dir", target_path.string());
        throw std::runtime_error(msg.data());

        return; // o_O
    }

    if (not check_and_create_dir(target_path_timestamp)) 
    {
        static std::string msg;
        msg = std::format("stash_saves: can't create backup dir with timestamp", target_path.string());
        throw std::runtime_error(msg.data());

        return; // o_O
    }

    auto saves = get_files_with_extension(exe_path, ".sav");
    std::cout << std::format("I see {} save in '{}' directory", saves.size(), exe_path.string() ) << std::endl;

    if (saves.empty())
    {
        static std::string msg;
        msg = std::format("stash_saves: nothing to stash at {}", exe_path.string());
        throw std::runtime_error(msg.data());   
    }

    // Unary predicate to check if a string starts with "autosave"
    auto starts_with_autosave = [](const std::string& s) {

        return s.starts_with("autosave");
    };
    auto partition_point = std::ranges::partition(saves, starts_with_autosave);

    // View for "user saves" (not starting with "autosave")
    auto user_saves = std::ranges::subrange(partition_point.begin(), saves.end());
    std::ranges::for_each(user_saves, [&target_path_timestamp](auto const& save){

        std::string const command = std::format("mv {} {}", save, (target_path_timestamp / save).string() );
        execute_terminal_command(command);
    });

    auto auto_saves = std::ranges::subrange(saves.begin(), partition_point.begin());
    std::ranges::for_each(auto_saves, [&target_path_timestamp](auto const& save){

        std::string const command = std::format("cp {} {}", save, (target_path_timestamp / save).string() );
        execute_terminal_command(command);
    });


}

int main() {

try 
{
    StashSaves(); // go!
	
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
