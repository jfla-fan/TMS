#include "echo.hpp"

#include <fmt/format.h>

#include <initializer_list>
#include <userver/formats/json.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/components/component.hpp>
#include <userver/http/content_type.hpp>
#include <userver/server/handlers/http_handler_base.hpp>


namespace tms::handlers::extra::get
{

EchoHandler::EchoHandler(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& component_context)
  : HttpHandlerBase(config, component_context)
  { }


std::string EchoHandler::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                            userver::server::request::RequestContext&) const
{
  userver::formats::json::ValueBuilder builder;

  builder["MethodStr"]          = request.GetMethodStr();
  builder["HttpMajor"]          = request.GetHttpMajor();
  builder["HttpMinor"]          = request.GetHttpMinor();
  builder["URL"]                = request.GetUrl();
  builder["RequestPath"]        = request.GetRequestPath();
  builder["RequestPathSuffix"]  = request.GetPathSuffix();
  builder["Host"]               = request.GetHost();
  builder["ArgCount"]           = request.ArgCount();
  builder["PathArgCount"]       = request.PathArgCount();
  builder["ArgNames"]           = request.ArgNames();
  
  for (std::string_view name : request.ArgNames())
  {
    builder["Args"]["GetPathArg"].PushBack(request.GetPathArg(name));
    builder["Args"]["GetArg"].PushBack(request.GetArg(name));
  }

  builder["HeaderCount"] = request.HeaderCount();
  for (const auto& [name, value] : request.RequestHeaders())
  {
    std::initializer_list< std::string_view > header_item { name, value };
    builder["Headers"].PushBack(header_item);
  }

  builder["CookieCount"] = request.CookieCount();
  for (const auto& [name, value] : request.RequestCookies())
  {
    std::initializer_list< std::string_view > header_item { name, value };
    builder["Cookies"].PushBack(header_item);
  }

  request.GetHttpResponse().SetContentType(userver::http::content_type::kApplicationJson);

  return userver::formats::json::ToString(builder.ExtractValue());
}


}
