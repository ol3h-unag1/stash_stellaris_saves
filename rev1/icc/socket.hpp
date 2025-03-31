#pragma once

#include <string>           // std::string
#include <vector>           // std::vector
#include <unordered_map>    // std::unordered_map
#include <cstdint>          // std::uint16_t
#include <any>              // std::any

#include "message.hpp"

namespace StashSaves
{

void write_to_socket(const std::string& what, const std::string& socket_path);
std::string read_from_socket(const std::string& socket_path);

}


namespace Communication
{



}

#define DEBUG_COMMUNICATION
#ifdef DEBUG_COMMUNICATION

template<typename Type>
auto create_message(Type data)  {

    return Communication::Message<Type>{ std::move(data) };
}

auto create_str_message() {

    return create_message<std::string>("UWUWUWUWUW!");
}

#endif