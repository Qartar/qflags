// choice_option.cpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
choice_option::choice_option(char const* name,
                             std::initializer_list<char const*>&& choices,
                             char const* default_value) :
    string_option(name, default_value)
{
    _choices.insert(choices.begin(), choices.end());
    assert(_choices.size() == choices.size());
    assert(_choices.find(default_value) != _choices.cend());
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
choice_option::choice_option(char const* name,
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
int choice_option::parse(int argc, char const* const* argv, std::string* errors)
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");

    // Check if argument matches option's name.
    if (argc < 1 || argv[0] != ("--" + _name)) {
        return 0;
    }

    if (argc < 2) {
        errors->append("Error: Insufficient arguments for choice option '");
        errors->append(_name);
        errors->append("'.\n");
        return -1;
    }

    if (argv[1][0] == '-' || _choices.find(argv[1]) == _choices.cend()) {
        errors->append("Error: Invalid argument for choice option '");
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
