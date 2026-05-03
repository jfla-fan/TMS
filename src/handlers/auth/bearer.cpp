#include "bearer.hpp"

#include "../../db/queries.hpp"
#include "../../models/user.hpp"
#include "../../utils/jwt.hpp"

#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/auth/user_scopes.hpp>
#include <userver/server/handlers/auth/auth_checker_base.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include <userver/storages/postgres/postgres.hpp>
#include <userver/storages/postgres/query.hpp>
#include <userver/storages/query.hpp>
#include <userver/http/common_headers.hpp>

#include <algorithm>


namespace tms::auth
{

class AuthCheckerBearer final : public userver::server::handlers::auth::AuthCheckerBase
{
public:
    using AuthCheckResult = userver::server::handlers::auth::AuthCheckResult;

    AuthCheckerBearer(userver::server::auth::UserScopes scopes, const userver::components::ComponentContext& component_context)
        : scopes_(std::move(scopes))
        , pg_cluster_(component_context.FindComponent<userver::components::Postgres>("pg-tms-db")
                                       .GetCluster()) { }

    [[nodiscard]] AuthCheckResult CheckAuth(const userver::server::http::HttpRequest& request,
                                            userver::server::request::RequestContext& request_context) const override;

    [[nodiscard]] bool SupportsUserAuth() const noexcept override { return true; }

private:
    userver::server::auth::UserScopes scopes_;
    userver::storages::postgres::ClusterPtr pg_cluster_;
};


AuthCheckerBearer::AuthCheckResult AuthCheckerBearer::CheckAuth(const userver::server::http::HttpRequest& request,
                                                                userver::server::request::RequestContext& request_context) const
{
    const auto& auth_value = request.GetHeader(userver::http::headers::kAuthorization);
    if (auth_value.empty())
    {
        return AuthCheckResult {
            AuthCheckResult::Status::kTokenNotFound,
            {},
            "Empty <Authorization> header.",
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }

    const auto bearer_start_pos = auth_value.find(' '); // "Authorization: Bearer <some_token>" header format
    if (bearer_start_pos == std::string::npos ||
        std::string_view { auth_value.data(), bearer_start_pos } != "Bearer")
    {
        return AuthCheckResult {
            AuthCheckResult::Status::kTokenNotFound,
            {},
            "Authorization header must have 'Bearer <token>' format.",
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }

    std::string token { auth_value.data() + bearer_start_pos + 1 };
    LOG_INFO() << fmt::format("Ready to parse token - {}", token);

    utils::JWTAuthSettings parsedTokenData;
    try
    {
        parsedTokenData = utils::DecodeJWT(token);
    } catch (const std::exception& ex)
    {
        LOG_WARNING() << fmt::format("Token parsing error: {}.", ex.what());

        return AuthCheckResult {
            AuthCheckResult::Status::kInvalidToken,
            {},
            fmt::format("Token parsing error: {}.", ex.what()),
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }

    // @todo add cache later
    auto result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,  tms::queries::kFindUserById, parsedTokenData.id);
    if (result.IsEmpty())
    {
        return AuthCheckResult {
            AuthCheckResult::Status::kTokenNotFound,
            {},
            "User doesn't exist. Invalid token.",
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }

    // try/catch?
    {
        auto user = result.AsSingleRow< tms::models::User >(userver::storages::postgres::kRowTag);

        // check scopes
        std::string_view user_scope = tms::models::ToStringView(user.role);
        bool contains_scope = std::find(scopes_.cbegin(), scopes_.cend(), user_scope) != scopes_.cend();

        if (!contains_scope)
        {
            return AuthCheckResult {
                AuthCheckResult::Status::kForbidden,
                {},
                fmt::format("No {} permission.", user_scope),
                userver::server::handlers::HandlerErrorCode::kForbidden
            };
        }

        LOG_INFO() << fmt::format("Authorization for user {} is passed.", user_scope);
    }

    request_context.SetData(std::string { utils::JWTAuthSettings::kRequestContextKey }, parsedTokenData);

    return {};
}


userver::server::handlers::auth::AuthCheckerBasePtr CheckerFactory::operator()(const userver::components::ComponentContext& context,
                                                                               const userver::server::handlers::auth::HandlerAuthConfig& auth_config,
                                                                               const userver::server::handlers::auth::AuthCheckerSettings&) const
{
    return std::make_shared< AuthCheckerBearer >(auth_config["scopes"].As< userver::server::auth::UserScopes >(), context);
}

}
