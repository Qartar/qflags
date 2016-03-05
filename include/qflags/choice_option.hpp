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
    choice_option(name, "", std::move(choices), default_value) {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE choice_option::choice_option(char const* name,
                                           std::set<std::string> const& choices,
                                           char const* default_value) :
    choice_option(name, "", choices, default_value) {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE choice_option::choice_option(char const* name,
                                           char const* short_name,
                                           std::initializer_list<char const*>&& choices,
                                           char const* default_value) :
    string_option(name, short_name, default_value)
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
                                           char const* short_name,
                                           std::set<std::string> const& choices,
                                           char const* default_value) :
    string_option(name, short_name, default_value),
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
    int argn = _parse_string(argc, argv, &_value_string, errors);

    if (argn > 0) {
        // Check that the argument value is valid.
        if (_choices.find(argv[1]) == _choices.cend()) {
            errors->append("Error: Invalid argument for choice option '");
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
