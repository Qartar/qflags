// qflags.h
//

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 * Utility class for normalizing command line arguments into a UTF-8 encoded
 * array of argument strings.
 */
class command_line
{
  public:
    /**
     * Initialize from a UTF-16 encoded wide-character command line string.
     *  e.g. the return value of GetCommandLineW() on Windows.
     *
     *  @param args
     *      a UTF-16 encoded wide-character string.
     */
    command_line(wchar_t const* args);

    /**
     * Initialize from an array of UTF-16 encoded wide-character command line
     * argument strings.
     *  e.g. the return value of CommandLineToArgvW() on Windows.
     *
     *  @param argc
     *      number of elements in `argv`
     *  @param argv
     *      an array of UTF-16 encoded wide-character argument strings
     */
    command_line(int argc, wchar_t const* const* argv);

    /**
     * Initialize from an array of command line argument strings encoded
     * using the specified locale. Defaults to the 'user-preferred' locale set
     * by the operating system.
     *
     *  @param argc
     *      number of elements in `argv`
     *  @param argv
     *      an array of argument strings
     *  @param locale
     *      a locale name used to interpret the strings in `argv`
     *      a value of `nullptr` will suppress encoding conversion.
     */
    command_line(int argc, char const* const* argv, char const* locale = "");

    /**
     * Construct a copy from another command_line instance.
     */
    command_line(command_line const& other);

    /**
     * Copy from another command_line instance.
     */
    command_line& operator=(command_line const& other);

    //! @return
    //!     number of argument strings.
    int argc() const { return static_cast<int>(_argv.size() - 1); }

    //! @return
    //!     array of UTF-8 encoded argument strings.
    char const* const* argv() const { return _argv.data(); }

    //! @return
    //!     specified argument by index as a UTF-8 encoded string.
    char const* argv(int argn) const { return _argv[argn]; }

  private:
    std::vector<char> _args;    //!< Argument string buffer
    std::vector<char*> _argv;   //!< Argument string pointers

  private:
    //! Initialize from array of UTF-16 encoded wide-character strings.
    void _init(int argc, wchar_t const* const* argv);

    //! Initialize from array of UTF-8 encoded character strings.
    void _init(int argc, char const* const* argv);
};

} // namespace qflags
