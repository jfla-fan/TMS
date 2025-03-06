#pragma once

#include <jwt-cpp/jwt.h>
#include <userver/storages/postgres/io/chrono.hpp>

#include "../models/user_fwd.hpp"


namespace tms::utils
{
    struct JWTAuthSettings
    {
        static constexpr std::string_view kRequestContextKey = "jwt-auth-settings";
        static constexpr std::chrono::hours kDefaultTokenLifetime { 6 };

        models::UserId id;
        models::EUserRole role;
        std::string login;
        std::string email;
        userver::storages::postgres::TimePointTz issued_at;
        userver::storages::postgres::TimePointTz expires_at;
    };

    std::string GenerateJWT(const JWTAuthSettings& settings);
    std::string GenerateJWT(const models::User& user, const std::chrono::microseconds& token_lifetime = JWTAuthSettings::kDefaultTokenLifetime);
    JWTAuthSettings DecodeJWT(const std::string& data);
}