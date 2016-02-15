// command.hpp
//

#include <qflags/qflags.h>

#include <cassert>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE command::command(char const* name) :
    argument(name) {}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE int command::parse(int argc, char const* const* argv, std::string* errors)
{
    assert(argv && "argv must not be null!");
    assert(errors && "errors must not be null!");

    // Check if argument matches command's name.
    if (argc < 1 || argv[0] != _name) {
        return 0;
    }

    // Create command line arguments from the remaining arguments.
    auto args = command_line(argc, argv, nullptr);

    // Attempt to parse the command arguments.
    if (parser::parse(args, errors)) {
        _is_set = true;
        return argc;
    } else {
        return -1;
    }
}

} // namespace qflags
