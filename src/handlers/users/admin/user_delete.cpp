#include "user_delete.hpp"

#include "../../../utils/error.hpp"
#include "../../../models/user.hpp"
#include "../../../db/queries.hpp"

#include <userver/http/status_code.hpp>
#include <userver/storages/postgres/exceptions.hpp>
#include <userver/utils/from_string.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>


namespace tms::handlers::users::admin::del
{

DeleteUserHandler::DeleteUserHandler(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context)
    , pg_cluster_(component_context.FindComponent<userver::components::Postgres>("pg-tms-db")
                                   .GetCluster())
{ }


userver::formats::json::Value DeleteUserHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest& request,
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
        auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                                      tms::queries::kDeleteUserById,
                                                      user_id);
                                                     
        user = result.AsSingleRow< tms::models::User >(userver::storages::postgres::kRowTag);
    } catch (const userver::storages::postgres::NonSingleRowResultSet& ex)
    {
        LOG_ERROR() << fmt::format("Failed to find user with id - {}, details: {}", user_id, ex.what());
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
        return tms::error::MakeErrorJson(tms::error::kUserNotFoundError, fmt::format("Failed to find user (id - {})", user_id));
    }

    return userver::formats::json::ValueBuilder { user }.ExtractValue();
}


}