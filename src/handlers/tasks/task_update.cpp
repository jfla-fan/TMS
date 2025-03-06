#include "task_update.hpp"

#include "../../utils/jwt.hpp"
#include "../../utils/error.hpp"
#include "../../db/queries.hpp"
#include "../../dto/task.hpp"
#include "../../models/task.hpp"
#include "../../validators/validators.hpp"

#include <userver/storages/postgres/exceptions.hpp>
#include <userver/utils/from_string.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>


namespace tms::handlers::tasks::put
{

UpdateTaskHandler::UpdateTaskHandler(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context)
    , pg_cluster_(component_context.FindComponent<userver::components::Postgres>("pg-tms-db")
                                   .GetCluster())
{ }


userver::formats::json::Value UpdateTaskHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest& request,
                                                                        const userver::formats::json::Value& request_json,
                                                                        userver::server::request::RequestContext& context) const
{
    TaskId task_id;
    try {
        task_id = userver::utils::FromString< TaskId >(request.GetPathArg("task_id"));
    } catch (const std::runtime_error& ex) {
        LOG_WARNING() << fmt::format("Failed to parse task id, actual value: {}", request.GetPathArg("task_id"));
        request.SetResponseStatus(userver::http::kBadRequest);
        return tms::error::MakeErrorJson(tms::error::kValidationError, "Failed to parse task id");
    }

    auto update_data = request_json.As< tms::dto::TaskUpdateDTO >();

    tms::error::ErrorInfo error_info;
    if (!tms::validators::Validate(update_data, error_info))
    {
        LOG_WARNING() << fmt::format("Failed to validate update task dto: {}", error_info.message);
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return tms::error::MakeErrorJson(std::move(error_info));
    }

    int user_id = context.GetData< tms::utils::JWTAuthSettings >(tms::utils::JWTAuthSettings::kRequestContextKey).id;

    tms::models::Task task;
    try {
        auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                               tms::queries::kUpdateTaskByIdAndUser,
                                                      update_data.title,
                                                      update_data.description,
                                                      update_data.category,
                                                      update_data.deadline,
                                                      update_data.priority,
                                                      update_data.status,
                                                      task_id,
                                                      user_id);
        
        task = result.AsSingleRow< tms::models::Task >(userver::storages::postgres::kRowTag);
    } catch (const userver::storages::postgres::NonSingleRowResultSet& ex) {
        LOG_ERROR() << fmt::format("Failed to find task with id - {}, details: {}", task_id, ex.what());
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
        return tms::error::MakeErrorJson(tms::error::kTaskNotFoundError, fmt::format("Failed to find task (id - {})", task_id));
    }

    return userver::formats::json::ValueBuilder { task }.ExtractValue();
}


}