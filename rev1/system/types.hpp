#pragma once

#include <cstdint>      // std::size_t, std::uint8_t
#include <limits>       // std::numeric_limits<T>::max()

namespace StashSaves::Types
{

using EnumUnderlyingType = std::int8_t;

constexpr EnumUnderlyingType INIT_ENUM_VALUE = -1;
constexpr EnumUnderlyingType DEFAULT_ENUM_VALUE = 0;

constexpr EnumUnderlyingType MAX_ENUM_VALUE = std::numeric_limits<EnumUnderlyingType>::max();
constexpr EnumUnderlyingType ERROR_ENUM_VALUE = MAX_ENUM_VALUE - 1;

template <typename Enum>
constexpr Enum INITIALIZE_ENUM_VALUE() {

    return static_cast<Enum>(INIT_ENUM_VALUE);
}

}
