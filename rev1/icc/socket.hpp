#pragma once

#include <string>  // std::string

#include <cstdint> // std::uint16_t
#include <any>     // std::any

namespace StashSaves
{

void write_to_socket(const std::string& what, const std::string& socket_path);
std::string read_from_socket(const std::string& socket_path);

}


#include <type_traits> // std::conditional
#include <vector>      // std::vector

namespace Communication
{

using ContainerType = std::any;

template<typename Type>
struct Message 
{
    using DataType = Type;
    ContainerType _container;
};

template<typename Type>
auto create_message(Type data)  {

    return Message<Type>{ std::move(data) };
}

auto create_str_message() {

    return create_message<std::string>("UWUWUWUWUW!");
}


}
