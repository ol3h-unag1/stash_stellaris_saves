#pragma once

#include <string>       // std::string

#include "types.hpp"

namespace StashSaves
{
    
enum class E_Platform_ID : Types::EnumUnderlyingType
{
    Init = Types::INIT_ENUM_VALUE,
    Default = Types::DEFAULT_ENUM_VALUE,

    Unknown,
    
    Windows,
    Linux,
    WSL,

    Error = Types::ERROR_ENUM_VALUE,
    Max = Types::MAX_ENUM_VALUE
};

}

std::string to_string(StashSaves::E_Platform_ID id);