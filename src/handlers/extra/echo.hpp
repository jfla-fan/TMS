#pragma once

#include <userver/server/handlers/http_handler_base.hpp>


namespace tms::handlers::extra::get
{
    class EchoHandler final : public userver::server::handlers::HttpHandlerBase
    {
    public:
        static constexpr std::string_view kName = "handler-extra-echo-get";

        EchoHandler(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& component_context);

        std::string HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                       userver::server::request::RequestContext& context) const override;
    };
}