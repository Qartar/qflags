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
     *
     */
    command_line(wchar_t const* args);

    /**
     *
     */
    command_line(int argc, wchar_t const* const* argv);

    /**
     *
     */
    command_line(int argc, char const* const* argv, char const* locale = "");

    //! @return
    //!     number of argument strings.
    int argc() const { return static_cast<int>(_argv.size()); }

    //! @return
    //!     array of UTF-8 encoded argument strings.
    char const* const* argv() const { return _argv.data(); }

    //! @return
    //!     specified argument by index as a UTF-8 encoded string.
    char const* argv(int argn) const { return _argv[argn]; }

  private:
    std::vector<char> _args;
    std::vector<char*> _argv;

  private:
    //! Initialize from array of UTF-16 encoded wide-character strings.
    void _init(int argc, wchar_t const* const* argv);

    //! Initialize from array of UTF-8 encoded character strings.
    void _init(int argc, char const* const* argv);
};

} // namespace qflags
