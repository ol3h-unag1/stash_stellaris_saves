import <string>;
import <filesystem>;
import <iostream>;

namespace fs = std::filesystem;

// Define the enum class for operations
export enum class EFilesystemEvent
{
    DELETED,
    COPIED,
    INVALID
};

// Main program
int main(int argc, char* argv[]) 
{

    if (argc != 3) {
        std::cerr << "Usage: program <file_path> <operation>\n";
        std::cerr << "Operations: 0 (MOVE), 1 (COPY)\n";
        return 1;
    }

    return 0;
}
