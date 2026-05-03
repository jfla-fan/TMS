#pragma once

#include "../models/user_fwd.hpp"

#include <string_view>
#include <userver/utils/optional_ref.hpp>


namespace tms::error
{
    struct ErrorInfo;
}


namespace tms::validators
{
    using ErrorRef = userver::utils::OptionalRef< tms::error::ErrorInfo >;

    bool ValidateUserName(const std::optional< std::string_view >& name, ErrorRef error_info = std::nullopt);
    bool ValidateUserLastName(const std::optional< std::string_view >& last_name, ErrorRef error_info = std::nullopt);
    bool ValidateUserEmail(const std::optional< std::string_view >& email, ErrorRef error_info = std::nullopt);
    bool ValidateUserLogin(const std::optional< std::string_view >& login, ErrorRef error_info = std::nullopt);
    bool ValidateUserPassword(const std::optional< std::string_view >& password, ErrorRef error_info = std::nullopt);
    bool ValidateUserRole(const std::optional< tms::models::EUserRole >& role, ErrorRef error_info = std::nullopt);
    bool ValidateUserId(const std::optional< tms::models::UserId >& id, ErrorRef error_info = std::nullopt);
}