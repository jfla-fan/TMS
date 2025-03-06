#include "error.hpp"

#include <userver/formats/json/value.hpp>
#include <userver/utils/trivial_map.hpp>


namespace
{
    constexpr userver::utils::TrivialBiMap ErrorTypeToStringMap =
        [] (auto selector) {
            using namespace tms::error;
            static_assert(kErrorCount == 13, "Change this map after changing the enum.");

            return selector()
                    .Case(kValidationError, "Validation")
                    .Case(kConflictError, "Conflict")
                    .Case(kAuthError, "Auth")
                    .Case(kServerError, "Server")
                    .Case(kJWTError, "JWTError")
                    .Case(kJWTParseError, "JWTParse")
                    .Case(kJWTGenerateError, "JWTGenerate")
                    .Case(kUserError, "UserError")
                    .Case(kUserNotFoundError, "UserNotFoundError")
                    .Case(kTaskError, "TaskError")
                    .Case(kTaskNotFoundError, "TaskNotFoundError")
                    .Case(kUnknown, "Unkwnon")
                    .Case(kNone, "None");
        };
}


std::string_view tms::error::ToStringView(EErrorType error_type)
{
    return ErrorTypeToStringMap.TryFind(error_type).value();
}


std::optional< tms::error::EErrorType > tms::error::ErrorTypeFromString(std::string_view error_type)
{
    return ErrorTypeToStringMap.TryFind(error_type);
}


userver::formats::json::Value tms::error::MakeErrorJson(ErrorInfo&& message)
{
    userver::formats::json::ValueBuilder builder;

    builder["code"] = message.error_type;
    builder["message"] = message.message;
    builder["details"] = message.details;

    return builder.ExtractValue();
}


userver::formats::json::Value tms::error::MakeErrorJson(EErrorType error_type, std::string message, userver::formats::json::Value details)
{
    return MakeErrorJson({ error_type, std::move(message), std::move(details) });
}


userver::formats::json::Value tms::error::Serialize(const EErrorType& error_type, userver::formats::serialize::To< userver::formats::json::Value >)
{
    return userver::formats::json::ValueBuilder { ToStringView(error_type) }.ExtractValue();
}
