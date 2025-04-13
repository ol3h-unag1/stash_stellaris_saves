#pragma once

#include <string>
#include <filesystem>
#include <vector>


namespace StashSaves::Util
{
using Int = int;

inline namespace v1
{

namespace fs = std::filesystem;



bool is_directory_exists(std::string const& path);
std::vector<fs::path> get_flat_subdirectories(fs::path const& dir);
std::vector<fs::path>  get_files_in_directory(fs::path const& directory);

} // end namespace StashSaves::Util::v1

} // end namespace StashSaves::Util