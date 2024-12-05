#include <iostream>
#include <filesystem>

#include <cstdlib>    // for getenv
#include <unistd.h>   // for getuid
#include <pwd.h>      // for getpwuid

#include "util.hpp"
namespace StashSaves::Util
{


Tuple3Str v1::generate_paths()  {

    return std::make_tuple("Alice", "Bob", "Charlie");
}

bool v1::is_directory_exists(const std::string& path) {

    return fs::exists(path) && fs::is_directory(path);
}

std::string v1::get_current_username() {

    if (const char* username = getenv("USER"); username != nullptr) 
    {
        return username;
    }

    if (struct passwd* pw = getpwuid(getuid()); pw != nullptr) 
    {
        return pw->pw_name;
    }
    return "";
}

} // end namespace StashSaves::Util

/*
int main()
{
	namespace util = StashSaves::Util;
	//std::cout << util::generate_paths().size() << std::endl;
	auto [a,b,c] = util::generate_paths();
	std::cout << a << std::endl;
	std::cout << b << std::endl;
	std::cout << c << std::endl;
}
*/