#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include <userver/formats/json.hpp>


namespace tms::handlers::users::admin::post
{
    class CreateUserHandler final : public userver::server::handlers::HttpHandlerJsonBase
    {
    public:
        static constexpr std::string_view kName = "handler-admin-user-post";

        CreateUserHandler(const userver::components::ComponentConfig& config,
                          const userver::components::ComponentContext& component_context);

        userver::formats::json::Value HandleRequestJsonThrow(const userver::server::http::HttpRequest& request,
                                                             const userver::formats::json::Value& request_json,
                                                             userver::server::request::RequestContext& context) const override final;

    private:
        userver::storages::postgres::ClusterPtr pg_cluster_;
    };
}