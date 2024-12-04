#include <iostream>

#include "util.hpp"
namespace StashSaves::Util
{

std::vector<std::string> v1::generate_paths()  {

    std::vector<std::string> paths;
    paths.emplace_back("/home/user/Documents/file1.txt");
    paths.emplace_back("/home/user/Documents/file2.txt");
    paths.emplace_back("/home/user/Documents/file3.txt");
    paths.emplace_back("/home/user/Documents/file4.txt");

    return paths;
}

std::vector<std::string> v2::generate_paths() {
    std::vector<std::string> paths;
    paths.emplace_back("/home/user/Documents/file1.txt");
    paths.emplace_back("/home/user/Documents/file2.txt");

    return paths;
}


} // end namespace StashSaves::Util


int main()
{
	namespace util = StashSaves::Util;
	std::cout << util::generate_paths().size() << std::endl;
	std::cout << util::v2::generate_paths().size() << std::endl;
}