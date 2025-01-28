#include <string> // for std::string
#include <format> // for std::format
#include <type_traits> // for std::underlying_type_t

#include "platform_identity_data.hpp"

std::string to_string(StashSaves::E_Platform_ID id) {

    using E_Platform_ID = StashSaves::E_Platform_ID;

    switch (id)
    {
    case E_Platform_ID::Default:
        return "E_Platform_ID::Default";
    case E_Platform_ID::Init:
        return "E_Platform_ID::Init";

    case E_Platform_ID::Unknown:
        return "E_Platform_ID::Unknown";

    case E_Platform_ID::Windows:
        return "E_Platform_ID::Windows";
    case E_Platform_ID::Linux:
        return "E_Platform_ID::Linux";
    case E_Platform_ID::WSL:
        return "E_Platform_ID::WSL";
        
    case E_Platform_ID::Error:
        return "E_Platform_ID::Error";    
    case E_Platform_ID::Max:
        return "E_Platform_ID::Max";
    default:
        return std::format("{} undefined E_Platform_ID value: {}", __func__, static_cast<std::underlying_type_t<E_Platform_ID>>(id));
    }
}
