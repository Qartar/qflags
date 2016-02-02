// range_option.cpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
range_option::range_option(char const* name,
                           std::initializer_list<int64_t>&& choices,
                           int64_t default_value) :
    integer_option(name, default_value),
    _minimum_value(INT64_MIN),
    _maximum_value(INT64_MAX)
{
    _choices.insert(choices.begin(), choices.end());
    assert(_choices.size() && _choices.size() == choices.size());
    assert(_choices.find(default_value) != _choices.cend());
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
range_option::range_option(char const* name,
                           int64_t minimum_value,
                           int64_t maximum_value,
                           int64_t default_value) :
    integer_option(name, default_value),
    _minimum_value(minimum_value),
    _maximum_value(maximum_value)
{
    assert(minimum_value <= default_value && default_value <= maximum_value);
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
int range_option::parse(int argc, char const* const* argv, std::string* errors)
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");

    // Check if argument matches option's name.
    if (argc < 1 || argv[0] != ("--" + _name)) {
        return 0;
    }

    if (argc < 2) {
        errors->append("Error: Insufficient arguments for range option '");
        errors->append(_name);
        errors->append("'.\n");
        return -1;
    }

    char* endptr = nullptr;
    long long value = std::strtoll(argv[1], &endptr, 0);

    // Detect if parsing failed.
    if (endptr == argv[1]) {
        errors->append("Error: Failed to parse argument for range option '");
        errors->append(_name);
        errors->append("': '");
        errors->append(argv[1]);
        errors->append("'.\n");
        return -1;
    } else if (*endptr != '\0') {
        errors->append("Error: Argument for range option '");
        errors->append(_name);
        errors->append("' contains invalid characters: '");
        errors->append(endptr);
        errors->append("'.\n");
        return -1;
    }

    // Check that the argument value is valid.
    if ((_choices.size() && _choices.find(value) == _choices.cend())
            || value < _minimum_value || value > _maximum_value) {
        errors->append("Error: Invalid argument for range option '");
        errors->append(_name);
        errors->append("': '");
        errors->append(argv[1]);
        errors->append("'.\n");
        return -1;
    }

    _is_set = true;
    _value_string = argv[1];
    _value_integer = value;

    return 2;
}

} // namespace qflags
