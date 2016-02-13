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
QFLAGS_INLINE range_option::range_option(char const* name,
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
QFLAGS_INLINE int range_option::parse(int argc, char const* const* argv, std::string* errors)
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");

    // Check if argument could be parsed as any integer.
    auto integer_result = integer_option::parse(argc, argv, errors);
    if (integer_result < 2 ) {
        return integer_result;
    }

    // Check that the argument value is valid.
    if ((_choices.size() && _choices.find(_value_integer) == _choices.cend())
            || _value_integer < _minimum_value || _value_integer > _maximum_value) {
        errors->append("Error: Invalid argument for range option '");
        errors->append(_name);
        errors->append("': '");
        errors->append(argv[1]);
        errors->append("'.\n");

        // Reset is_set to false, leave value undefined.
        _is_set = false;
        return -1;
    }

    return integer_result;
}

} // namespace qflags
