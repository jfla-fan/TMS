#include "user_get.hpp"

#include "../../../utils/error.hpp"
#include "../../../models/user.hpp"
#include "../../../db/queries.hpp"

#include <userver/utils/from_string.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/cluster.hpp>


namespace tms::handlers::users::admin::get
{

GetUserHandler::GetUserHandler(const userver::components::ComponentConfig& config,
                                   const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context)
    , pg_cluster_(component_context.FindComponent<userver::components::Postgres>("pg-tms-db")
                                   .GetCluster())
{ }


userver::formats::json::Value GetUserHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest& request,
                                                                     const userver::formats::json::Value&,
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

    tms::models::User user;
    try {
        auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                                tms::queries::kFindUserById,
                                                      user_id);
        
        user = result.AsSingleRow< tms::models::User >(userver::storages::postgres::kRowTag);
    } catch (const userver::storages::postgres::NonSingleRowResultSet& ex) {
        LOG_WARNING() << fmt::format("Failed to find user with id - {}", request.GetPathArg("task_id"));
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
        return tms::error::MakeErrorJson(tms::error::kUserNotFoundError, "User not found");
    }

    return userver::formats::json::ValueBuilder { user }.ExtractValue();
}


}