#pragma once

#include <memory>
#include <filesystem>

namespace StashSaves
{

enum class E_Platform_ID
{
    Init = 0,
    Unsupported,
    Windows,
    Linux,
    WSL,
    Error
};

namespace fs = std::filesystem;
class PlatformIdentity 
{
public:
    PlatformIdentity();
    virtual ~PlatformIdentity() = default;
    
    PlatformIdentity(PlatformIdentity const&) = delete;
    PlatformIdentity& operator=(PlatformIdentity const&) = delete;
    PlatformIdentity(PlatformIdentity&&) = delete;
    PlatformIdentity& operator=(PlatformIdentity&&) = delete;

private:
    struct Access;

public:
    static std::shared_ptr<PlatformIdentity::Access> instance();

#define DEBUG_GET_PLATFORM_ID
#ifdef DEBUG_GET_PLATFORM_ID
public:
    auto get_platform_id() const;
#endif

public:
    fs::path get_current_username() const;
    fs::path get_save_games_path() const;
    fs::path get_backup_path() const;
    fs::path get_socket_path() const;

private:
    E_Platform_ID const _platform_id = E_Platform_ID::Init;

};

} // end of namespace StashSaves 