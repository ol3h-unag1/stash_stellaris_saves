#include <string> // std::string


namespace StashSaves
{

void write_to_socket(const std::string& what, const std::string& socket_path);
std::string read_from_socket(const std::string& socket_path);

}