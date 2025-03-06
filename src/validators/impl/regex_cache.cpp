#include "regex_cache.hpp"

#include <memory>

#include <fmt/format.h>

#include <unicode/errorcode.h>
#include <unicode/parseerr.h>
#include <unicode/regex.h>
#include <unicode/unistr.h>

#include <userver/utils/assert.hpp>
#include <userver/utils/str_icase.hpp>

#ifndef NO_SERVICE
#include <userver/rcu/rcu_map.hpp>
#else
#include <boost/unordered_map.hpp>
#endif


using RegexCache = tms::validators::impl::RegexCache;

struct RegexCache::Impl
{
    struct Entry
    {
        Entry(icu::UnicodeString&& str, std::shared_ptr< icu::RegexPattern> pattern)
            : original_string(std::move(str))
            , regex_pattern(std::move(pattern))
        { }

        icu::UnicodeString original_string; // need to keep it alive as long as the pattern itself
        std::shared_ptr< icu::RegexPattern > regex_pattern;
    };

// #define NO_SERVICE

#ifndef NO_SERVICE
    using RegexMap = userver::rcu::RcuMap< std::string_view, Entry >;
    using ValuePtr = RegexMap::ValuePtr;
    using ConstValuePtr = RegexMap::ConstValuePtr;
#else
    using RegexMap = boost::unordered_map< std::string, Entry, userver::utils::StrCaseHash, std::equal_to<> >;
    using ValuePtr = Entry;
    using ConstValuePtr = const Entry&;
#endif

    RegexMap regex_map;

    auto GetOrCreateValue(std::string_view key) -> ConstValuePtr
    {
#ifndef NO_SERVICE
        RegexMap::ConstValuePtr value_ptr = regex_map.Get(key);
        if (value_ptr) {
            return value_ptr;
        }

        return regex_map.Insert(key, CreateRegexPattern(key)).value;
#else
        auto it = regex_map.find(key, regex_map.hash_function(), regex_map.key_eq());
        if (it == regex_map.end())
        {
            auto inserted = regex_map.emplace(std::pair { std::string(key), CreateRegexPattern(key) });
            it = inserted.first;
        }

        return it->second;
#endif
    }

    ValuePtr CreateRegexPattern(std::string_view pattern)
    {
        UParseError parse_error;
        icu::ErrorCode status;

        auto original_string = icu::UnicodeString::fromUTF8(pattern);
        auto regex_pattern = std::shared_ptr< icu::RegexPattern >
                                {
                                    icu::RegexPattern::compile(original_string,
                                                                 0,
                                                                 parse_error,
                                                                 status)
                                };

        if (status.isFailure()) {
            throw std::runtime_error(fmt::format("Regex pattern status error - {}", status.errorName()));
        }

        UASSERT(regex_pattern);

#ifndef NO_SERVICE
        return std::make_shared< Entry >(std::move(original_string), std::move(regex_pattern));
#else
        return { std::move(original_string), std::move(regex_pattern) };
#endif
    }
};


std::shared_ptr< const icu::RegexPattern > RegexCache::operator [] (std::string_view pattern)
{
#ifndef NO_SERVICE
    return pimpl_->GetOrCreateValue(pattern)->regex_pattern;
#else
    return pimpl_->GetOrCreateValue(pattern).regex_pattern;
#endif
}


RegexCache& tms::validators::impl::GetDefaultRegexCache()
{
    // @todo need to have some coroutine-friendly cache initialization mechanism
    static RegexCache cache;
    return cache;
}