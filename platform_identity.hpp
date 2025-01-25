#pragma once


#include <memory>



class PlatformIdentity 
{
private:
    PlatformIdentity() = default;
    virtual ~PlatformIdentity() = default;
    
    PlatformIdentity(PlatformIdentity const&) = delete;
    PlatformIdentity& operator=(PlatformIdentity const&) = delete;
    PlatformIdentity(PlatformIdentity&&) = delete;
    PlatformIdentity& operator=(PlatformIdentity&&) = delete;

private:
    struct Access;

public:
    //using SharedPtr = std::shared_ptr<PlatformIdentity::Access>;
    static std::shared_ptr<PlatformIdentity::Access> instance();

public: 


};

