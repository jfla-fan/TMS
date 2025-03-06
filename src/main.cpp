#include <userver/clients/http/component.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/assert.hpp>
#include <userver/utils/daemon_run.hpp>

#include "handlers/auth/bearer.hpp"
#include "handlers/extra/echo.hpp"

#include "handlers/users/user_register.hpp"
#include "handlers/users/user_login.hpp"
#include "handlers/users/admin/user_create.hpp"
#include "handlers/users/admin/user_get.hpp"
#include "handlers/users/admin/user_update.hpp"
#include "handlers/users/admin/user_delete.hpp"

#include "handlers/tasks/task_delete.hpp"
#include "handlers/tasks/task_get.hpp"
#include "handlers/tasks/task_update.hpp"
#include "handlers/tasks/task_create.hpp"
#include "handlers/tasks/task_list.hpp"

#include "utils/hash.hpp"




int main(int argc, char* argv[]) {
  
  UINVARIANT(tms::utils::Initialize(), "Failed to initialize sodium library");

  userver::server::handlers::auth::RegisterAuthCheckerFactory("bearer", std::make_unique< tms::auth::CheckerFactory >());

  auto component_list = userver::components::MinimalServerComponentList()
                        .Append< userver::server::handlers::Ping                      >()
                        .Append< userver::components::TestsuiteSupport                >()
                        .Append< userver::components::HttpClient                      >()
                        .Append< userver::components::Postgres                        >("pg-tms-db")
                        .Append< userver::server::handlers::TestsControl              >()
                        .Append< userver::clients::dns::Component                     >()
  #ifndef NDEBUG
                        // extra
                        .Append< tms::handlers::extra::get::EchoHandler               >()
  #endif
                        // user handlers
                        .Append< tms::handlers::users::post::RegisterUserHandler      >()
                        .Append< tms::handlers::users::post::LoginUserHandler         >()
                        .Append< tms::handlers::tasks::post::CreateTaskHandler        >()
                        .Append< tms::handlers::tasks::get::GetTaskHandler            >()
                        .Append< tms::handlers::tasks::put::UpdateTaskHandler         >()
                        .Append< tms::handlers::tasks::del::DeleteTaskHandler         >()
                        .Append< tms::handlers::tasks::get::ListTasksHandler          >()

                        // admin handlers
                        .Append< tms::handlers::users::admin::post::CreateUserHandler >()
                        .Append< tms::handlers::users::admin::get::GetUserHandler     >()
                        .Append< tms::handlers::users::admin::put::UpdateUserHandler  >()
                        .Append< tms::handlers::users::admin::del::DeleteUserHandler  >();

  return userver::utils::DaemonMain(argc, argv, component_list);

}
