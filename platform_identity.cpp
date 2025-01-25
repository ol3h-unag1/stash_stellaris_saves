#include <iostream>

#include "platform_identity.hpp"

struct PlatformIdentity::Access final : public PlatformIdentity 
{
    Access() = default;
    ~Access() override = default;
};

std::shared_ptr<PlatformIdentity::Access> PlatformIdentity::instance() {

    static std::shared_ptr<PlatformIdentity::Access> instance{ std::make_shared<PlatformIdentity::Access>() };
    return instance;
}


int main()
{
    PlatformIdentity::instance()->say_hello();
    return 0;
}

// compare my approach to using std::enable_shared_from_this (ask chat for halp)