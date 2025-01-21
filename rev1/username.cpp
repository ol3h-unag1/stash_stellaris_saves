#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

std::vector<fs::path> get_human_users(const fs::path& mount_path) {
    std::vector<fs::path> users;
    fs::path users_path = mount_path / "c/Users";

    for (const auto& entry : fs::directory_iterator(users_path)) {
        if (entry.is_directory()) {
            fs::path user_name = entry.path().filename();
            if (user_name != "Default" && user_name != "Public" && user_name != "All Users" && user_name != "Default User" && user_name.string().find("defaultuser") != 0) {
                users.push_back(user_name);
            }
        }
    }

    return users;
}

int main() {
    fs::path mount_path = "/mnt/host"; // Adjust this path as necessary
    std::vector<fs::path> users = get_human_users(mount_path);

    std::cout << "Human users in c/Users:" << std::endl;
    for (const auto& user : users) {
        std::cout << user.string() << std::endl;
    }

    return 0;
}