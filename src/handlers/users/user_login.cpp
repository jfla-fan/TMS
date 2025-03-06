#include "user_login.hpp"

#include "../../models/user.hpp"
#include "../../dto/user.hpp"
#include "../../db/queries.hpp"
#include "../../validators/validators.hpp"
#include "../../utils/jwt.hpp"
#include "../../utils/hash.hpp"
#include "../../utils/error.hpp"

#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/cluster.hpp>


namespace tms::handlers::users::post
{

LoginUserHandler::LoginUserHandler(const userver::components::ComponentConfig& config,
                                   const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context)
    , pg_cluster_(component_context.FindComponent<userver::components::Postgres>("pg-tms-db")
                                   .GetCluster())
{ }

userver::formats::json::Value LoginUserHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest& request,
                                                                       const userver::formats::json::Value& request_json,
                                                                       userver::server::request::RequestContext&) const
{
    auto login_params = request_json.As< tms::dto::UserLoginDTO >();

    tms::error::ErrorInfo error_info;
    if (!tms::validators::Validate(login_params, error_info))
    {
        LOG_WARNING() << fmt::format("Failed to validate user login dto: {}", error_info.message);
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return tms::error::MakeErrorJson(std::move(error_info));
    }

    auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                           tms::queries::kFindUserByLogin,
                                                 *login_params.login);

    if (result.IsEmpty())
    {
        LOG_WARNING() << fmt::format("Failed to find login: {}", *login_params.login);
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        return tms::error::MakeErrorJson(tms::error::kAuthError, "Wrong login");
    }

    auto user = result.AsSingleRow< tms::models::User >(userver::storages::postgres::kRowTag);

    if (!tms::utils::VerifyArgon2idPassword(user.hashed_password, *login_params.password))
    {
        LOG_WARNING() << "Password validation failed";
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        return tms::error::MakeErrorJson(tms::error::kAuthError, "Wrong password");   
    }

    std::string token;
    try {
        token = utils::GenerateJWT(user);    
    } catch (const std::system_error& err) {
        LOG_ERROR() << fmt::format("Error while generating jwt token - {}", err.what());
        request.SetResponseStatus(userver::server::http::HttpStatus::kInternalServerError);
        return tms::error::MakeErrorJson(tms::error::kJWTGenerateError, "Failed to generate JWT token.");
    }

    userver::formats::json::ValueBuilder builder;
    builder["token"] = token;

    return builder.ExtractValue();
}


}