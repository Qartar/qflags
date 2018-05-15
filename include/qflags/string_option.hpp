// string_option.hpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 * 
 */
QFLAGS_INLINE string_option::string_option(char const* name,
                                           char const* default_value) :
    string_option(name, "", default_value, "") {}

////////////////////////////////////////////////////////////////////////////////
/**
 * 
 */
QFLAGS_INLINE string_option::string_option(char const* name,
                                           char const* short_name,
                                           char const* default_value,
                                           char const* description) :
    option(name, short_name, default_value ? default_value : "", description),
    _default_value(default_value ? default_value : "") {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE int string_option::parse(int argc, char const* const* argv, std::string* errors)
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");

    int argn = _is_set ? 0 : _parse_string(argc, argv, &_value_string, errors);

    _is_set |= (argn > 0) ? true : false;

    return argn;
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE int option::_parse_string(int argc,
                                        char const* const* argv,
                                        std::string* value_string,
                                        std::string* errors) const
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");
    assert(value_string && "value_string must not be null!");
    assert(argv[0][0] == '-' && "argv must start with argument prefix!");

    bool is_long_opt = (argv[0][1] == '-') ? true : false;

    // Choose name or short name based on argument prefix. If short name is
    // empty then always use the long name regardless of prefix.
    std::string argument_name = (_short_name.empty() || is_long_opt)
                                ? "--" + _name
                                : "-" + _short_name;

    // Check if argument matches option's name.
    if (argc < 1 || strncmp(argv[0], argument_name.c_str(), argument_name.length())) {
        return 0;
    }

    // Check if argument has the form: "--<name>=<value>"
    if (is_long_opt && argv[0][argument_name.length()] == '=') {
        (*value_string) = argv[0] + argument_name.length() + 1;
        return 1;
    }
    // Check if argument has the form: "-<shortname><value>"
    else if (!is_long_opt && argv[0][argument_name.length()] != '\0') {
        (*value_string) = argv[0] + argument_name.length();
        return 1;
    }
    // Check that argument exactly matches option's name.
    else if (argv[0][argument_name.length()] != '\0') {
        return 0;
    }
    // Check if there are insufficient arguments.
    else if (argc < 2) {
        errors->append("Error: Insufficient arguments for option '");
        errors->append(_name);
        errors->append("'.\n");
        return -1;
    }

    (*value_string) = argv[1];
    return 2;
}

} // namespace qflags
