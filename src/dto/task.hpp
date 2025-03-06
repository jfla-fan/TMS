#pragma once

#include "../models/task_fwd.hpp"

#include <userver/formats/json.hpp>

#include <optional>
#include <string>
#include <userver/storages/postgres/io/chrono.hpp>


namespace tms::dto
{
    struct TaskCreateDTO
    {
        std::optional< std::string > title;
        std::optional< std::string > description;
        std::optional< std::string > category;
        std::optional< userver::storages::postgres::TimePointTz > deadline;
        std::optional< tms::models::ETaskPriority > priority;
        std::optional< tms::models::ETaskStatus > status;
    };

    struct TaskUpdateDTO : TaskCreateDTO
    {
    };
    
    TaskCreateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To< TaskCreateDTO >);
    TaskUpdateDTO Parse(const userver::formats::json::Value& json, userver::formats::parse::To< TaskUpdateDTO >);
}