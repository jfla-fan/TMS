#include "user.hpp"

#include "../models/user.hpp"

#include <userver/formats/parse/common_containers.hpp>


namespace tms::dto
{
    UserRegisterDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To< UserRegisterDTO >)
    {
        return {
            json["name"].As< std::optional< std::string > >(),
            json["last_name"].As< std::optional< std::string > >(),
            json["email"].As< std::optional< std::string > >(),
            json["login"].As< std::optional< std::string > >(),
            json["password"].As< std::optional< std::string > >(),
        };
    }

    
    UserUpdateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To< UserUpdateDTO >)
    {
        return { Parse(json, userver::formats::parse::To< UserCreateDTO > {}) };
    }
    

    UserCreateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To< UserCreateDTO >)
    {
        return {
            {
                json["name"].As< std::optional< std::string > >(),
                json["last_name"].As< std::optional< std::string > >(),
                json["email"].As< std::optional< std::string > >(),
                json["login"].As< std::optional< std::string > >(),
                json["password"].As< std::optional< std::string > >()
            },
            models::UserRoleFromString(json["role"].As< std::optional< std::string > >().value_or(""))
        };
    }


    UserLoginDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To< UserLoginDTO >)
    {
        return {
            json["login"].As< std::optional< std::string > >(),
            json["password"].As< std::optional< std::string > >()
        };
    }
}