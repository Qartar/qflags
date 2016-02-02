// string_option.cpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 * 
 */
string_option::string_option(char const* name,
                             char const* default_value) :
    option(name, default_value ? default_value : ""),
    _default_value(default_value ? default_value : "") {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
int string_option::parse(int argc, char const* const* argv, std::string* errors)
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");

    // Check if argument matches option's name.
    if (argc < 1 || argv[0] != ("--" + _name)) {
        return 0;
    }

    if (argc < 2) {
        errors->append("Error: Insufficient arguments for string option '");
        errors->append(_name);
        errors->append("'.\n");
        return -1;
    }

    if (argv[1][0] == '-') {
        errors->append("Error: Invalid argument for string option '");
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
