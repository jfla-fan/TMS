#include "task_list_get.hpp"

#include "models/task.hpp"
#include "models/user.hpp"
#include "utils/jwt.hpp"
#include "db/queries.hpp"
#include "handlers/details/filter_settings.hpp"

#include <userver/storages/postgres/null.hpp>
#include <userver/utils/meta.hpp>
#include <userver/utils/text.hpp>
#include <userver/utils/enumerate.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/utils/from_string.hpp>
#include <userver/components/component_context.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/text_light.hpp>


namespace tms::handlers::tasks::get
{

ListTasksHandler::ListTasksHandler(const userver::components::ComponentConfig& config,
                                    const userver::components::ComponentContext& component_context)
    : HttpHandlerJsonBase(config, component_context)
    , pg_cluster_(component_context.FindComponent< userver::components::Postgres >("pg-tms-db")
                                   .GetCluster())
{ }


userver::formats::json::Value ListTasksHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest& request,
                                                                       const userver::formats::json::Value&,
                                                                       userver::server::request::RequestContext& context) const
{
    auto filters = impl::ParseFromRequest< impl::FilterSettings >(request);
    LogFilterSettingsDebug(filters);

    int user_id = context.GetData< tms::utils::JWTAuthSettings >(tms::utils::JWTAuthSettings::kRequestContextKey).id;

    auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                       tms::queries::kFindTasksByFiltering,
                                       std::vector< models::UserId >{ user_id },
                                       filters.statuses,
                                       filters.priorities,
                                       filters.categories,
                                       filters.deadline_start,
                                       filters.deadline_end,
                                       filters.page,
                                       filters.limit);

    auto tasks = result.AsSetOf< tms::models::Task >(userver::storages::postgres::kRowTag);

    userver::formats::json::ValueBuilder builder;

    builder["tasks"] = tasks;
    builder["page"]  = filters.page;
    builder["limit"] = filters.limit;
    builder["total"] = tasks.Size();

    return builder.ExtractValue();
}


}