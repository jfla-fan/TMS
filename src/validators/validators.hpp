#pragma once

#include <userver/utils/optional_ref.hpp>


namespace tms::error
{
    struct ErrorInfo;
}

namespace tms::dto
{
    struct UserRegisterDTO;
    struct UserUpdateDTO;
    struct UserCreateDTO;
    struct UserLoginDTO;
    struct TaskCreateDTO;
    struct TaskCreateDTO2;
    struct TaskUpdateDTO;
}


namespace tms::validators
{
    using ErrorRef = userver::utils::OptionalRef< tms::error::ErrorInfo >;

    bool Validate(const tms::dto::UserRegisterDTO& user_register_dto, ErrorRef error_info = std::nullopt);
    bool Validate(const tms::dto::UserUpdateDTO& user_register_dto, ErrorRef error_info = std::nullopt);
    bool Validate(const tms::dto::UserCreateDTO& user_create_dto, ErrorRef error_info = std::nullopt);
    bool Validate(const tms::dto::UserLoginDTO& user_login_dto, ErrorRef error_info = std::nullopt);
    bool Validate(const tms::dto::TaskCreateDTO& task_create_dto, ErrorRef error_info = std::nullopt);
    bool Validate(const tms::dto::TaskCreateDTO2& task_create_dto, ErrorRef error_info = std::nullopt);
    bool Validate(const tms::dto::TaskUpdateDTO& task_create_dto, ErrorRef error_info = std::nullopt);
}