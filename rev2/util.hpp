#pragma once

#include <string>
#include <filesystem>

namespace StashSaves::Util
{
using Int = int;

inline namespace v1
{

namespace fs = std::filesystem;



bool is_directory_exists(const std::string& path);
std::vector<fs::path> get_flat_subdirectories(const fs::path& dir);


} // end namespace StashSaves::Util::v1

} // end namespace StashSaves::Util