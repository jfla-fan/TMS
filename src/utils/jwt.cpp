#include "jwt.hpp"

#include "../models/user.hpp"

#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/utils/from_string.hpp>


namespace
{
    namespace defaults
    {
        static const std::string kSecret { "secret" };
        static const std::string kIssuer { "tms.service" };
        static const std::string kType { "JWT" };
        static const std::string kUserRole { "user_role" };
        static const std::string kUserLogin { "login" };
        static const std::string kUserEmail { "email" };
    }

    auto Verifier = jwt::verify().with_type(defaults::kType)
                                 .with_issuer(defaults::kIssuer)
                                 .with_claim(defaults::kUserRole, [](const jwt::verify_ops::verify_context<jwt::traits::kazuho_picojson>& ctx, std::error_code& ec) {
                                        auto role = ctx.get_claim(jwt::json::type::string, ec);
                                        if (ec) return;
                                        if (!tms::models::UserRoleFromString(role.as_string())) {
                                            ec = jwt::error::token_verification_error::claim_value_missmatch;
                                            return;
                                        }
                                    })
                                 .with_claim(defaults::kUserLogin, [](const jwt::verify_ops::verify_context<jwt::traits::kazuho_picojson>& ctx, std::error_code& ec) {
                                        auto role = ctx.get_claim(jwt::json::type::string, ec);
                                        if (ec) return;
                                    })
                                 .with_claim(defaults::kUserEmail, [](const jwt::verify_ops::verify_context<jwt::traits::kazuho_picojson>& ctx, std::error_code& ec) {
                                        auto role = ctx.get_claim(jwt::json::type::string, ec);
                                        if (ec) return;
                                    })
                                 .allow_algorithm(jwt::algorithm::hs256{ defaults::kSecret });
}



std::string tms::utils::GenerateJWT(const JWTAuthSettings &settings)
{
    return jwt::create().set_type(defaults::kType)
                        .set_issuer(defaults::kIssuer)
                        .set_subject(std::to_string(settings.id))
                        .set_issued_at(settings.issued_at.GetUnderlying())
                        .set_expires_at(settings.expires_at.GetUnderlying())
                        .set_payload_claim(defaults::kUserRole, picojson::value { models::ToStringView(settings.role).data() })
                        .set_payload_claim(defaults::kUserLogin, picojson::value { settings.login })
                        .set_payload_claim(defaults::kUserEmail, picojson::value { settings.email })
                        .sign(jwt::algorithm::hs256 { defaults::kSecret });
}


std::string tms::utils::GenerateJWT(const models::User& user, const std::chrono::microseconds& token_lifetime)
{
    auto time_now = userver::utils::datetime::Now();
    
    utils::JWTAuthSettings settings {
            user.id,
            user.role,
            user.login,
            user.email,
            userver::storages::postgres::TimePointTz{ time_now },
            userver::storages::postgres::TimePointTz{ time_now + token_lifetime }
    };

    return GenerateJWT(settings);
}


tms::utils::JWTAuthSettings tms::utils::DecodeJWT(const std::string& data)
{
    auto decoded = jwt::decode(data);
    Verifier.verify(decoded);

    return {
        userver::utils::FromString< models::UserId >(decoded.get_subject()),
        *models::UserRoleFromString(decoded.get_payload_claim(defaults::kUserRole).as_string()),
        decoded.get_payload_claim(defaults::kUserLogin).as_string(),
        decoded.get_payload_claim(defaults::kUserEmail).as_string(),
        userver::storages::postgres::TimePointTz{ decoded.get_issued_at() },
        userver::storages::postgres::TimePointTz{ decoded.get_expires_at() }
    };
}