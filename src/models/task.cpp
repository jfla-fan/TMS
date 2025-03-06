#include "task.hpp"

#include <userver/formats/serialize/common_containers.hpp>


userver::formats::json::Value tms::models::Serialize(const Task& task, userver::formats::serialize::To< userver::formats::json::Value >)
{
    userver::formats::json::ValueBuilder builder;
    
    builder["task_id"]      = task.id;
    builder["title"]        = task.title;
    builder["description"]  = task.description;
    builder["category"]     = task.category;
    builder["deadline"]     = task.deadline;
    builder["priority"]     = ToStringView(task.priority);
    builder["status"]       = ToStringView(task.status);
    builder["user_id"]      = task.user_id;
    builder["created_at"]   = task.created_at;
    builder["updated_at"]   = task.updated_at;

    return builder.ExtractValue();
}