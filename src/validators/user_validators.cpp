#include "user_validators.hpp"
#include "common_validators.hpp"
#include "validation_rules.hpp"


namespace tms::validators
{
    bool ValidateUserName(const std::optional< std::string_view >& name, ErrorRef error_info)
    {
        return ValidateField(name, rules::kUserNameRules, error_info, "name");
    }

    bool ValidateUserLastName(const std::optional< std::string_view >& last_name, ErrorRef error_info)
    {
        return ValidateField(last_name, rules::kUserLastNameRules, error_info, "last_name");
    }

    bool ValidateUserEmail(const std::optional< std::string_view >& email, ErrorRef error_info)
    {
        return ValidateField(email, rules::kUserEmailRules, error_info, "email");
    }

    bool ValidateUserLogin(const std::optional< std::string_view >& login, ErrorRef error_info)
    {
        return ValidateField(login, rules::kUserLoginRules, error_info, "login");
    }

    bool ValidateUserPassword(const std::optional< std::string_view >& password, ErrorRef error_info)
    {
        return ValidateField(password, rules::kUserPasswordRules, error_info, "password");
    }

    bool ValidateUserRole(const std::optional< tms::models::EUserRole >& role, ErrorRef error_info)
    {
        return ValidateMissing(role, error_info, "role");
    }

    bool ValidateUserId(const std::optional< tms::models::UserId >& id, ErrorRef error_info)
    {
        return ValidateMissing(id, error_info, "user_id");
    }

}