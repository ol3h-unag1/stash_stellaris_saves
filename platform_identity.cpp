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
    std::cout << PlatformIdentity::instance().use_count() << std::endl;
    return 0;
}
