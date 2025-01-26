#pragma once

#include <memory>
#include <filesystem>

namespace StashSaves
{

namespace fs = std::filesystem;
class PlatformIdentity 
{
public:
    enum class E_Platform_ID
    {
        Init = 0,
        Unsupported,
        Windows,
        Linux,
        WSL,
        Error
    };

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

private:
    auto get_platform_id() const;

public:
    fs::path get_current_username() const;
    fs::path get_save_games_path() const;
    fs::path get_backup_path() const;
    fs::path get_socket_path() const;

private:
    E_Platform_ID const _platform_id = E_Platform_ID::Init;

};

} // end of namespace StashSaves 