#include "common_validators.hpp"
#include "impl/regex_cache.hpp"

#include <unicode/errorcode.h>
#include <unicode/unistr.h>
#include <userver/logging/log.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/utils/text_light.hpp>


bool tms::validators::ValidateUtf8Length(std::string_view line,
                                         const rules::ValidationRulesLength& rules,
                                         ErrorRef error_info,
                                         std::string_view field_name)
{
    std::size_t line_length;
    try
    {
        line_length = userver::utils::text::utf8::GetCodePointsCount(line);
    } catch (const std::runtime_error& ex)
    {
        if (error_info)
        {
            error_info->error_type = tms::error::kValidationError;
            error_info->message = fmt::format("Field isn't a valid utf-8 sequence.");
            impl::SetDetails(field_name, ex.what(), *error_info);
        }

        return false;
    }

    if (line_length < rules.min_length || line_length > rules.max_length)
    {
        if (error_info)
        {
            error_info->error_type = tms::error::kValidationError;
            error_info->message = fmt::format("Field does not fit in range [{}, {}]", rules.min_length, rules.max_length);
            impl::SetDetails(field_name, fmt::format("[{}, {}]", rules.min_length, rules.max_length), *error_info);
        }

        return false;
    }

    return true;
}


bool tms::validators::ValidateRegex(std::string_view line,
                                    const rules::ValidationRulesRegex& rules,
                                    ErrorRef error_info,
                                    std::string_view field_name)
{
    std::unique_ptr< icu::RegexMatcher > matcher = impl::CreateRegexMatcher(rules.pattern, error_info);

    if (!matcher)
    {
        if (error_info) {
            LOG_ERROR() << fmt::format("Error during creation of a regex matcher: {}", error_info->message);
        }

        return false;
    }

    auto input = icu::UnicodeString::fromUTF8(line);
    matcher->reset(input);

    icu::ErrorCode status;
    if (!matcher->matches(status))
    {
        if (error_info) {
            error_info->error_type = tms::error::kValidationError;
            error_info->message = "Wrong format.";
            impl::SetDetails(field_name, fmt::format("Status code - {}.", status.errorName()), *error_info);
        }

        return false;
    }

    return true;
}


bool tms::validators::ValidateField(std::optional< std::string_view > field_value,
                                    const rules::ValidationRulesField& rules,
                                    ErrorRef error_info,
                                    std::string_view field_name)
{
    return ValidateMissing(field_value, error_info, field_name) &&
           ValidateUtf8Length(*field_value, rules, error_info, field_name) &&
           ValidateRegex(*field_value, rules, error_info, field_name);
}


bool tms::validators::impl::SetDetails(std::string_view field_name, std::string_view field_value, tms::error::ErrorInfo& error_info)
{
    if (!field_name.empty())
    {
        userver::formats::json::ValueBuilder builder;
        builder.EmplaceNocheck(field_name, field_value);
        error_info.details = builder.ExtractValue();

        return true;
    }

    return false;
}


std::unique_ptr< icu::RegexMatcher > tms::validators::impl::CreateRegexMatcher(std::string_view pattern,
                                                                               ErrorRef error_info)
{
    RegexCache& cache = GetDefaultRegexCache();

    std::shared_ptr< const icu::RegexPattern > regex_pattern;
    try {
        regex_pattern = cache[pattern];
    } catch (const std::runtime_error& ex) {
        if (error_info) {
            error_info->error_type = tms::error::kValidationError;
            error_info->message = ex.what();
        }

        return nullptr;
    }

    icu::ErrorCode status;
    std::unique_ptr< icu::RegexMatcher > matcher { regex_pattern->matcher(status) };

    if (status.isFailure()) {
        if (error_info) {
            error_info->error_type = tms::error::kValidationError;
            error_info->message = fmt::format("Failed to create regex matcher, status - {}", status.errorName());
        }

        return nullptr;
    }

    UASSERT(matcher);

    return matcher;
}