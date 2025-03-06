#include "hash.hpp"

#include <array>
#include <fmt/format.h>
#include <sodium.h>


namespace tms::utils
{
    bool Initialize()
    {
        return sodium_init() >= 0;
    }

    std::string GenerateArgon2idHash(std::string_view password)
    {
        static constexpr std::size_t kHashStringSize = crypto_pwhash_STRBYTES;

        if (password.length() > crypto_pwhash_PASSWD_MAX) // unlikely
        {
            throw std::runtime_error(fmt::format("Password length must be within range [{} : {})",
                                                      crypto_pwhash_PASSWD_MIN, crypto_pwhash_PASSWD_MAX));
        }

        std::array< char, kHashStringSize > hash_string {};
        if (crypto_pwhash_str(hash_string.data(),
                              password.data(),
                              password.size(),
                              crypto_pwhash_OPSLIMIT_SENSITIVE,
                              crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
            throw std::runtime_error("Failed to hash password using Argon2id. Out of memory?");
        }

        return std::string(hash_string.data());
    }


    bool VerifyArgon2idPassword(std::string_view hash, std::string_view password)
    {
        return crypto_pwhash_str_verify(hash.data(), password.data(), password.length()) == 0;
    }
}