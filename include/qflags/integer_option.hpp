// integer_option.hpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE integer_option::integer_option(char const* name, int64_t default_value) :
    integer_option(name, "", default_value, "") {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE integer_option::integer_option(char const* name,
                                             char const* short_name,
                                             int64_t default_value,
                                             char const* description) :
    option(name, short_name, std::to_string(default_value).c_str(), description),
    _value_integer(default_value),
    _default_value(default_value) {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE int integer_option::parse(int argc, char const* const* argv, std::string* errors)
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");

    int argn = _parse_integer(argc, argv, &_value_string, &_value_integer, errors);

    _is_set = (argn > 0) ? true : false;

    return argn;
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE int integer_option::_parse_integer(int argc,
                                                 char const* const* argv,
                                                 std::string* value_string,
                                                 int64_t* value_integer,
                                                 std::string* errors) const
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");
    assert(value_string && "value_string must not be null!");
    assert(value_integer && "value_integer must not be null!");

    // Parse the argument as a string.
    int argn = _parse_string(argc, argv, value_string, errors);

    if (argn > 0) {
        // Parse string value as an integer.
        char* endptr = nullptr;
        long long value = std::strtoll(value_string->c_str(), &endptr, 0);

        // Detect if parsing failed.
        if (endptr == value_string->c_str()) {
            errors->append("Error: Failed to parse argument for integer option '");
            errors->append(_name);
            errors->append("': '");
            errors->append(*value_string);
            errors->append("'.\n");
            return -1;
        } else if (*endptr != '\0') {
            errors->append("Error: Argument for integer option '");
            errors->append(_name);
            errors->append("' contains invalid characters: '");
            errors->append(endptr);
            errors->append("'.\n");
            return -1;
        }

        (*value_integer) = value;
    }

    return argn;
}

} // namespace qflags
