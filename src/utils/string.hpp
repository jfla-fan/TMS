#pragma once


#include <string_view>
#include <array>


namespace tms::utils
{
    namespace impl
    {
        template< auto Value, std::size_t N >
        struct ValueArray;

        template< auto Value, std::size_t N, std::size_t... I >
        constexpr auto GenValueArrayHelper(std::index_sequence< I... >) -> std::array< decltype(Value), N >;

        template< auto Value, std::size_t N >
        constexpr auto GenValueArray() -> std::array< decltype(Value), N >;
    }

    template< auto Value, std::size_t N >
    constexpr auto Repeat() -> std::basic_string_view< decltype(Value) >;
}


#include "string.inl"