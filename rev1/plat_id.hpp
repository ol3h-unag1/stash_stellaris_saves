#pragma once

#include <string>
#include <optional>

namespace StashSaves::PlatformIdentity {

inline namespace v1 {

    enum class E_Platform {
        Android,
        Emscripten,
        Fuchsia,
        Linux,
        POSIX,
        Unknown,
        WSL,
        WSL2,
        Windows,
        macOS,
        Unsupported
    };
    
    struct PlatformId 
    {
        E_Platform platform_value;
        std::string platform_name;
    };

    std::optional<PlatformId> get_platform();

} // end of inline namespace StashSaves::PlatformIdentity::v1

} // namespace StashSaves::PlatformIdentity