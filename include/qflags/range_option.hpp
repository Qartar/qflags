// range_option.hpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE range_option::range_option(char const* name,
                                         std::initializer_list<int64_t>&& choices,
                                         int64_t default_value) :
    range_option(name, "", std::move(choices), default_value) {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE range_option::range_option(char const* name,
                                         int64_t minimum_value,
                                         int64_t maximum_value,
                                         int64_t default_value) :
    range_option(name, "", minimum_value, maximum_value, default_value) {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE range_option::range_option(char const* name,
                                         char const* short_name,
                                         std::initializer_list<int64_t>&& choices,
                                         int64_t default_value) :
    integer_option(name, short_name, default_value),
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
QFLAGS_INLINE range_option::range_option(char const* name,
                                         char const* short_name,
                                         int64_t minimum_value,
                                         int64_t maximum_value,
                                         int64_t default_value) :
    integer_option(name, short_name, default_value),
    _minimum_value(minimum_value),
    _maximum_value(maximum_value)
{
    assert(minimum_value <= default_value && default_value <= maximum_value);
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE int range_option::parse(int argc, char const* const* argv, std::string* errors)
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");

    // Check if argument could be parsed as any integer.
    int argn = _parse_integer(argc, argv, &_value_string, &_value_integer, errors);

    if (argn > 0) {
        // Check that the argument value is valid.
        if ((_choices.size() && _choices.find(_value_integer) == _choices.cend())
                || _value_integer < _minimum_value || _value_integer > _maximum_value) {
            errors->append("Error: Invalid argument for range option '");
            errors->append(_name);
            errors->append("': '");
            errors->append(argv[1]);
            errors->append("'.\n");

            // Leave value undefined.
            return -1;
        }

        _is_set = true;
    }

    return argn;
}

} // namespace qflags
