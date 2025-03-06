#include "../models/task.hpp"
#include "task.hpp"

#include <userver/logging/log.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/storages/postgres/io/chrono.hpp>


namespace
{
    std::optional< userver::storages::postgres::TimePointTz > ToOptionalTimePoint(const std::optional< std::string >& duration)
    {
        if (!duration)
        {
            LOG_ERROR() << "Task deadline duration is null";
            return std::nullopt;
        }

        auto timePoint = userver::utils::datetime::OptionalStringtime(*duration); 
        if (!timePoint)
        {
            LOG_ERROR() << fmt::format("Failed to parse string time from duration {}", *duration);
            return std::nullopt;
        }

        return timePoint ? std::make_optional(userver::storages::postgres::TimePointTz{ *timePoint }) : std::nullopt;
    }

    std::optional< tms::models::ETaskStatus > ToOptionalTaskStatus(const std::optional< std::string >& status)
    {
        return status ? tms::models::TaskStatusFromString(*status) : std::nullopt;
    }

    std::optional< tms::models::ETaskPriority > ToOptionalTaskPriority(const std::optional< std::string >& priority)
    {
        return priority ? tms::models::TaskPriorityFromString(*priority) : std::nullopt;
    }
}


namespace tms::dto
{
    TaskCreateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To< TaskCreateDTO >)
    {
        return {
            json["title"].As< std::optional< std::string > >(),
            json["description"].As< std::optional< std::string > >(),
            json["category"].As< std::optional< std::string > >(),
            ToOptionalTimePoint(json["deadline"].As< std::optional< std::string > >()),
            ToOptionalTaskPriority(json["priority"].As< std::optional< std::string > >()),
            ToOptionalTaskStatus(json["status"].As< std::optional< std::string > >()),
        };
    }

    TaskUpdateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To< TaskUpdateDTO >)
    {
        return {
            Parse(json, userver::formats::parse::To< TaskCreateDTO > {})
        };
    }

}