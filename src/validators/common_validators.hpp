#pragma once

#include "../utils/error.hpp"

#include "validation_rules.hpp"

#include <userver/utils/optional_ref.hpp>
#include <unicode/regex.h>


namespace tms::validators
{
    using ErrorRef = userver::utils::OptionalRef< tms::error::ErrorInfo >;

    template< class T >
    bool ValidateMissing(std::optional< T > value,
                         ErrorRef error_info = std::nullopt,
                         std::string_view field_name = {});

    bool ValidateUtf8Length(std::string_view line,
                            const rules::ValidationRulesLength& rules,
                            ErrorRef error_info = std::nullopt,
                            std::string_view field_name = {});

    bool ValidateRegex(std::string_view line,
                       const rules::ValidationRulesRegex& rules,
                       ErrorRef error_info = std::nullopt,
                       std::string_view field_name = {});

    bool ValidateField(std::optional< std::string_view > field_value,
                       const rules::ValidationRulesField& rules,
                       ErrorRef error_info = std::nullopt,
                       std::string_view field_name = {});

    namespace impl
    {
        bool SetDetails(std::string_view field_name,
                        std::string_view field_value,
                        tms::error::ErrorInfo& error_info);

        std::unique_ptr< icu::RegexMatcher > CreateRegexMatcher(std::string_view pattern,
                                                                ErrorRef error_info = std::nullopt);
    }
}


template< class T >
bool tms::validators::ValidateMissing(std::optional< T > value,
                                      ErrorRef error_info,
                                      std::string_view field_name)
{
    if (!value)
    {
        if (error_info)
        {
            error_info->error_type = tms::error::kValidationError;
            error_info->message = "Missing or incorrectly parsed field";
            impl::SetDetails(field_name, "Missing or not parsed.", *error_info);
        }

        return false;
    }

    return true;
}