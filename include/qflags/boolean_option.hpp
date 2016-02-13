// boolean_option.hpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE boolean_option::boolean_option(char const* name, bool default_value) :
    option(name, default_value ? "true" : "false"),
    _value_boolean(default_value),
    _default_value(default_value) {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE int boolean_option::parse(int argc, char const* const* argv, std::string* errors)
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");

    // Check if argument matches option's name.
    if (argc < 1 || argv[0] != ("--" + _name)) {
        return 0;
    }

    if (argc < 2) {
        errors->append("Error: Insufficient arguments for integer option '");
        errors->append(_name);
        errors->append("'.\n");
        return -1;
    }

    // Check if argument matches any of the following:
    //  "true" "True" "TRUE" "1"
    if (argv[1] == std::string("true") || argv[1] == std::string("True")
            || argv[1] == std::string("TRUE") || argv[1] == std::string("1")) {
        _value_boolean = true;
    }
    // Check if argument matches any of the following:
    //  "false" "False" "FALSE" "0"
    else if (argv[1] == std::string("false") || argv[1] == std::string("False")
             || argv[1] == std::string("FALSE") || argv[1] == std::string("0")) {
        _value_boolean = false;
    }
    // Invalid argument.
    else {
        errors->append("Error: Invalid argument for boolean option '");
        errors->append(_name);
        errors->append("': '");
        errors->append(argv[1]);
        errors->append("'.\n");
        return -1;
    }

    _is_set = true;
    _value_string = argv[1];
    return 2;
}

} // namespace qflags
