// parser.hpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QFLAGS_INLINE parser::parser() :
    _command_line(0, (char const* const*)nullptr, nullptr),
    _remaining(0, (char const* const*)nullptr, nullptr) {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE bool parser::add_argument(argument* arg)
{
    assert(arg && "arg cannot be null!");

    if (!arg) {
        fprintf(stderr, "Error: Invalid parameter, arg cannot be null!\n");
        return false;
    }

    // Check for invalid characters in the argument name.
    if (arg->name().find('=') != std::string::npos) {
        fprintf(stderr,
                "Error: Argument name '%s' contains invalid character '='.\n",
                arg->name().c_str());
        return false;
    }

    // Check for duplicate arguments.
    auto const iter = _arguments.find(arg->name());
    if (iter != _arguments.cend()) {
        // Same argument being added twice.
        if (iter->second == arg) {
            fprintf(stderr,
                    "Warning: The argument with name '%s' has already been added to the parser.\n",
                    arg->name().c_str());
            return true;
        }
        // Another argument being added with the same name.
        else {
            fprintf(stderr,
                    "Error: An argument with name '%s' has already been added to the parser.\n",
                    arg->name().c_str());
            return false;
        }
    }

    _arguments[arg->name()] = arg;

    // Add flags to a separate list.
    if (arg->is_flag()) {
        _flags.push_back(arg);
    }
    // Add commands to a separate list.
    if (arg->is_command()) {
        _commands.push_back(arg);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE argument const& parser::operator[](std::string const& name) const
{
    return *(_arguments.at(name));
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE argument const& parser::operator[](char const* name) const
{
    return *(_arguments.at(name));
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE bool parser::parse(command_line const& command_line, std::string* errors)
{
    _command_line = command_line;

    int const argc = _command_line.argc();
    std::vector<char const*> argv(_command_line.argv(),
                                  _command_line.argv() + argc);

    for (size_t ii = 0; ii < argv.size();) {
        // Arg is a short flag or group of short flags.
        if (argv[ii][0] == '-' && argv[ii][1] != '-') {
            if (!parse_flags(argv[ii], errors)) {
                return false;
            }

            argv.erase(argv.cbegin() + ii);
            continue;
        }
        // Arg is an argument terminator: "--"
        else if (argv[ii][0] == '-' && argv[ii][1] == '-' && argv[ii][2] == '\0') {

            // Remove terminator from remaining arguments and stop parsing.
            argv.erase(argv.cbegin() + ii);
            break;
        }
        // Arg is a long flag or option.
        else if (argv[ii][0] == '-' && argv[ii][1] == '-') {
            int count = 0;

            for (auto iter : _arguments) {
                count = iter.second->parse(static_cast<int>(argv.size() - ii),
                                           argv.data() + ii,
                                           errors);

                if (count) {
                    break;
                }
            }

            if (count > 0) {
                argv.erase(argv.cbegin() + ii,
                           argv.cbegin() + ii + count);
                continue;
            } else if (count == 0) {
                errors->append("Error: '");
                errors->append(argv[ii] + 2);
                errors->append("' is not a valid option.\n");
                return false;
            } else {
                return false;
            }
        }
        // Arg is a command or an unbound argument.
        else {
            int count = parse_command(static_cast<int>(argv.size() - ii),
                                      argv.data() + ii,
                                      errors);

            if (count > 0) {
                // Only one command is allowed per command-line.
                argv.erase(argv.cbegin() + ii,
                           argv.cbegin() + ii + count);
                break;
            }
            else if (count < 0) {
                return false;
            }
            // Arg is an unbound argument.
        }

        ++ii; // Only increment if no arguments were removed.
    }

    _remaining = qflags::command_line(static_cast<int>(argv.size()),
                                      argv.data(),
                                      nullptr);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE bool parser::parse_flags(char const* arg, std::string* errors)
{
    assert(arg && "flags arg cannot be null!");
    assert(arg[0] == '-' && arg[1] != '-' && "flags arg must start with a single dash!");

    std::string flags_str(arg);
    std::string invalid_flags;

    for (size_t ii; flags_str.length() > 1;) {
        for (ii = 0; ii < _flags.size(); ++ii) {
            bool was_set = _flags[ii]->is_set();

            auto args = flags_str.c_str();
            auto len = _flags[ii]->parse(0, &args, errors);

            if (len) {
                if (was_set) {
                    errors->append("Warning: The flag with name '");
                    errors->append(_flags[ii]->name());
                    errors->append("' was set more than once.\n");
                }

                flags_str.erase(flags_str.begin() + 1,
                                flags_str.begin() + 1 + len);

                break;
            }
        }

        if (ii == _flags.size()) {
            invalid_flags.append(flags_str.c_str() + 1, 1);
            flags_str.erase(flags_str.begin() + 1,
                            flags_str.begin() + 2);
        }
    }

    if (invalid_flags.length()) {
        errors->append("Error: The command line contains invalid flags '");
        errors->append(invalid_flags);
        errors->append("'.\n");
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE int parser::parse_command(int argc, char const* const* argv, std::string* errors)
{
    for (auto command : _commands) {
        int argn = command->parse(argc, argv, errors);
        // Parsed succeeded.
        if (argn > 0) {
            return argn;
        }
        // Parsing failed.
        else if (argn < 0) {
            return -1;
        }
    }
    // Failed to match any command.
    return 0;
}

} // namespace qflags
