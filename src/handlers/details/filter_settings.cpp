#include "filter_settings.hpp"

#include "models/task.hpp"

#include <sstream>

#include <userver/logging/log.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/utils/from_string.hpp>
#include <userver/utils/text_light.hpp>


using userver::storages::postgres::TimePointTz;


namespace
{
    std::optional< TimePointTz > ToOptionalTimePoint(const std::string& duration)
    {
        if (duration.empty())
        {
            return std::nullopt;
        }

        auto timePoint = userver::utils::datetime::OptionalStringtime(duration); 
        if (!timePoint)
        {
            LOG_WARNING() << "ToOptionalTimePoint: failed to parse duration";
            return std::nullopt;
        }

        return timePoint ? std::make_optional(TimePointTz{ *timePoint }) : std::nullopt;
    }

    std::optional< int > ToOptionalInt(std::string_view value)
    {
        try {
            return userver::utils::FromString< int >(value);
        } catch (const std::runtime_error& ex) {
            LOG_WARNING() << fmt::format("ToOptionalInt, failed to parse, details: {}", ex.what());
            return std::nullopt;
        }
    }

    int ToIntOrDefault(std::string_view value, int default_value)
    {
        try
        {
           return userver::utils::FromString< int >(value); 
        } catch (...) {
            return default_value;
        }
    }

    template< typename ContainerResult, typename ContainerSource, typename Transform >
    std::optional< ContainerResult > ToContainerConvert(ContainerSource&& source, Transform&& func, std::string_view definition)
    {
        ContainerResult result;
        for (const auto& item : source)
        {
            auto transformed = func(item);
            if (!transformed.has_value())
            {
                LOG_WARNING() << fmt::format("ToContainerConvert - failed to parse {} (value - {})", definition, item);
                continue;
            }

            result.push_back(*transformed);
        }

        return !result.empty() ? std::make_optional(result) : std::nullopt;
    }

    template< class T >
    inline void LogSettingsValueDebug(const std::optional< std::vector< T > >& values, std::string_view definition)
    {
        if (!values)
        {
            LOG_DEBUG() << fmt::format("No {} specified", definition);
            return;
        }

        std::ostringstream result(fmt::format("{}: ", definition));
        for (const auto& value : *values)
        {
            result << fmt::format("{} ", value);
        }

        LOG_DEBUG() << result.str();
    }

    template< class T >
    inline void LogSettingsValueDebug(const std::optional< T >& value, std::string_view definition)
    {
        if (!value)
        {
            LOG_DEBUG() << fmt::format("No {} specified", definition);
            return;
        }

        LOG_DEBUG() << fmt::format("{}: {}", definition, *value);
    }

    template< class T >
    inline void LogSettingsValueDebug(const T& value, std::string_view definition)
    {
        LOG_DEBUG() << fmt::format("{}: {}", definition, value);
    }
}


namespace tms::handlers::impl
{
    template<> FilterSettings ParseFromRequest(const userver::server::http::HttpRequest& request)
    {
        using userver::utils::text::SplitIntoStringViewVector;

        FilterSettings result;

        const auto status_converter   = [&](std::string_view value) { return ::tms::models::TaskStatusFromString(value); };
        const auto priority_converter = [&](std::string_view value) { return ::tms::models::TaskPriorityFromString(value); };
        const auto category_converter = [&](std::string_view value) { return value.empty() ? std::nullopt : std::make_optional< std::string_view >(value); };

        result.statuses         = ToContainerConvert< decltype(FilterSettings::statuses)::value_type   >(SplitIntoStringViewVector(request.GetArg("statuses"), ","),
                                                                                                         status_converter, "status");
        result.priorities       = ToContainerConvert< decltype(FilterSettings::priorities)::value_type >(SplitIntoStringViewVector(request.GetArg("priorities"), ","),
                                                                                                         priority_converter, "priority");
        result.categories       = ToContainerConvert< decltype(FilterSettings::categories)::value_type >(SplitIntoStringViewVector(request.GetArg("categories"), ","),
                                                                                                         category_converter, "category");
        result.deadline_start   = ToOptionalTimePoint(request.GetArg("deadline_start"));
        result.deadline_end     = ToOptionalTimePoint(request.GetArg("deadline_end"));
        result.page             = ToIntOrDefault(request.GetArg("page"), 1);
        result.limit            = ToIntOrDefault(request.GetArg("limit"), 10);

        return result;
    }

    template<> FilterSettings2 ParseFromRequest(const userver::server::http::HttpRequest& request)
    {
        using userver::utils::text::SplitIntoStringViewVector;

        FilterSettings2 result { ParseFromRequest< FilterSettings >(request), std::nullopt };

        result.user_ids = ToContainerConvert< decltype(FilterSettings2::user_ids)::value_type >(SplitIntoStringViewVector(request.GetArg("user_ids"), ","),
                                                                                                ToOptionalInt, "user_id");

        return result;
    }

    template<>
    void LogFilterSettingsDebug(const FilterSettings &filters)
    {
        LogSettingsValueDebug(filters.statuses, "statuses");
        LogSettingsValueDebug(filters.priorities, "priorities");
        LogSettingsValueDebug(filters.categories, "categories");
        // #todo add formatters for TimePointTz
        // LogSettingsValueDebug(filters.deadline_start, "deadline_start");
        // LogSettingsValueDebug(filters.deadline_end, "deadline_end");
        LogSettingsValueDebug(filters.page, "page");
        LogSettingsValueDebug(filters.limit, "limit");
    }

    template<>
    void LogFilterSettingsDebug(const FilterSettings2 &filters)
    {
        LogSettingsValueDebug(filters.user_ids, "user_ids");
        LogFilterSettingsDebug< FilterSettings >(filters);
    }
}