#pragma once

#include <string>
#include <optional>

namespace StashSaves::PlatformIdentity {

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
        E_Platform platform;
        std::string name;
    };

    std::optional<PlatformId> get_platform();

} // namespace StashSaves::PlatformIdentity