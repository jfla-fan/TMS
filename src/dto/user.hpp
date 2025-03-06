#pragma once

#include "../models/user_fwd.hpp"

#include <userver/formats/json.hpp>

#include <optional>
#include <string>


namespace tms::dto
{
    struct UserRegisterDTO
    {
        std::optional< std::string > name;
        std::optional< std::string > last_name;
        std::optional< std::string > email;
        std::optional< std::string > login;
        std::optional< std::string > password;
    };

    struct UserCreateDTO : UserRegisterDTO
    {
        std::optional< tms::models::EUserRole > role;
    };

    struct UserLoginDTO
    {
        std::optional< std::string > login;
        std::optional< std::string > password;
    };

    struct UserUpdateDTO : UserCreateDTO
    {
    };


    UserRegisterDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To< UserRegisterDTO >);
    UserUpdateDTO   Parse(const userver::formats::json::Value& json, userver::formats::parse::To< UserUpdateDTO >);
    UserCreateDTO   Parse(const userver::formats::json::Value& json, userver::formats::parse::To< UserCreateDTO >);
    UserLoginDTO    Parse(const userver::formats::json::Value& json, userver::formats::parse::To< UserLoginDTO >);
}
