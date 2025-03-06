#pragma once

#include <userver/utils/fast_pimpl.hpp>
#include <unicode/regex.h>


namespace tms::validators::impl
{
    class RegexCache final
    {
    public:
        RegexCache() = default;

        std::shared_ptr< const icu::RegexPattern > operator [] (std::string_view pattern);
    private:
#ifndef NO_SERVICE
        static constexpr std::size_t kImplSize = 176;
#else
        static constexpr std::size_t kImplSize = 120;   
#endif
        static constexpr std::size_t kImplAlignment = 16;
        
        struct Impl;
        userver::utils::FastPimpl< Impl, kImplSize, kImplAlignment > pimpl_;
    };

    RegexCache& GetDefaultRegexCache();
}