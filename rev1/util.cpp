#include <iostream>

#include "util.hpp"
namespace StashSaves::Util
{


Tuple3Str v1::generate_paths()  {

    return std::make_tuple("Alice", "Bob", "Charlie");
}

bool is_directory_exists(const std::string& path) {

    return fs::exists(path) && fs::is_directory(path);
}


} // end namespace StashSaves::Util


int main()
{
	namespace util = StashSaves::Util;
	//std::cout << util::generate_paths().size() << std::endl;
	auto [a,b,c] = util::generate_paths();
	std::cout << a << std::endl;
	std::cout << b << std::endl;
	std::cout << c << std::endl;
}