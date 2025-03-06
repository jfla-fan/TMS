#include <userver/utils/meta_light.hpp>


template< auto Value, std::size_t N >
struct tms::utils::impl::ValueArray
{
    static constexpr std::array< decltype(Value), N > array = GenValueArray< Value, N >();
};


template< auto Value, std::size_t N, std::size_t... I >
constexpr auto tms::utils::impl::GenValueArrayHelper(std::index_sequence< I... >) -> std::array< decltype(Value), N >
{
    return { { ( (void)I, Value )... } };
}


template< auto Value, std::size_t N >
constexpr auto tms::utils::impl::GenValueArray() -> std::array< decltype(Value), N >
{
    return GenValueArrayHelper< Value, N >(std::make_index_sequence< N >{});
}


template< auto Value, std::size_t N >
constexpr auto tms::utils::Repeat() -> std::basic_string_view< decltype(Value) >
{
    static_assert(userver::meta::kIsCharacter< decltype(Value) >);

    auto& array = impl::ValueArray< Value, N >::array;
    return { array.data(), array.size() };
}

static_assert(tms::utils::Repeat< 'a', 5 >() == "aaaaa");
static_assert(tms::utils::Repeat< L'a', 1 >() == L"a");
