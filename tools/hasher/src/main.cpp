#include <boost/program_options/errors.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/throw_exception.hpp>
#include <userver/utils/str_icase.hpp>
#include <boost/program_options.hpp>
#include <boost/unordered_map.hpp>
#include <fmt/format.h>

#include "utils/error.hpp"
#include "utils/hash.hpp"
#include "validators/user_validators.hpp"

#include <cassert>
#include <iostream>
#include <map>


namespace po = boost::program_options;

using tms::error::ErrorInfo;
using tms::validators::ErrorRef;

using userver::utils::StrIcaseHash;
using userver::utils::StrIcaseEqual;


enum EAction
{
    eA_Hash,
    eA_Validate,
    eA_Unknown
};

EAction FromString(std::string_view action)
{
    static boost::unordered_map< std::string, EAction, StrIcaseHash, StrIcaseEqual > action_map =
    {
        { "generate", eA_Hash },
        { "gen", eA_Hash },
        { "validate", eA_Validate },
        { "val", eA_Validate },
    };

    auto it = action_map.find(action, action_map.hash_function(), action_map.key_eq());
    return it != action_map.end() ? it->second : EAction::eA_Unknown;
}

namespace actions
{
    namespace impl
    {
        [[maybe_unused]] std::pair< bool, ErrorInfo > ValidatePassword(std::string_view password)
        {
            ErrorInfo info;
            bool is_valid = false;

            try {
                is_valid = tms::validators::ValidateUserPassword(password, info);
            } catch (const std::exception& ex) {
                std::cout << fmt::format("Exception caught during validation of user password: {}\n", ex.what());
            }

            if (!is_valid)
            {
                std::cout << fmt::format("Password validation failed, reason: {}\n", info.message);
            }
            else
            {
                std::cout << fmt::format("Password validation successfull\n");
            }

            return { is_valid, std::move(info) };
        }

        std::optional< std::string > GenerateHash(std::string_view password)
        {
            std::string hash;
            try {
                hash = tms::utils::GenerateArgon2idHash(password);        
            } catch (const std::runtime_error& ex) {
                std::cout << fmt::format("Exception caught during generating argon2id hash: {}\n", ex.what());
                return std::nullopt;
            }

            return hash;
        }
    }

    void ShowUsage(const po::options_description& desc)
    {
        std::cout << "Usage: hash_tool <command> [options]\n\n"
                  << "Commands:\n"
                  << "\tgenerate (gen):  Generate a hash from a password\n"
                  << "\tvalidate (val):  Validate a password against a hash\n"
                  << desc << std::endl;
    }
    
    void UnrecognizedCommand(std::string_view command, const po::options_description& desc)
    {
        std::cout << fmt::format("Unrecognized command - \"{}\"\n", command);
        ShowUsage(desc);
    }

    void GenerateHash(const std::vector< std::string >& args)
    {
        po::options_description generate_desc("Generate hash options");
        generate_desc.add_options()
            ("force,f", "Force hash if validation failed.")
            ("password,p", po::value< std::string >()->required());
        
        po::variables_map generate_vm;
        po::store(po::command_line_parser(args).options(generate_desc).run(), generate_vm);

        if (generate_vm.count("help"))
        {
            std::cout << generate_desc << '\n';
            return;
        }

        po::notify(generate_vm);

        std::string password = generate_vm["password"].as< std::string >();
        auto [ is_valid, _ ] = impl::ValidatePassword(password);
        
        if (!is_valid && !generate_vm.count("force"))
        {
            return;
        }

        std::optional hash = impl::GenerateHash(password);

        if (hash)
        {
            std::cout << fmt::format("Generated hash: {}\n", *hash);
        }
    }

    void ValidatePassword(const std::vector< std::string >& args)
    {
        po::options_description validate_desc("Validate password options");
        validate_desc.add_options()
            ("password,p", po::value< std::string >()->required());
        
        po::variables_map validate_vm;
        po::store(po::command_line_parser(args).options(validate_desc).run(), validate_vm);

        if (validate_vm.count("help"))
        {
            std::cout << validate_desc << '\n';
            return;
        }

        po::notify(validate_vm);

        std::string password = validate_vm["password"].as< std::string >();
        impl::ValidatePassword(password);
    }
}


int main(int argc, char* argv[])
{
    if (!tms::utils::Initialize())
    {
        std::cout << "Failed to initialize tms::utils. Abort.\n";
        return 1;
    }

    try
    {

        po::options_description general_desc("Action");
        general_desc.add_options()
            ("help,h", "Usage:")
            ("command", po::value< std::string >(), "Command to execute: generate or validate")
            ("subargs", po::value< std::vector< std::string > >(), "Arguments for the command");

        po::positional_options_description pos_desc;
        pos_desc.add("command",  1)
                .add("subargs", -1);
        
        po::variables_map vm;
        po::parsed_options parsed = po::command_line_parser(argc, argv).options(general_desc)
                                                                    .positional(pos_desc)
                                                                    .allow_unregistered()
                                                                    .run();
        po::store(parsed, vm);
        po::notify(vm);

        if (vm.count("help") || !vm.count("command"))
        {
            actions::ShowUsage(general_desc);
            return 1;
        }

        std::string_view command_str = vm["command"].as< std::string >();
        EAction action = FromString(command_str);
        std::vector< std::string > subargs = po::collect_unrecognized(parsed.options, po::collect_unrecognized_mode::include_positional);

        switch (action)
        {

            case EAction::eA_Hash:
            {
                actions::GenerateHash(subargs);
                break;
            }

            case EAction::eA_Validate:
            {
                actions::ValidatePassword(subargs);
                break;
            }

            default:
            {
                actions::UnrecognizedCommand(command_str, general_desc);
                break;
            }

        }
    }
    catch (const po::error& ex)
    {
        std::cout << fmt::format("Programs options exception caught: {}\n", ex.what());
    }
    catch (const std::exception& ex)
    {
        std::cout << fmt::format("Unknown exception caught: {}\n", ex.what());
    }
    
    return 0;
}