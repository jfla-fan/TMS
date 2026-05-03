#include "common_validators.hpp"
#include "validation_rules.hpp"

#include <array>

#include <userver/utest/utest.hpp>
#include <userver/utils/optionals.hpp>


using namespace tms::validators;
using tms::error::ErrorInfo;
using userver::utils::ToString;


namespace
{
    struct ValidateUtf8LengthTestEntry
    {
        std::string_view value;
        rules::ValidationRulesLength rules;
        bool expected_result;
    };

    constexpr std::array< ValidateUtf8LengthTestEntry, 20 > ValidateUtf8LengthTestEntries =
    {
        // Correct entries
        ValidateUtf8LengthTestEntry
        {"some",            {4, 4},     true}, // Exact minimum length
        {"valid",           {4, 10},    true}, // Within range
        {"valid string",    {1, 20},    true}, // Within range
        {"中文",            {1, 2},     true}, // Unicode within range
        {"Élodie",          {6, 6},     true}, // Unicode within range
        {"🤖",             {1, 10},    true}, // Emoji
        {"a",               {1, 1},     true}, // Single character
        {"12345",           {5, 5},     true}, // Exact maximum length
        {"   ",             {3, 3},     true}, // Only spaces
        {"\n\t\r",          {3, 3},     true}, // Whitespace characters

        // Incorrect entries
        {"",                            {1, 10}, false}, // Empty string (below minimum)
        {"abc",                         {4, 10}, false}, // Below minimum
        {"this is a very long string",  {1, 10}, false}, // Above maximum
        {"\xF0\x28\x8C\x28",            {1, 10}, false}, // Invalid UTF-8 sequence
        {"中文",                         {1, 1}, false}, // Unicode above maximum
        {"🤖",                          {0, 0}, false}, // Emoji above maximum
        {"a",                           {2, 10}, false}, // Single character below minimum
        {"12345678901",                 {1, 10}, false}, // Above maximum
        {"   ",                         {4, 10}, false}, // Spaces below minimum
        {"\n\t\r",                      {4, 4},  false} // below minimum
    };

    struct ValidateRegexTestEntry
    {
        std::string_view value;
        rules::ValidationRulesRegex rules;
        bool expected_result;
    };

    const std::array< ValidateRegexTestEntry, 10 > ValidateRegexTestEntries =
    {
        // Correct entries
        ValidateRegexTestEntry
            {"hello",       {{R"(^[a-z]+$)"}},      true }, // All lowercase letters
        {"HELLO",       {{R"(^[A-Z]+$)"}},      true }, // All uppercase letters
        {"12345",       {{R"(^\d+$)"}},         true }, // All digits
        {"hello123",    {{R"(^[a-z0-9]+$)"}},   true }, // Letters and digits
        {"hello_world", {{R"(^[a-z_]+$)"}},     true }, // Letters and underscores

        // Incorrect entries
        {"hello!",      {{R"(^[a-z]+$)"}},      false }, // Contains special character
        {"HELLO123",    {{R"(^[A-Z]+$)"}},      false }, // Contains digits
        {"123 456",     {{R"(^\d+$)"}},         false }, // Contains space
        {"",            {{R"(^[a-z]+$)"}},      false }, // Empty string
        {"hello world", {{R"(^[a-z]+$)"}},      false } // Contains space
    };

    struct ValidateFieldTestEntry
    {
        std::optional< std::string_view > value;
        rules::ValidationRulesField rules;
        bool expected_result;
    };

    const std::array< ValidateFieldTestEntry, 10 > ValidateFieldTestEntries =
    {
        // Correct entries
        ValidateFieldTestEntry
        { "hello",                      { {4, 20}, {  {R"(^[a-z]+$)"    } } },    true  }, // Valid length and regex
        { "HELLO",                      { {4, 20}, { {R"(^[A-Z]+$)"     } } },    true  }, // Valid length and regex
        { "12345",                      { {4, 20}, { {R"(^\d+$)"        } } },    true  }, // Valid length and regex
        { "hello123",                   { {4, 20}, { {R"(^[a-z0-9]+$)"  } } },    true  }, // Valid length and regex
        { "hello_world",                { {4, 20}, { {R"(^[a-z_]+$)"    } } },    true  }, // Valid length and regex
    
        // Incorrect entries
        { std::nullopt,                 { {4, 20}, { {R"(^[a-z]+$)" } } },        false }, // Missing value
        { "abc",                        { {4, 20}, { {R"(^[a-z]+$)" } } },        false }, // Below minimum length
        { "this is a very long string", { {4, 20}, { {R"(^[a-z]+$)" } } },        false }, // Above maximum length
        { "hello!",                     { {4, 20}, { {R"(^[a-z]+$)" } } },        false }, // Invalid regex
        { "\xF0\x28\x8C\x28",           { {4, 20}, { {R"(^[a-z]+$)" } } },        false } // Invalid UTF-8
    };
}


class ValidateUtf8LengthTest : public ::testing::TestWithParam< ValidateUtf8LengthTestEntry >   { };
class ValidateRegexTest      : public ::testing::TestWithParam< ValidateRegexTestEntry >        { };
class ValidateFieldTest      : public ::testing::TestWithParam< ValidateFieldTestEntry >        { };


UTEST(CommonValidators, ValidateMissingNull)
{
    EXPECT_FALSE(ValidateMissing< std::string_view >(std::nullopt));
}


UTEST(CommonValidators, ValidateMissingNotNull)
{
    EXPECT_TRUE(ValidateMissing< std::string_view >(""));
}


UTEST_P(ValidateUtf8LengthTest, ValidateUtf8Length)
{
    const auto& [ value, rules, expected_result ] = GetParam();
    ErrorInfo info;
    EXPECT_EQ(ValidateUtf8Length(value, rules, info), expected_result)
        << fmt::format("Value - {}\nRange - [{}, {}]\nMessage: {}\nExpected: {}", value,
                                                                         rules.min_length,
                                                                         rules.max_length,
                                                                         info.message,
                                                                         expected_result);
}


UTEST_P(ValidateRegexTest, ValidateRegex)
{
    const auto& [ value, rules, expected_result ] = GetParam();
    ErrorInfo info;
    EXPECT_EQ(ValidateRegex(value, rules, info), expected_result) <<
        fmt::format("Value - {}\nRegex - {}\nMessage: {}\nExpected - {}", value, rules.pattern, info.message, expected_result);
}


UTEST_P(ValidateFieldTest, ValidateField)
{
    const auto& [ value, rules, expected_result ] = GetParam();
    ErrorInfo info;
    EXPECT_EQ(ValidateField(value, rules, info, "field"), expected_result)
        << fmt::format("Value - {}\nRules: range [{}, {}], regex {}\nMessage: {}\nExpected: {}",
            ToString(value),
            rules.min_length,
            rules.max_length,
            rules.pattern,
            info.message,
            expected_result);
}


INSTANTIATE_UTEST_SUITE_P(
    CommonValidators,
    ValidateUtf8LengthTest,
    ::testing::ValuesIn(ValidateUtf8LengthTestEntries)
);


// I don't really know why, but I'm getting SEGV with all the 3 suits instantiated.
// in debug mode (make test-debug). Release works fine without any magic.
// With one suite excluded or with a dummy test below everything works fine.
// If you have any idea what's going on, I would gladly hear it.
UTEST(CommonValidators, Dummy)
{
    GTEST_SKIP();
}


INSTANTIATE_UTEST_SUITE_P(
    CommonValidators,
    ValidateRegexTest,
    ::testing::ValuesIn(ValidateRegexTestEntries)
);


INSTANTIATE_UTEST_SUITE_P(
    CommonValidators,
    ValidateFieldTest,
    ::testing::ValuesIn(ValidateFieldTestEntries)
);