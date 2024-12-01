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

void perform_operation(fs::path const& source_path, EFilesystemEvent event) {

    if (!fs::exists(source_path)) {
        throw std::runtime_error("Error: File does not exist at the provided path.");
    }

    switch (operation) {
        case EOperation::DELETED: {
            // do move
            break;
        }
        case EOperation::COPIED: {
            // do copy
            break;
        }
        case EOperation::INVALID:
        default:
            throw std::invalid_argument("Error: Invalid operation specified.");
            break;
    }
}

// Main program
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: program <file_path> <operation>\n";
        std::cerr << "Operations: 0 (MOVE), 1 (COPY)\n";
        return 1;
    }

    std::string filepath = argv[1];
    int operation_code;

    try {
        operation_code = std::stoi(argv[2]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid operation code. Please provide an integer.\n";
        return 1;
    }

    EFilesystemEvent operation = static_cast<EFilesystemEvent>(
        (operation_code == 0) ? 0 : (operation_code == 1) ? 1 : 2
    );

    perform_operation(filepath, operation);

    return 0;
}
