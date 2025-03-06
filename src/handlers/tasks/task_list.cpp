#include "task_list.hpp"

#include "../../models/task.hpp"
#include "../../utils/jwt.hpp"
#include "../../db/queries.hpp"

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


namespace
{
    using userver::storages::postgres::TimePointTz;

    struct FilterSettings
    {
        std::optional< std::vector< tms::models::ETaskStatus > >     statuses;
        std::optional< std::vector< tms::models::ETaskPriority > >   priorities;
        std::optional< std::vector< std::string_view > >             categories;
        std::optional< TimePointTz >                                 deadline_start;
        std::optional< TimePointTz >                                 deadline_end;
        int                                                          page;
        int                                                          limit;
    };
    
    std::optional< TimePointTz > ToOptionalTimePoint(const std::string& duration)
    {
        if (duration.empty())
        {
            LOG_INFO() << "Task deadline duration is is empty";
            return std::nullopt;
        }

        auto timePoint = userver::utils::datetime::OptionalStringtime(duration); 
        if (!timePoint)
        {
            LOG_ERROR() << fmt::format("Failed to parse string time from duration {}", duration);
            return std::nullopt;
        }

        return timePoint ? std::make_optional(TimePointTz{ *timePoint }) : std::nullopt;
    }

    int ToIntOrDefault(const std::string& value, int default_value)
    {
        try
        {
           return userver::utils::FromString< int >(value); 
        } catch (...) {
            return default_value;
        }
    }

    template< typename ContainerResult, typename ContainerSource, typename Transform >
    std::optional< ContainerResult > ToContainerConvert(ContainerSource&& source, Transform&& func, std::string_view definition)
    {
        ContainerResult result;
        for (const auto& item : source)
        {
            auto transformed = func(item);
            if (!transformed.has_value())
            {
                LOG_WARNING() << fmt::format("ToContainerConvert - failed to parse {} (value - {})", definition, item);
                continue;
            }

            result.push_back(*transformed);
        }

        return !result.empty() ? std::make_optional(result) : std::nullopt;
    }

    FilterSettings Parse(const userver::server::http::HttpRequest& request)
    {
        using userver::utils::text::SplitIntoStringViewVector;

        FilterSettings result;

        const auto status_converter = [&](std::string_view value) { return ::tms::models::TaskStatusFromString(value); };
        const auto priority_converter = [&](std::string_view value) { return ::tms::models::TaskPriorityFromString(value); };
        const auto category_converter = [&](std::string_view value) { return value.empty() ? std::nullopt : std::make_optional< std::string_view >(value); };

        result.statuses         = ToContainerConvert< decltype(FilterSettings::statuses)::value_type >(SplitIntoStringViewVector(request.GetArg("statuses"), ","),
                                                                                                 status_converter, "status");
        result.priorities       = ToContainerConvert< decltype(FilterSettings::priorities)::value_type >(SplitIntoStringViewVector(request.GetArg("priorities"), ","),
                                                                                                   priority_converter, "priority");
        result.categories       = ToContainerConvert< decltype(FilterSettings::categories)::value_type >(SplitIntoStringViewVector(request.GetArg("categories"), ","),
                                                                                                   category_converter, "category");
        result.deadline_start   = ToOptionalTimePoint(request.GetArg("deadline_start"));
        result.deadline_end     = ToOptionalTimePoint(request.GetArg("deadline_end"));
        result.page             = ToIntOrDefault(request.GetArg("page"), 1);
        result.limit            = ToIntOrDefault(request.GetArg("limit"), 10);

        return result;
    }
}
void LogFilterSettingsDebug(const FilterSettings& filters); // @todo move to anonimous namespace ???


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
    FilterSettings filters = Parse(request);
    LogFilterSettingsDebug(filters);

    int user_id = context.GetData< tms::utils::JWTAuthSettings >(tms::utils::JWTAuthSettings::kRequestContextKey).id;

    auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                                           tms::queries::kFindTasksByFiltering,
                                                  std::vector { user_id },
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


void LogFilterSettingsDebug(const FilterSettings& filters)
{
#ifndef NDEBUG
    if (filters.statuses)
    {
        LOG_DEBUG() << "Statuses: ";
        for (const auto& status : *filters.statuses)
        {
            LOG_DEBUG() << fmt::format("{}", status);
        }
    }
    else {
        LOG_DEBUG() << "No statuses";
    }
    
    if (filters.priorities)
    {
        LOG_DEBUG() << "Priorities: ";
        for (const auto& priority : *filters.priorities)
        {
            LOG_DEBUG() << fmt::format("{}", priority);
        }
    }
    else {
        LOG_DEBUG() << "No priorities";
    }
    
    if (filters.categories)
    {
        LOG_DEBUG() << "Categories: ";
        for (const auto& category : *filters.categories)
        {
            LOG_DEBUG() << fmt::format("{}", category);
        }
    }
    else {
        LOG_DEBUG() << "No categories";
    }
    

    if (filters.deadline_start)
        LOG_DEBUG() << "Deadline start " << *filters.deadline_start;
    else
        LOG_DEBUG() << "No deadline start specified";


    if (filters.deadline_end)
        LOG_DEBUG() << "Deadline end " << *filters.deadline_end;
    else
        LOG_DEBUG() << "No deadline end specified";

    LOG_DEBUG() << fmt::format("Page - {}, Limit - {}", filters.page, filters.limit);
#endif
}