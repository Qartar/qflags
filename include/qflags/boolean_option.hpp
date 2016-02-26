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

    int argn = _parse_boolean(argc, argv, &_value_string, &_value_boolean, errors);

    _is_set = (argn > 0) ? true : false;

    return argn;
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE int boolean_option::_parse_boolean(int argc,
                                                 char const* const* argv,
                                                 std::string* value_string,
                                                 bool* value_boolean,
                                                 std::string* errors) const
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");
    assert(value_string && "value_string must not be null!");
    assert(value_boolean && "value_boolean must not be null!");

    // Parse the argument as a string.
    int argn = _parse_string(argc, argv, value_string, errors);

    if (argn > 0) {
        // Check if argument matches any of the following:
        //  "true" "True" "TRUE" "1"
        if ((*value_string) == std::string("true") || (*value_string) == std::string("True")
                || (*value_string) == std::string("TRUE") || (*value_string) == std::string("1")) {
            (*value_boolean) = true;
        }
        // Check if argument matches any of the following:
        //  "false" "False" "FALSE" "0"
        else if ((*value_string) == std::string("false") || (*value_string) == std::string("False")
                 || (*value_string) == std::string("FALSE") || (*value_string) == std::string("0")) {
            (*value_boolean) = false;
        }
        // Invalid argument.
        else {
            errors->append("Error: Invalid argument for boolean option '");
            errors->append(_name);
            errors->append("': '");
            errors->append(*value_string);
            errors->append("'.\n");
            return -1;
        }
    }

    return argn;
}

} // namespace qflags
