#include "user.hpp"

#include <userver/formats/serialize/common_containers.hpp>


userver::formats::json::Value tms::models::Serialize(const User& user, userver::formats::serialize::To< userver::formats::json::Value >)
{
    userver::formats::json::ValueBuilder builder;
    
    builder["id"]           = user.id;
    builder["name"]         = user.name;
    builder["last_name"]    = user.last_name;
    builder["email"]        = user.email;
    builder["login"]        = user.login;
    builder["role"]         = ToStringView(user.role);
    builder["password"]     = user.hashed_password;
    builder["created_at"]   = user.created_at;
    builder["updated_at"]   = user.updated_at;

    return builder.ExtractValue();
}