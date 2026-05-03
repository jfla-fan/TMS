#include "user_validators.hpp"

#include "../utils/error.hpp"

#include <array>

#include <userver/utils/optionals.hpp>
#include <userver/utest/utest.hpp>


using namespace tms::validators;
using namespace tms::error;
using userver::utils::ToString;


namespace
{
    struct ValidateUserFieldTestEntry
    {
        std::optional< std::string_view > value;
        bool expected_result;
    };

    constexpr std::array< ValidateUserFieldTestEntry, 11 > ValidateUserNameTestEntries = {
        // Correct entries
        ValidateUserFieldTestEntry
        { "JohnDoe",                        true  }, // Valid name
        { "Admin",                          true  }, // Valid name 2
        { "Élodie",                         true  }, // Unicode name
        { "O'Connor",                       true  }, // Name with apostrophe
        { "Jean-Luc",                       true  }, // Name with hyphen
        { "Åsa",                            true  }, // Unicode name with special character
    
        // Incorrect entries
        { "Jo",                             false }, // Too short
        { "JohnDoe123",                     false }, // Contains numbers
        { "John Doe",                       false }, // Contains space
        { "",                               false }, // Empty string
        { "John@Doe",                       false }, // Contains invalid symbol
    };

    constexpr std::array< ValidateUserFieldTestEntry, 11 > ValidateUserLastNameTestEntries =
    {
        // Correct entries (expected_result = true)
        ValidateUserFieldTestEntry
        { "Smith",                          true  }, // Valid last name
        { "User",                           true  }, // Valid last name 2
        { "Åsa",                            true  }, // Unicode last name
        { "O’Reilly",                       true  }, // Last name with apostrophe
        { "van-der-Waals",                  true  }, // Last name with hyphen
        { "Иванов",                         true  }, // Cyrillic last name
    
        // Incorrect entries (expected_result = false)
        { "Sm",                             false }, // Too short
        { "Smith123",                       false }, // Contains numbers
        { "van der Waals",                  false }, // Contains space
        { "",                               false }, // Empty string
        { "Smith@Doe",                      false }, // Contains invalid symbol
    };

    constexpr std::array<ValidateUserFieldTestEntry, 23> ValidateUserEmailTestEntries = {
        // Correct entries
        ValidateUserFieldTestEntry
        { "user@example.com",               true  }, // Valid email
        { "admin@example.com",              true  }, // Valid email 2
        { "user.name+tag@example.co.uk",    true  }, // Valid email with tag
        { "user_name@example.com",          true  }, // Valid email with underscore
        { "user@sub.example.com",           true  }, // Valid email with subdomain
        { "user@example-domain.com",        true  }, // Valid email with hyphen
        { "user@example.com.uk",            true  }, // Valid email with multiple domains
        { "user@example.travel",            true  }, // Valid email with a long TLD
        { "user@123.example.com",           true  }, // Valid email with numbers in domain
        { "user@example.co.in",             true  }, // Valid email with country code TLD
    
        // Incorrect entries
        { "user@",                         false  }, // Invalid domain
        { "user@com",                      false  }, // Invalid domain (no TLD)
        { "user@example..com",             false  }, // Double dot
        { "user@.com",                     false  }, // Leading dot
        { "user@example,com",              false  }, // Invalid symbol
        { "user@example.",                 false  }, // Trailing dot
        { "user@.example.com",             false  }, // Leading dot in domain
        { "user@example..com",             false  }, // Consecutive dots
        { "user@-example.com",             false  }, // Leading hyphen in domain
        { "user@example-.com",             false  }, // Trailing hyphen in domain
        { "user@example.c",                false  }, // TLD too short
        { "user@example.123",              false  }, // Numeric TLD
        { "user@example.c_o_m",            false  }, // Invalid characters in TLD
    };

    constexpr std::array< ValidateUserFieldTestEntry, 11 > ValidateUserLoginTestEntries =
    {
        // Correct entries (expected_result = true)
        ValidateUserFieldTestEntry
        { "user_123",                       true  }, // Valid login
        { "admin_login",                    true  }, // Valid login 2
        { "user-login",                     true  }, // Valid login with hyphen
        { "UserLogin",                      true  }, // Valid login with uppercase
        { "user123",                        true  }, // Valid login with numbers
        { "user.login",                     true  }, // Valid login with dot
    
        // Incorrect entries (expected_result = false)
        { "usr",                            false }, // Too short
        { "user@login",                     false }, // Contains invalid symbol
        { "user login",                     false }, // Contains space
        { "",                               false }, // Empty string
        { "user!",                          false }, // Contains invalid symbol
    };

    constexpr std::array< ValidateUserFieldTestEntry, 11 > ValidateUserPasswordTestEntries = {
        // Correct entries (expected_result = true)
        ValidateUserFieldTestEntry
        { "Admin@123",                      true  }, // Valid password
        { "Password123!",                   true  }, // Valid password
        { "P@ssw0rd",                       true  }, // Valid password
        { "SecurePwd1#",                    true  }, // Valid password
        { "P@ssw0rd!",                      true  }, // Valid password
        { "1qaz@WSX",                       true  }, // Valid password
    
        // Incorrect entries (expected_result = false)
        { "password",                       false }, // No uppercase, digit, or special character
        { "PASSWORD123",                    false }, // No lowercase or special character
        { "P@ss",                           false }, // Too short
        { "",                               false }, // Empty string
        { "password123",                    false }, // No special character
    };
}


class ValidateUserNameTest      : public testing::TestWithParam< ValidateUserFieldTestEntry > { };
class ValidateUserLastNameTest  : public testing::TestWithParam< ValidateUserFieldTestEntry > { };
class ValidateUserEmailTest     : public testing::TestWithParam< ValidateUserFieldTestEntry > { };
class ValidateUserLoginTest     : public testing::TestWithParam< ValidateUserFieldTestEntry > { };
class ValidateUserPasswordTest  : public testing::TestWithParam< ValidateUserFieldTestEntry > { };


UTEST_P(ValidateUserNameTest, ValidateUserName)
{
    const auto& [ value, expected_result ] = GetParam();
    ErrorInfo info; bool actual_result = ValidateUserName(value, info);
    EXPECT_EQ(actual_result, expected_result) <<
        fmt::format("Value: {}\nExpected: {}\nMessage: {}", ToString(value), expected_result, info.message);
}


UTEST_P(ValidateUserLastNameTest, ValidateUserLastName)
{
    const auto& [ value, expected_result ] = GetParam();
    ErrorInfo info; bool actual_result = ValidateUserLastName(value, info);
    EXPECT_EQ(actual_result, expected_result) <<
        fmt::format("Value - {}\nExpected - {}\nMessage: {}", ToString(value), expected_result, info.message);
}


UTEST_P(ValidateUserEmailTest, ValidateUserEmail)
{
    const auto& [ value, expected_result ] = GetParam();
    ErrorInfo info; bool actual_result = ValidateUserEmail(value, info);
    EXPECT_EQ(actual_result, expected_result) <<
        fmt::format("Value - {}\nExpected - {}\nMessage: {}", ToString(value), expected_result, info.message);
}


UTEST_P(ValidateUserLoginTest, ValidateUserLogin)
{
    const auto& [ value, expected_result ] = GetParam();
    ErrorInfo info; bool actual_result = ValidateUserLogin(value, info);
    EXPECT_EQ(actual_result, expected_result) <<
        fmt::format("Value - {}\nExpected - {}\nMessage: {}", ToString(value), expected_result, info.message);
}


UTEST_P(ValidateUserPasswordTest, ValidateUserPassword)
{
    const auto& [ value, expected_result ] = GetParam();
    ErrorInfo info; bool actual_result = ValidateUserPassword(value, info);
    EXPECT_EQ(actual_result, expected_result) <<
        fmt::format("Value - {}\nExpected - {}\nMessage: {}", ToString(value), expected_result, info.message);
}


INSTANTIATE_UTEST_SUITE_P(
    UserValidators,
    ValidateUserNameTest,
    testing::ValuesIn(ValidateUserNameTestEntries)
);


INSTANTIATE_UTEST_SUITE_P(
    UserValidators,
    ValidateUserLastNameTest,
    testing::ValuesIn(ValidateUserLastNameTestEntries)
);


INSTANTIATE_UTEST_SUITE_P(
    UserValidators,
    ValidateUserEmailTest,
    testing::ValuesIn(ValidateUserEmailTestEntries)
);


INSTANTIATE_UTEST_SUITE_P(
    UserValidators,
    ValidateUserLoginTest,
    testing::ValuesIn(ValidateUserLoginTestEntries)
);


INSTANTIATE_UTEST_SUITE_P(
    UserValidators,
    ValidateUserPasswordTest,
    testing::ValuesIn(ValidateUserPasswordTestEntries)
);