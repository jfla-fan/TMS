#pragma once

#include <string>

#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>
#include <userver/formats/json.hpp>
#include <userver/utils/strong_typedef.hpp>
#include <userver/utils/trivial_map.hpp>



namespace tms::models
{
    enum class EUserRole : std::uint16_t
    {
        kUser,
        kAdmin,
        
        // special
        kCount,
    };

    inline constexpr std::optional< EUserRole > UserRoleFromString(std::string_view role);
    inline constexpr std::string_view ToStringView(EUserRole role);

    using UserId = std::int32_t;
    struct User
    {
        UserId id;
        std::string name;
        std::string last_name;
        std::string email;
        std::string login;
        EUserRole role;
        std::string hashed_password;
        ::userver::storages::postgres::TimePointTz created_at;
        ::userver::storages::postgres::TimePointTz updated_at;
    };

    userver::formats::json::Value Serialize(const User& user, userver::formats::serialize::To< userver::formats::json::Value >);
}

using tms::models::UserId;

template<>
struct userver::storages::postgres::io::CppToUserPg<tms::models::EUserRole>
{
    static constexpr DBTypeName postgres_name = "tms.user_role";
    static constexpr userver::utils::TrivialBiMap enumerators =
        [] (auto selector)
        {
            using ::tms::models::EUserRole;
            static_assert(userver::utils::UnderlyingValue(EUserRole::kCount) == 2, "Add or remove selector cases if you change the enum.");
            
            return selector()
                    .Case("user", EUserRole::kUser)
                    .Case("admin", EUserRole::kAdmin);
        };
};


template<>
struct fmt::formatter< tms::models::EUserRole >
{
    template<typename ParseContext>
    constexpr static auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const tms::models::EUserRole& role, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}", tms::models::ToStringView(role));
    }
};


inline constexpr std::optional< tms::models::EUserRole > tms::models::UserRoleFromString(std::string_view role)
{
    return userver::storages::postgres::io::CppToUserPg<tms::models::EUserRole>::enumerators.TryFind(role);
}

inline constexpr std::string_view tms::models::ToStringView(EUserRole role)
{
    return userver::storages::postgres::io::CppToUserPg<tms::models::EUserRole>::enumerators.TryFind(role).value();
}
