#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <format>
#include <filesystem>
#include <algorithm>
#include <ranges>
#include <chrono>
#include <cstdlib>
#include <pwd.h>
#include <unistd.h>

#include <exception>
#include <execution> // For parallel execution policies
#include <string_view>
#include <utility>

namespace fs = std::filesystem;


// TODO:
// Looking over all those functions I can't get aways from thoughts about pipes.
// But. 
// Main thing I think is how to check all required to werk korrecten things:
// - directories 
// - their parent directories
// So we need to have constantly valid 'target_path'
// Constantly valid 'parent_path / executable_path'
// When those are invalid system is paused or terminates itself.
//
// So the idea is to check them on a start.
// And then keep check on paths, if they're invalidated:
// - pause system;
// - validate path (create temporary or something);
// - unpause system;
// And also
// - terminate system (in case we can't find source 'save games' path)
// And also
// - keep pathes alive, so system can be:
//      a) gracefully terminated;
//      b) or keep working, but stashing files to another directory, e.g
//         in 'executable_path / tmp_ssstash /'         
// And also
// - we can redirect all calls to the system and cache results and only update the if index changes,
//   so that we know, when we know, that we good

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
    files.reserve(128);
    for (const auto& entry : fs::directory_iterator(directory)) 
    {
        if (entry.is_regular_file() && 
            entry.path().extension() == extension) 
        {
            files.push_back(entry.path().filename().string());
            std::cout << files.size() << ": " << entry << std::endl;
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

// Function to create and return a sorted reference view
auto sorted_lt_ref_vector(auto const& collection) {
    // Create a vector of reference wrappers to the original strings
    std::vector<std::reference_wrapper<const std::string>> sorted_refs(collection.begin(), collection.end());

    // Sort the references based on the string values
    std::ranges::sort(sorted_refs, [](const auto& a, const auto& b) {
        return a.get() < b.get();
    });

    // Return the sorted reference view
    return sorted_refs;
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
//  for (auto const& file : saves)
//  {
//       std::cout << file << std::endl;
//  }

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
    auto sorted_user_saves_ref_vector = sorted_lt_ref_vector(std::ranges::subrange(partition_point.begin(), saves.end()));
    
    auto sorted_user_saves_view = sorted_user_saves_ref_vector 
    | std::ranges::views::transform([](const auto& ref) -> const std::string& {
          return ref.get();
      }) 
    | std::ranges::views::take(sorted_user_saves_ref_vector.size() - 1); // Exclude last element so we can resume game

    std::cout << "Sorted view of " << sorted_user_saves_view.size() << std::endl;
    for (auto const& file : sorted_user_saves_view)
    {
        std::cout << file << std::endl; //.get() << std::endl;
    }

    std::ranges::for_each(sorted_user_saves_view, [&target_path_timestamp](auto const& save){

        //auto const& save = save_ref.get();
        std::string const command = std::format("mv {} {}", save, (target_path_timestamp / save).string() );
        execute_terminal_command(command);
    });

//  for (auto const& u_save : sorted_user_saves)
//  {
//      std::cout << u_save << std::endl;
//  }

    auto auto_saves = std::ranges::subrange(saves.begin(), partition_point.begin());
    std::ranges::for_each(auto_saves, [&target_path_timestamp](auto const& save){

        std::string const command = std::format("cp {} {}", save, (target_path_timestamp / save).string() );
        execute_terminal_command(command);
    });


}

namespace WorkInProgress 
{

    using Int = int;

    auto GeneratePaths() {

        auto const user_path = GetUserBackUpDirectory();
        auto const exe_path = get_executable_directory();
        auto const target_path = user_path / exe_path.filename();
        auto const target_path_timestamp = add_timestamp_to_path(target_path);

        return std::make_tuple(user_path, exe_path, target_path, target_path_timestamp);
    }

    // Generalized function for creating directories with detailed exceptions
        void ensure_directory(
            const std::filesystem::path& path, 
            const char* file, 
            Int line){

            if (not std::filesystem::exists(path)) 
            {
                if (not std::filesystem::create_directory(path)) 
                {
                    std::string msg = std::format("Failed to create directory '{}'. (File: {}, Line: {})", path.string(), file, line);
                    throw std::runtime_error(msg);
                }
            }

        }

    auto
    get_stems_for_extension(
        const fs::path& directory, 
        const std::string& extension) {

        if (!fs::exists(directory) || !fs::is_directory(directory)) 
        {
            throw std::runtime_error("Invalid directory: " + directory.string());
        }

        std::vector<std::string> stems;
        stems.reserve(128);

        for (const auto& entry : fs::directory_iterator(directory)) 
        {
            if (entry.is_regular_file() && entry.path().extension() == extension) 
            {
                stems.emplace_back(entry.path().stem().string()); // Use stem() to get the filename without extension
                //std::cout << stems.size() << ": " << entry.path().stem().string() << std::endl;
            }
        }

        return stems;
    }

#define ENSURE_DIR(path) ensure_directory(path, __FILE__, __LINE__)

// returns 0 - if ok 
// returns 1 - if not ok
// portion - [1, 50] how many saves stash a time;
//           50 by deafault;
//           out of range values defaults to 50;

int StashSaves(int portion = 50) {

    portion = std::clamp(portion, 1, 50);

    auto [user_path, exe_path, target_path, target_path_timestamp] = GeneratePaths();

    try 
    {   
        ENSURE_DIR(target_path);
        ENSURE_DIR(target_path_timestamp);
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1; // Exit with error code
    }

    auto saves = get_stems_for_extension(exe_path, ".sav");
    std::vector<std::string_view> view{ saves.begin(), saves.end() };
    std::sort(std::execution::par, view.begin(), view.end());

    auto const& sorted_view = view;
    auto resume_view = sorted_view | std::views::take(sorted_view.size() > 0 ? sorted_view.size() - 1 : 0);
    
    // T_T    
    auto is_auto_save = [](auto&& entry) -> bool { return entry.starts_with("autosave"); };
    auto is_not_auto_save = [&is_auto_save](auto&& entry) -> bool { return not is_auto_save(entry); };

    auto autosave_partion_point = std::ranges::find_if(resume_view, is_not_auto_save);
    auto autosaves = std::ranges::subrange(resume_view.begin(), autosave_partion_point);
    auto usersaves = std::ranges::subrange(autosave_partion_point, resume_view.end());

    auto printer = [&](std::string_view msg, auto&& collection) 
    {
        std::cout << msg << std::endl;
        for(auto&& item : collection)
        {
            std::cout << item << " -";
        }
        std::cout << std::endl;
    };


#define PRINTER(var) printer(#var ": ", var)

    PRINTER(resume_view);
    PRINTER(saves);
    PRINTER(autosaves);
    PRINTER(usersaves);

    //printer("resume_view: ", resume_view);
    //printer("saves: ", saves);

// TODO: we need to avoid useing callse to terminal as passing as tring will end up in the vulnarability? mhm
// so we do you std::filesystem
// yeah
// God-Speed!

    return 0;
}

}

int main() {

try 
{
    WorkInProgress::StashSaves(); // go!
	
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
