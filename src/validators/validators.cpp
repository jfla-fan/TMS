#include "validators.hpp"

#include "common_validators.hpp"
#include "user_validators.hpp"
#include "task_validators.hpp"

#include "dto/user.hpp"
#include "dto/task.hpp"


namespace tms::validators
{
    bool Validate(const tms::dto::UserRegisterDTO& user_register_dto, ErrorRef error_info)
    {
        return ValidateUserName(user_register_dto.name, error_info) &&
               ValidateUserLastName(user_register_dto.last_name, error_info) &&
               ValidateUserEmail(user_register_dto.email, error_info) &&
               ValidateUserLogin(user_register_dto.login, error_info) &&
               ValidateUserPassword(user_register_dto.password, error_info);
    }


    bool Validate(const tms::dto::UserUpdateDTO& user_register_dto, ErrorRef error_info)
    {
        return (!user_register_dto.name      || ValidateUserName(user_register_dto.name, error_info)) &&
               (!user_register_dto.last_name || ValidateUserLastName(user_register_dto.last_name, error_info)) &&
               (!user_register_dto.email     || ValidateUserEmail(user_register_dto.email, error_info)) &&
               (!user_register_dto.login     || ValidateUserLogin(user_register_dto.login, error_info)) &&
               (!user_register_dto.password  || ValidateUserPassword(user_register_dto.password, error_info));
    }


    bool Validate(const tms::dto::UserCreateDTO& user_create_dto, ErrorRef error_info)
    {
        return Validate(static_cast< tms::dto::UserRegisterDTO >(user_create_dto), error_info) &&
               ValidateUserRole(user_create_dto.role, error_info);
    }


    bool Validate(const tms::dto::UserLoginDTO& user_login_dto, ErrorRef error_info)
    {
        // rules may change, but the values already existing in database most likely won't.
        return ValidateMissing(user_login_dto.login, error_info) &&
               ValidateMissing(user_login_dto.password, error_info);
    }


    bool Validate(const tms::dto::TaskCreateDTO& task_create_dto, ErrorRef error_info)
    {
        return ValidateTaskTitle(task_create_dto.title, error_info) &&
               ValidateTaskDescription(task_create_dto.description, error_info) &&
               ValidateTaskCategory(task_create_dto.category, error_info) &&
               ValidateTaskDeadline(task_create_dto.deadline, error_info) &&
               ValidateTaskPriority(task_create_dto.priority, error_info) &&
               ValidateTaskStatus(task_create_dto.status, error_info);

    }


    bool Validate(const tms::dto::TaskUpdateDTO& task_create_dto, ErrorRef error_info)
    {
        return (!task_create_dto.title       || ValidateTaskTitle(task_create_dto.title, error_info)) &&
               (!task_create_dto.description || ValidateTaskDescription(task_create_dto.description, error_info)) &&
               (!task_create_dto.category    || ValidateTaskCategory(task_create_dto.category, error_info)) &&
               (!task_create_dto.deadline    || ValidateTaskDeadline(task_create_dto.deadline, error_info)) &&
               (!task_create_dto.priority    || ValidateTaskPriority(task_create_dto.priority, error_info)) &&
               (!task_create_dto.status      || ValidateTaskStatus(task_create_dto.status, error_info));
    }
}