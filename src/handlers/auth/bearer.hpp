#pragma once


#include <userver/server/handlers/auth/auth_checker_base.hpp>
#include <userver/server/handlers/auth/auth_checker_factory.hpp>


namespace tms::auth
{
    class CheckerFactory final : public userver::server::handlers::auth::AuthCheckerFactoryBase
    {
    public:
        userver::server::handlers::auth::AuthCheckerBasePtr operator()(const userver::components::ComponentContext& context,
                                                                       const userver::server::handlers::auth::HandlerAuthConfig& auth_config,
                                                                       const userver::server::handlers::auth::AuthCheckerSettings& auth_settings) const override;
    };
}