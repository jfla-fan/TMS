#include "task_get.hpp"

#include "db/queries.hpp"
#include "utils/error.hpp"
#include "models/task.hpp"

#include <userver/http/status_code.hpp>
#include <userver/utils/assert.hpp>
#include <userver/utils/from_string.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/http/http_response.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/exceptions.hpp>


namespace tms::handlers::tasks::admin::get
{

GetTaskHandler::GetTaskHandler(const userver::components::ComponentConfig& config,
                               const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context)
    , pg_cluster_(component_context.FindComponent<userver::components::Postgres>("pg-tms-db")
                                   .GetCluster())
{ }


userver::formats::json::Value GetTaskHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest& request,
                                                                     const userver::formats::json::Value&,
                                                                     userver::server::request::RequestContext&) const
{
    TaskId task_id;
    try {
        task_id = userver::utils::FromString< TaskId >(request.GetPathArg("task_id"));
    } catch (const std::runtime_error& ex) {
        LOG_WARNING() << fmt::format("Failed to parse task id, actual value: {}", request.GetPathArg("task_id"));
        request.SetResponseStatus(userver::http::kBadRequest);
        return tms::error::MakeErrorJson(tms::error::kValidationError, "Failed to parse task id");
    }

    tms::models::Task task;
    try {
        auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                           tms::queries::kFindTaskById,
                                           task_id);

        task = result.AsSingleRow< tms::models::Task >(userver::storages::postgres::kRowTag);
    } catch (const userver::storages::postgres::NonSingleRowResultSet& ex) {
        LOG_ERROR() << fmt::format("Failed to find task with id - {}, details: {}", task_id, ex.what());
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
        return tms::error::MakeErrorJson(tms::error::kTaskNotFoundError, fmt::format("Failed to find task (id - {})", task_id));
    }

    return userver::formats::json::ValueBuilder { task }.ExtractValue();
}


}