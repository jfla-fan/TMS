#include "task_create.hpp"

#include "db/queries.hpp"
#include "utils/error.hpp"
#include "models/task.hpp"
#include "dto/task.hpp"
#include "validators/validators.hpp"

#include <userver/components/component_context.hpp>
#include <userver/server/http/http_response.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/exceptions.hpp>


namespace tms::handlers::tasks::admin::post
{

CreateTaskHandler::CreateTaskHandler(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context)
    , pg_cluster_(component_context.FindComponent<userver::components::Postgres>("pg-tms-db")
                                   .GetCluster())
{ }


userver::formats::json::Value CreateTaskHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest& request,
                                                                        const userver::formats::json::Value& request_json,
                                                                        userver::server::request::RequestContext&) const
{
    auto create_params = request_json.As< tms::dto::TaskCreateDTO2 >();

    tms::error::ErrorInfo error_info;
    if (!tms::validators::Validate(create_params, error_info))
    {
        LOG_WARNING() << fmt::format("Failed to validate create task dto 2: {}", error_info.message);
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return tms::error::MakeErrorJson(std::move(error_info));
    }

    tms::models::Task task;
    try
    {
        auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                           tms::queries::kInsertTask,
                                           *create_params.title,
                                           *create_params.description,
                                           *create_params.category,
                                           *create_params.deadline,
                                           *create_params.priority,
                                           *create_params.status,
                                           *create_params.user_id);

        task = result.AsSingleRow< tms::models::Task >(userver::storages::postgres::kRowTag);
    } catch (const userver::storages::postgres::ForeignKeyViolation& ex)
    {
        LOG_ERROR() << fmt::format("Failed to create task for user {}, details: {}", *create_params.user_id, ex.what());
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
        return tms::error::MakeErrorJson(tms::error::kUserNotFoundError, "User not found.");
    }

    LOG_INFO() << fmt::format("Successfully created task {}, {}", task.title, task.id);
    
    return userver::formats::json::ValueBuilder { task }.ExtractValue();
}


}