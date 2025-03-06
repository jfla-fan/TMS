#pragma once

#include <string_view>


namespace tms::validators::rules
{
    struct ValidationRulesLength
    {
        std::size_t min_length;
        std::size_t max_length;
    };

    struct ValidationRulesRegex
    {
        std::string_view pattern;
    };

    struct ValidationRulesField : ValidationRulesLength,
                                  ValidationRulesRegex
    {
    };

    inline constexpr ValidationRulesField kUserNameRules {{3, 20},{R"(^[\p{L}'’-]{3,20}$)"}};
    inline constexpr ValidationRulesField kUserLastNameRules {{3, 20}, {R"(^[\p{L}'’-]{3,20}$)"}};
    inline constexpr ValidationRulesField kUserEmailRules {{5, 254}, {R"(^[a-zA-Z0-9._%+-]+@(?:[a-zA-Z0-9]+(?:-[a-zA-Z0-9]+)*\.)+[a-zA-Z]{2,}$)"}};
    inline constexpr ValidationRulesField kUserLoginRules {{4, 20}, {R"(^[A-Za-z0-9_.-]{4,20}$)"}};
    inline constexpr ValidationRulesField kUserPasswordRules {{8, 64},{R"(^(?=.*[A-Z])(?=.*[a-z])(?=.*\d)(?=.*[!@#$%^&*])[A-Za-z\d!@#$%^&*]{8,64}$)"}};

    inline constexpr ValidationRulesField kTaskTitleRules {{1, 100},{R"(^[\p{L}\p{N} \-,.!?]{1,100}$)"}};
    inline constexpr ValidationRulesField kTaskDescriptionRules {{0, 1000 },{R"(^[\p{L}\p{N}\s\-,.!?\n\t]{0,1000}$)"}};
    inline constexpr ValidationRulesField kTaskCategoryRules {{1, 100 },{R"(^[\p{L}\p{N}\-_]{1,100}$)"}};
}