// flag.hpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
QFLAGS_INLINE flag::flag(char const* name) :
    argument(name),
    _short_name("") {}

QFLAGS_INLINE flag::flag(char const* name, char const* short_name) :
    argument(name),
    _short_name(short_name) {}

////////////////////////////////////////////////////////////////////////////////
QFLAGS_INLINE int flag::parse(int argc, char const* const* argv, std::string* errors)
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");
    assert(argc || (argv[0][0] == '-' && argv[0][1] != '-'));

    // Check if argument matches flag's long name.
    if (argc && argv[0] == ("--" + _name)) {
        _is_set = true;
        return 1;
    }
    // Check if argument matches flag's short name.
    else if (!argc && _short_name.length()) {
        std::string flag(argv[0], argv[0] + _short_name.length() + 1);

        if (flag == ("-" + _short_name)) {
            _is_set = true;
            return static_cast<int>(_short_name.length());
        }
    }

    return 0;
}

} // namespace qflags
