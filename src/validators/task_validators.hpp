#pragma once

#include "../models/task_fwd.hpp"

#include <string_view>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/utils/optional_ref.hpp>


namespace tms::error
{
    struct ErrorInfo;
}


namespace tms::validators
{
    using ErrorRef = userver::utils::OptionalRef< tms::error::ErrorInfo >;

    bool ValidateTaskTitle(const std::optional< std::string_view >& title, ErrorRef error_info = std::nullopt);
    bool ValidateTaskDescription(const std::optional< std::string_view >& description, ErrorRef error_info = std::nullopt);
    bool ValidateTaskCategory(const std::optional< std::string_view >& category, ErrorRef error_info = std::nullopt);
    bool ValidateTaskDeadline(const std::optional< userver::storages::postgres::TimePointTz >& deadline, ErrorRef error_info = std::nullopt);
    bool ValidateTaskPriority(const std::optional< tms::models::ETaskPriority >& priority, ErrorRef error_info = std::nullopt);
    bool ValidateTaskStatus(const std::optional< tms::models::ETaskStatus >& status, ErrorRef error_info = std::nullopt);
}