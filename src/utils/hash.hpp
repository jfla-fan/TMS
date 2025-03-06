#pragma once

#include <string>


namespace tms::utils
{
    bool Initialize();
    std::string GenerateArgon2idHash(std::string_view password);
    bool VerifyArgon2idPassword(std::string_view hash, std::string_view password);
}