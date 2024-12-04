#pragma once

#include <tuple>
#include <string>

namespace StashSaves::Util
{
using Int = int;
using Tuple3Str = std::tuple<std::string, std::string, std::string>;
inline namespace v1
{


Tuple3Str generate_paths();

bool is_directory_exists(const std::string& path);



} // end namespace StashSaves::Util::v1

namespace v2
{



} // end namespace StashSaves::Util::v1
} // end namespace StashSaves::Util