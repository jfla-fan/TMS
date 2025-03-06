#include "task_create.hpp"

#include "db/queries.hpp"
#include "utils/error.hpp"
#include "utils/jwt.hpp"
#include "models/task.hpp"
#include "dto/task.hpp"
#include "validators/validators.hpp"

#include <userver/components/component_context.hpp>
#include <userver/server/http/http_response.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/exceptions.hpp>


namespace tms::handlers::tasks::post
{

CreateTaskHandler::CreateTaskHandler(const userver::components::ComponentConfig& config,
                                     const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context)
    , pg_cluster_(component_context.FindComponent<userver::components::Postgres>("pg-tms-db")
                                   .GetCluster())
{ }


userver::formats::json::Value CreateTaskHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest& request,
                                                                        const userver::formats::json::Value& request_json,
                                                                        userver::server::request::RequestContext& context) const
{
    auto create_params = request_json.As< tms::dto::TaskCreateDTO >();

    tms::error::ErrorInfo error_info;
    if (!tms::validators::Validate(create_params, error_info))
    {
        LOG_WARNING() << fmt::format("Failed to validate create task dto: {}", error_info.message);
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return tms::error::MakeErrorJson(std::move(error_info));
    }

    int user_id = context.GetData< tms::utils::JWTAuthSettings >(tms::utils::JWTAuthSettings::kRequestContextKey).id;

    tms::models::Task task;
    // no exceptions that could be caused by client
    {
        auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                                                      tms::queries::kInsertTask,
                                                      *create_params.title,
                                                      *create_params.description,
                                                      *create_params.category,
                                                      *create_params.deadline,
                                                      *create_params.priority,
                                                      *create_params.status,
                                                       user_id);

        task = result.AsSingleRow< tms::models::Task >(userver::storages::postgres::kRowTag);
    }

    LOG_INFO() << fmt::format("Successfully created task {}, {}", task.title, task.id);
    
    return userver::formats::json::ValueBuilder { task }.ExtractValue();
}


}