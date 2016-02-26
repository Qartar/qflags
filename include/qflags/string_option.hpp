// string_option.hpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 * 
 */
QFLAGS_INLINE string_option::string_option(char const* name,
                                           char const* default_value) :
    option(name, default_value ? default_value : ""),
    _default_value(default_value ? default_value : "") {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE int string_option::parse(int argc, char const* const* argv, std::string* errors)
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");

    int argn = _parse_string(argc, argv, &_value_string, errors);

    _is_set = (argn > 0) ? true : false;

    return argn;
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE int option::_parse_string(int argc,
                                        char const* const* argv,
                                        std::string* value_string,
                                        std::string* errors) const
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");
    assert(value_string && "value_string must not be null!");

    // Check if argument matches option's name.
    if (argc < 1 || argv[0] != ("--" + _name)) {
        return 0;
    }

    if (argc < 2) {
        errors->append("Error: Insufficient arguments for option '");
        errors->append(_name);
        errors->append("'.\n");
        return -1;
    }

    (*value_string) = argv[1];
    return 2;
}

} // namespace qflags
