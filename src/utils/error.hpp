#pragma once

#include <string>

#include <userver/utils/optional_ref.hpp>
#include <userver/formats/json.hpp>


namespace tms::error
{
    enum EErrorType : uint64_t
    {
        kValidationError = 0,

        kConflictError = 30,
        
        kAuthError = 60,
        
        kServerError = 100,
        
        kJWTError = 150,
        kJWTParseError,
        kJWTGenerateError,
        
        kUserError = 160,
        kUserNotFoundError,

        kTaskError = 260,
        kTaskNotFoundError,

        kUnknown = std::numeric_limits< uint64_t >::max(),
        kNone = kUnknown - 1,

        // special
        kErrorCount = 13
    };

    std::string_view ToStringView(EErrorType error_type);
    std::optional< EErrorType > ErrorTypeFromString(std::string_view error_type);

    struct ErrorInfo
    {
        EErrorType error_type { kNone };
        std::string message {};
        userver::formats::json::Value details {};
    };

    userver::formats::json::Value MakeErrorJson(ErrorInfo&& message);
    userver::formats::json::Value MakeErrorJson(EErrorType error_type, std::string message, userver::formats::json::Value details = {});

    userver::formats::json::Value Serialize(const EErrorType& error_type, userver::formats::serialize::To< userver::formats::json::Value >);
}


template<>
struct fmt::formatter< tms::error::EErrorType >
{
    template<typename ParseContext>
    constexpr static auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const tms::error::EErrorType& error_type, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}", tms::error::ToStringView(error_type));
    }
};