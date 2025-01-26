#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <filesystem>
#include <memory>

namespace StashSaves::PlatformIdentity
{
	struct PlatformId;
}

using PlatformId = StashSaves::PlatformIdentity::PlatformId;
using PlatformIdPtr = std::shared_ptr<PlatformId>;

namespace StashSaves::Util
{
using Int = int;
using Tuple3Str = std::tuple<std::string, std::string, std::string>;
inline namespace v1
{

namespace fs = std::filesystem;

fs::path get_current_username(PlatformIdPtr);

fs::path get_save_games_path(PlatformIdPtr);
fs::path get_backup_path(PlatformIdPtr);
fs::path get_socket_path(PlatformIdPtr);

bool is_directory_exists(const std::string& path);
std::vector<fs::path> get_flat_subdirectories(const fs::path& dir);


} // end namespace StashSaves::Util::v1

namespace v2
{



} // end namespace StashSaves::Util::v2
} // end namespace StashSaves::Util