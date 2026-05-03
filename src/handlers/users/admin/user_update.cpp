#include "user_update.hpp"

#include "models/user.hpp"
#include "dto/user.hpp"
#include "db/queries.hpp"
#include "utils/error.hpp"
#include "utils/hash.hpp"
#include "validators/validators.hpp"

#include <userver/storages/postgres/exceptions.hpp>
#include <userver/utils/from_string.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>



namespace tms::handlers::users::admin::put
{

UpdateUserHandler::UpdateUserHandler(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context)
    , pg_cluster_(component_context.FindComponent<userver::components::Postgres>("pg-tms-db")
                                   .GetCluster())
{ }


userver::formats::json::Value UpdateUserHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest& request,
                                                                        const userver::formats::json::Value& request_json,
                                                                        userver::server::request::RequestContext&) const
{
    UserId user_id;
    try {
        user_id = userver::utils::FromString< UserId >(request.GetPathArg("user_id"));
    } catch (const std::runtime_error& ex) {
        LOG_WARNING() << fmt::format("Failed to parse user id, actual value: {}", request.GetPathArg("user_id"));
        request.SetResponseStatus(userver::http::kBadRequest);
        return tms::error::MakeErrorJson(tms::error::kValidationError, "User id parsing error");
    }

    auto update_data = request_json.As< tms::dto::UserUpdateDTO >();

    tms::error::ErrorInfo error_info;
    if (!tms::validators::Validate(update_data, error_info)) {
        LOG_WARNING() << fmt::format("Failed to validate user update dto: {}", error_info.message);
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return tms::error::MakeErrorJson(std::move(error_info));
    }

    tms::models::User user;
    try {
        std::optional< std::string > hashed_password;
        if (update_data.password)
        {
            hashed_password = tms::utils::GenerateArgon2idHash(*update_data.password);
        }
        
        auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                                      tms::queries::kUpdateUserById,
                                                      update_data.name,
                                                      update_data.last_name,
                                                      update_data.email,
                                                      update_data.login,
                                                      update_data.role,
                                                      hashed_password,
                                                      user_id);
                                                     
        user = result.AsSingleRow< tms::models::User >(userver::storages::postgres::kRowTag);
    } catch (const userver::storages::postgres::UniqueViolation& ex)
    {
        LOG_ERROR() << fmt::format("Unique violation while trying to update user (id - {}): {}", user_id, ex.what());
        request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
        return tms::error::MakeErrorJson(tms::error::kConflictError, "Email or login already exist");
    } catch (const userver::storages::postgres::NonSingleRowResultSet& ex)
    {
        LOG_ERROR() << fmt::format("Failed to find user with id - {}, details: {}", user_id, ex.what());
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
        return tms::error::MakeErrorJson(tms::error::kUserNotFoundError, "User not found");
    }

    return userver::formats::json::ValueBuilder { user }.ExtractValue();
}


}