#include "task_validators.hpp"

#include "../utils/error.hpp"
#include "../utils/string.hpp"

#include <array>

#include <userver/utils/optionals.hpp>
#include <userver/utest/utest.hpp>


using namespace tms::validators;
using namespace tms::error;
using userver::utils::ToString;
using tms::utils::Repeat;


namespace
{
    struct ValidateTaskFieldTestEntry
    {
        std::optional< std::string_view > value;
        bool expected_result;
    };

    constexpr std::array< ValidateTaskFieldTestEntry, 14 > ValidateTaskTitleTestEntries =
    {
        // Correct entries
        ValidateTaskFieldTestEntry
        { "Task Title",          true  }, // Valid title
        { "Task 123",            true  }, // Valid title with numbers
        { "Task-Title",          true  }, // Valid title with hyphen
        { "Task, Title!",        true  }, // Valid title with comma and exclamation mark
        { "Task. Title?",        true  }, // Valid title with period and question mark
        { "Заголовок задачи",    true  }, // Valid title with non-Latin characters
        { "12345",               true  }, // Valid title with only numbers
        { "A",                   true  }, // Valid title with minimum length (1 character)
        { Repeat< 'a', 100 >(),  true  }, // Valid title with maximum length (100 characters)

        // Incorrect entries
        { "",                    false }, // Empty title
        { "Task@Title",          false }, // Invalid symbol (@)
        { Repeat< 'a', 101 >(),  false }, // Too long (101 characters)
        { "Task\nTitle",         false }, // Invalid symbol (newline)
        { "Task\tTitle",         false }, // Invalid symbol (tab)
    };

    constexpr std::array< ValidateTaskFieldTestEntry, 12 > ValidateTaskDescriptionTestEntries =
    {
        // Correct entries
        ValidateTaskFieldTestEntry
        { "Task Description",    true  }, // Valid description
        { "Task 123",            true  }, // Valid description with numbers
        { "Task-Description",    true  }, // Valid description with hyphen
        { "Task, Description!",  true  }, // Valid description with comma and exclamation mark
        { "Task. Description?",  true  }, // Valid description with period and question mark
        { "Описание задачи",     true  }, // Valid description with non-Latin characters
        { "",                    true  }, // Valid empty description
        { "A",                   true  }, // Valid description with minimum length (1 character)
        { Repeat< 'a', 1000 >(), true  }, // Valid description with maximum length (1000 characters)
        { "T\nD\tW\nNewlines",   true  }, // Valid description with newlines and tabs
    
        // Incorrect entries
        { "Task@Description",    false }, // Invalid symbol (@)
        { Repeat< 'a', 1001 >(), false }, // Too long (1001 characters)
    };

    constexpr std::array< ValidateTaskFieldTestEntry, 14 > ValidateTaskCategoryTestEntries =
    {
        // Correct entries
        ValidateTaskFieldTestEntry
        { "Category",            true  }, // Valid category
        { "Category123",         true  }, // Valid category with numbers
        { "Category-Name",       true  }, // Valid category with hyphen
        { "Category_Name",       true  }, // Valid category with underscore
        { "Категория",           true  }, // Valid category with non-Latin characters
        { "12345",               true  }, // Valid category with only numbers
        { "A",                   true  }, // Valid category with minimum length (1 character)
        { Repeat< 'a', 100 >(),  true  }, // Valid category with maximum length (100 characters)
    
        // Incorrect entries
        { "",                    false }, // Empty category
        { "Category@Name",       false }, // Invalid symbol (@)
        { Repeat< 'a', 101 >(),  false }, // Too long (101 characters)
        { "Category Name",       false }, // Invalid symbol (space)
        { "Category\nName",      false }, // Invalid symbol (newline)
    };
}


class ValidateTaskTitleTest         : public testing::TestWithParam< ValidateTaskFieldTestEntry > { };
class ValidateTaskDescriptionTest   : public testing::TestWithParam< ValidateTaskFieldTestEntry > { };
class ValidateTaskCategoryTest      : public testing::TestWithParam< ValidateTaskFieldTestEntry > { };


UTEST_P(ValidateTaskTitleTest, ValidateTaskTitle)
{
    const auto& [value, expected_result] = GetParam();
    tms::error::ErrorInfo info; bool actual_result = ValidateTaskTitle(value, info);
    EXPECT_EQ(actual_result, expected_result)
        << fmt::format("Value - {}\nExpected - {}\nMessage - {}", ToString(value), expected_result, info.message);
}


UTEST_P(ValidateTaskDescriptionTest, ValidateTaskDescription)
{
    const auto& [value, expected_result] = GetParam();
    tms::error::ErrorInfo info; bool actual_result = ValidateTaskDescription(value, info);
    EXPECT_EQ(actual_result, expected_result)
        << fmt::format("Value - {}\nExpected - {}\nMessage - {}", ToString(value), expected_result, info.message);
}


UTEST_P(ValidateTaskCategoryTest, ValidateTaskCategory)
{
    const auto& [value, expected_result] = GetParam();
    tms::error::ErrorInfo info; bool actual_result = ValidateTaskCategory(value, info);
    EXPECT_EQ(actual_result, expected_result)
        << fmt::format("Value - {}\nExpected - {}\nMessage - {}", ToString(value), expected_result, info.message);
}


INSTANTIATE_UTEST_SUITE_P(
    TaskValidators,
    ValidateTaskTitleTest,
    ::testing::ValuesIn(ValidateTaskTitleTestEntries)
);


INSTANTIATE_UTEST_SUITE_P(
    TaskValidators,
    ValidateTaskDescriptionTest,
    ::testing::ValuesIn(ValidateTaskDescriptionTestEntries)
);


INSTANTIATE_UTEST_SUITE_P(
    TaskValidators,
    ValidateTaskCategoryTest,
    ::testing::ValuesIn(ValidateTaskCategoryTestEntries)
);