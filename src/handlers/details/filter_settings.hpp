#include <optional>
#include <vector>

#include "models/user_fwd.hpp"
#include <models/task_fwd.hpp>

#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/server/http/http_request.hpp>


namespace tms::handlers::impl
{
    /// @todo change container types to boost::small_vector
    struct FilterSettings
    {
        std::optional< std::vector< tms::models::ETaskStatus > >     statuses;
        std::optional< std::vector< tms::models::ETaskPriority > >   priorities;
        std::optional< std::vector< std::string_view > >             categories;
        std::optional< userver::storages::postgres::TimePointTz >    deadline_start;
        std::optional< userver::storages::postgres::TimePointTz >    deadline_end;
        int                                                          page;
        int                                                          limit;
    };

    struct FilterSettings2 : FilterSettings
    {
        std::optional< std::vector< tms::models::UserId > > user_ids;
    };

    template< class Settings >
    Settings ParseFromRequest(const userver::server::http::HttpRequest& request);

    template< class Settings >
    void LogFilterSettingsDebug(const Settings& filters);
}