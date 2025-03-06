#pragma once

#include "user_fwd.hpp"

#include <userver/logging/log.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/io/enum_types.hpp>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/utils/strong_typedef.hpp>
#include <userver/utils/trivial_map.hpp>


namespace tms::models
{
    enum class ETaskStatus : int32_t
    {
        kPending,
        kInProgress,
        kCompleted,
        kExpired,
        
        // special
        kCount,
    };

    enum class ETaskPriority : int32_t
    {
        kLow,
        kNormal,
        kUrgent,

        // special
        kCount,
    };


    inline constexpr std::string_view ToStringView(ETaskStatus status);
    inline constexpr std::optional< ETaskStatus > TaskStatusFromString(std::string_view status);
    inline constexpr std::string_view ToStringView(ETaskPriority priority);
    inline constexpr std::optional< ETaskPriority > TaskPriorityFromString(std::string_view priority);

    using TaskId = std::int32_t;
    struct Task
    {
        TaskId id;
        std::string title;
        std::string description;
        std::string category;
        ::userver::storages::postgres::TimePointTz deadline;
        ETaskPriority priority;
        ETaskStatus status;
        UserId user_id;
        ::userver::storages::postgres::TimePointTz created_at;
        ::userver::storages::postgres::TimePointTz updated_at;
    };

    ETaskStatus Parse(const userver::formats::json::Value& value, userver::formats::parse::To< ETaskStatus >);
    std::optional< ETaskStatus > Parse(const userver::formats::json::Value&, userver::formats::parse::To< std::optional< ETaskStatus > >);
    userver::formats::json::Value Serialize(const Task& task, userver::formats::serialize::To< userver::formats::json::Value >);
}

using tms::models::TaskId;

template<>
struct userver::storages::postgres::io::CppToUserPg<tms::models::ETaskStatus>
{
    static constexpr DBTypeName postgres_name = "tms.task_status";
    static constexpr userver::utils::TrivialBiMap enumerators =
        [] (auto selector)
        {
            using ::tms::models::ETaskStatus;
            static_assert(userver::utils::UnderlyingValue(ETaskStatus::kCount) == 4, "Add or remove selector cases if you change the enum.");

            return selector()
                    .Case("pending",        ETaskStatus::kPending)
                    .Case("in_progress",    ETaskStatus::kInProgress)
                    .Case("completed",      ETaskStatus::kCompleted)
                    .Case("expired",        ETaskStatus::kExpired);
        };
};


template<>
struct userver::storages::postgres::io::CppToUserPg< tms::models::ETaskPriority >
{
    static constexpr DBTypeName postgres_name = "tms.task_priority";
    static constexpr userver::utils::TrivialBiMap enumerators =
        [] (auto selector)
        {
            using ::tms::models::ETaskPriority;
            static_assert(userver::utils::UnderlyingValue(ETaskPriority::kCount) == 3, "Add or remove selector cases if you change the enum.");

            return selector()
                    .Case("low",    ETaskPriority::kLow)
                    .Case("normal", ETaskPriority::kNormal)
                    .Case("urgent", ETaskPriority::kUrgent);
        };
};


template<>
struct fmt::formatter< tms::models::ETaskPriority >
{
    template<typename ParseContext>
    constexpr static auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const tms::models::ETaskPriority& priority, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}", tms::models::ToStringView(priority));
    }
};


template<>
struct fmt::formatter< tms::models::ETaskStatus >
{
    template<typename ParseContext>
    constexpr static auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const tms::models::ETaskStatus& status, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}", tms::models::ToStringView(status));
    }
};


inline constexpr std::string_view tms::models::ToStringView(ETaskStatus status)
{
    return userver::storages::postgres::io::CppToUserPg<tms::models::ETaskStatus>::enumerators.TryFind(status).value();

    // using tms::models::ETaskStatus;

    // static std::unordered_map< ETaskStatus, std::string_view > s_map
    // {
    //     { ETaskStatus::kPending, "pending" },
    //     { ETaskStatus::kInProgress, "in_progress" },
    //     { ETaskStatus::kCompleted, "completed"},
    //     { ETaskStatus::kExpired, "expired"}
    // };

    // return s_map[status];
}

inline constexpr std::optional< tms::models::ETaskStatus > tms::models::TaskStatusFromString(std::string_view status)
{
    return userver::storages::postgres::io::CppToUserPg<tms::models::ETaskStatus>::enumerators.TryFind(status);
}

inline constexpr std::string_view tms::models::ToStringView(ETaskPriority priority)
{
    return userver::storages::postgres::io::CppToUserPg<tms::models::ETaskPriority>::enumerators.TryFind(priority).value();

    // using tms::models::ETaskPriority;

    // static std::unordered_map< ETaskPriority, std::string_view > s_map
    // {
    //     { ETaskPriority::kLow, "low" },
    //     { ETaskPriority::kNormal, "normal" },
    //     { ETaskPriority::kUrgent, "urgent"}
    // };

    // return s_map[priority];
}

inline constexpr std::optional< tms::models::ETaskPriority > tms::models::TaskPriorityFromString(std::string_view priority)
{
    return userver::storages::postgres::io::CppToUserPg<tms::models::ETaskPriority>::enumerators.TryFind(priority);
}