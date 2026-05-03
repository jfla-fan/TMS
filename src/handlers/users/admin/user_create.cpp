#include "user_create.hpp"

#include "utils/jwt.hpp"
#include "utils/hash.hpp"
#include "utils/error.hpp"
#include "models/user.hpp"
#include "db/queries.hpp"
#include "dto/user.hpp"
#include "validators/validators.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/exceptions.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include <userver/utils/datetime.hpp>



namespace tms::handlers::users::admin::post
{

CreateUserHandler::CreateUserHandler(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context)
    , pg_cluster_(component_context.FindComponent<userver::components::Postgres>("pg-tms-db")
                                   .GetCluster())
{ }
                          
userver::formats::json::Value CreateUserHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest& request,
                                                                        const userver::formats::json::Value& request_json,
                                                                        userver::server::request::RequestContext&) const
{
    auto create_params = request_json.As< dto::UserCreateDTO >();    

    tms::error::ErrorInfo error_info;
    if (!tms::validators::Validate(create_params, error_info)) {
        LOG_WARNING() << fmt::format("Failed to validate user create dto: {}", error_info.message);
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return tms::error::MakeErrorJson(std::move(error_info));
    }

    tms::models::User user;
    try {
        std::string hashed_password = tms::utils::GenerateArgon2idHash(*create_params.password);
        
        auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                                      tms::queries::kInsertUser,
                                                      *create_params.name,
                                                      *create_params.last_name,
                                                      *create_params.email,
                                                      *create_params.login,
                                                      *create_params.role,
                                                      hashed_password);
                                                     
        user = result.AsSingleRow< tms::models::User >(userver::storages::postgres::kRowTag);
    } catch (const userver::storages::postgres::UniqueViolation& ex) {
        LOG_ERROR() << fmt::format("Unique violation while trying to register user {}: {}", *create_params.name, ex.what());
        request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
        return tms::error::MakeErrorJson(tms::error::kConflictError, "Email or login already exist.");
    }
    
    userver::formats::json::ValueBuilder builder;

    std::string token;
    try {
        token = utils::GenerateJWT(user);    
    } catch (const std::system_error& err) {
        LOG_ERROR() << fmt::format("Error while generating jwt token - {}", err.what());
        request.SetResponseStatus(userver::server::http::HttpStatus::kInternalServerError);
        return tms::error::MakeErrorJson(tms::error::kJWTGenerateError, "Failed to generate JWT token.");
    }

    builder["user"] = user;
    builder["token"] = token;

    return builder.ExtractValue();    
}


}