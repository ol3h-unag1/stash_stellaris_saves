#pragma once

#include <memory>
#include <filesystem>

#include "types.hpp"

#define DEBUG_GET_PLATFORM_ID

namespace StashSaves
{

enum class E_Platform_ID : Types::EnumUnderlyingType;

namespace fs = std::filesystem;

class PlatformIdentity 
{
    //mock c-tor
private:
    PlatformIdentity(E_Platform_ID);

    // real c-tor
private:
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
    static std::shared_ptr<PlatformIdentity::Access> mock_instance();

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
    E_Platform_ID const _platform_id = Types::INITIALIZE_ENUM_VALUE<E_Platform_ID>(); // in case of emergency call 9-1-1
};

struct PlatformIdentity::Access final : public PlatformIdentity 
{
    // mock c-tor
    Access(E_Platform_ID platform_id);

    // real c-tor
    Access() = default;
    ~Access() override = default;
};

} // end of namespace StashSaves 