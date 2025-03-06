#include "task_validators.hpp"
#include "common_validators.hpp"
#include "validation_rules.hpp"


namespace tms::validators
{
    bool ValidateTaskTitle(const std::optional< std::string_view >& title, ErrorRef error_info)
    {
        return ValidateField(title, rules::kTaskTitleRules, error_info, "title");
    }

    bool ValidateTaskDescription(const std::optional< std::string_view >& description, ErrorRef error_info)
    {
        return ValidateField(description, rules::kTaskDescriptionRules, error_info, "description");
    }

    bool ValidateTaskCategory(const std::optional< std::string_view >& category, ErrorRef error_info)
    {
        return ValidateField(category, rules::kTaskCategoryRules, error_info, "category");
    }

    bool ValidateTaskDeadline(const std::optional< userver::storages::postgres::TimePointTz >& deadline, ErrorRef error_info)
    {
        return ValidateMissing(deadline, error_info, "deadline");
    }

    bool ValidateTaskPriority(const std::optional< tms::models::ETaskPriority >& priority, ErrorRef error_info)
    {
        return ValidateMissing(priority, error_info, "priority");
    }

    bool ValidateTaskStatus(const std::optional< tms::models::ETaskStatus >& status, ErrorRef error_info)
    {
        return ValidateMissing(status, error_info, "status");
    }
}