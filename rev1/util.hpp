#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <filesystem>

namespace StashSaves::Util
{
using Int = int;
using Tuple3Str = std::tuple<std::string, std::string, std::string>;
inline namespace v1
{

namespace fs = std::filesystem;

bool is_directory_exists(const std::string& path);

fs::path get_current_username();

std::vector<fs::path> get_flat_subdirectories(const fs::path& dir);


} // end namespace StashSaves::Util::v1

namespace v2
{



} // end namespace StashSaves::Util::v2
} // end namespace StashSaves::Util