#pragma once

#include <cstdint>


namespace tms::models
{
    enum class ETaskStatus : int32_t;
    enum class ETaskPriority : int32_t;
    using TaskId = int32_t;
    struct Task;
}