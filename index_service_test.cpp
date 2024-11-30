#include <iostream>
#include <filesystem>
#include <string>
#include <stdexcept>

namespace fs = std::filesystem;

bool canMoveFile(const fs::path& filePath) {
    try {
        // Check if file exists
        if (!fs::exists(filePath)) {
            throw std::runtime_error("File does not exist.");
        }

        // Check permissions
        auto perms = fs::status(filePath).permissions();
        bool canRead = (perms & fs::perms::owner_read) != fs::perms::none;
        bool canWrite = (perms & fs::perms::owner_write) != fs::perms::none;

        if (!canRead || !canWrite) {
            throw std::runtime_error("Insufficient permissions to move the file.");
        }

        return true;
    } catch (...) {
        // Rethrow any exceptions for upstream handling
        throw;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_file>" << std::endl;
        return 1;
    }

    fs::path filePath = argv[1];

    try {
        if (canMoveFile(filePath)) {
            std::cout << "The file can be moved: " << filePath << std::endl;

            // Example of moving the file to a temporary location
            fs::path tempPath = filePath;
            tempPath += ".moved";

            fs::rename(filePath, tempPath);
            std::cout << "File moved to: " << tempPath << std::endl;

            // Optionally move the file back for demonstration
            fs::rename(tempPath, filePath);
            std::cout << "File moved back to original location: " << filePath << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
        return 1;
    }

    return 0;
}
