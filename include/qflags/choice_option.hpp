// choice_option.hpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE choice_option::choice_option(char const* name,
                             std::initializer_list<char const*>&& choices,
                             char const* default_value) :
    string_option(name, default_value)
{
    _choices.insert(choices.begin(), choices.end());
    assert(_choices.size() && _choices.size() == choices.size());
    assert(_choices.find(default_value) != _choices.cend());
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE choice_option::choice_option(char const* name,
                             std::set<std::string> const& choices,
                             char const* default_value) :
    string_option(name, default_value),
    _choices(choices)
{
    assert(_choices.find(default_value) != _choices.cend());
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE int choice_option::parse(int argc, char const* const* argv, std::string* errors)
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");

    // Check if argument could be parsed as any string.
    auto string_result = string_option::parse(argc, argv, errors);
    if (string_result < 2) {
        return string_result;
    }

    // Check that the argument value is valid.
    if (_choices.find(argv[1]) == _choices.cend()) {
        errors->append("Error: Invalid argument for choice option '");
        errors->append(_name);
        errors->append("': '");
        errors->append(argv[1]);
        errors->append("'.\n");

        // Reset is_set to false, leave value undefined.
        _is_set = false;
        return -1;
    }

    return string_result;
}

} // namespace qflags
