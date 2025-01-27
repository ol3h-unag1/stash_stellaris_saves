#include <iostream>

#include "platform_identity.hpp"

int main()
{
    std::cout << "Creating instance of plat_id" << std::endl;
    auto plat_id{ StashSaves::PlatformIdentity::instance()};

    std::cout << "Current username: " << plat_id->get_current_username() << std::endl;
}