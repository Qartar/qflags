// integer_option.cpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
integer_option::integer_option(char const* name, int64_t default_value) :
    option(name, std::to_string(default_value).c_str()),
    _value_integer(default_value),
    _default_value(default_value) {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
int integer_option::parse(int argc, char const* const* argv, std::string* errors)
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

    char* endptr = nullptr;
    long long value = std::strtoll(argv[1], &endptr, 0);

    // Detect if parsing failed.
    if (endptr == argv[1]) {
        errors->append("Error: Failed to parse argument for integer option '");
        errors->append(_name);
        errors->append("': '");
        errors->append(argv[1]);
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

    _is_set = true;
    _value_string = argv[1];
    _value_integer = value;

    return 2;
}

} // namespace qflags
