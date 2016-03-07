// qflags.h
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>

//! Macro used to eliminate compiler warnings generated when formal parameters
//! are declared but unreferenced; particularly for parameters which are used
//! in one configuration but not another (e.g. Debug vs Release). MSVC's C1/C2
//! compiler generates unreferenced variable warnings for variables which are
//! only referenced in an assert expression however Clang/C2 does not.
#define QFLAGS_UNREFERENCED_PARAMETER(x) ((void)x)

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 * @class command_line
 *
 * @brief Utility class encapsulating a command line
 *
 * Normalizes command line arguments into a UTF-8 encoded array of argument
 * strings. It accepts a single wide-character string, an array of wide-
 * character strings, or an array of multi-byte character strings. It is
 * *highly* recommended for developers on Windows platforms to use the wide-
 * character versions in order to avoid non-reversible character conversions.
 */
class command_line
{
  public:

    /**
     * Functor for specifying the default locale on different platforms.
     */
    struct default_locale {
#if defined(__MINGW32__)
        // Only C and POSIX locales are supported on MinGW so suppress conversion.
        operator const char*() const { return nullptr; };
#else //!defined(__MINGW32__)
        operator const char*() const { return ""; };
#endif //!defined(__MINGW32__)
    };

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
     * Initialize from an array of UTF-16 encoded wide-character command line
     * argument strings. Automatically deduces argc from static array size.
     *
     *  @param argv
     *      an array of UTF-16 encoded wide-character argument strings
     */
    template<size_t _Size>
    command_line(wchar_t const* const (&argv)[_Size])
        : command_line(static_cast<int>(_Size), argv) {}

    /**
     * Initialize from a single command line argument string encoded using the
     * specified locale. Defaults to the 'user-preferred' local set by the
     * operating system.
     *
     *  @param args
     *      an argument string
     *  @param locale
     *      a locale name used to interpret the string `args`
     *      a value of `nullptr` will suppress encoding conversion.
     */
    command_line(char const* args, char const* locale = default_locale());

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
    command_line(int argc, char const* const* argv, char const* locale = default_locale());

    /**
     * Initialize from an array of command line argument strings encoded
     * using the specified locale. Defaults to the 'user-preferred' locale set
     * by the operating system. Automatically deduces argc from static array size.
     *
     *  @param argv
     *      an array of argument strings
     *  @param locale
     *      a locale name used to interpret the strings in `argv`
     *      a value of `nullptr` will suppress encoding conversion.
     */
    template<size_t _Size>
    command_line(char const* const (&argv)[_Size], char const* locale = default_locale())
        : command_line(static_cast<int>(_Size), argv, locale) {}

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
    //! Initialize from UTF-16 encoded wide-character string.
    void _init(wchar_t const* args);

    //! Initialize from UTF-8 encoded character string.
    void _init(char const* args);

    //! Initialize from array of UTF-16 encoded wide-character strings.
    void _init(int argc, wchar_t const* const* argv);

    //! Initialize from array of UTF-8 encoded character strings.
    void _init(int argc, char const* const* argv);
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class argument
 *
 * @brief Abstract base class for all argument types
 */
class argument
{
  public:
    //! @return
    //!     name of the argument
    std::string const& name() const { return _name; }

    //! @return
    //!     short name of the argument, may be empty
    std::string const& short_name() const { return _short_name; }

    //! @return
    //!     true if the argument was explicitly set by the command line
    bool is_set() const { return _is_set; }

    //! @return
    //!     true if the argument is a flag
    virtual bool is_flag() const { return false; }

    //! @return
    //!     true if the argument is a command
    virtual bool is_command() const { return false; }

    //! @return
    //!     true if the argument is implicitly convertible to a boolean
    virtual bool is_boolean() const { return false; }

    //! @return
    //!     true if the argument is implicitly convertible to an integer
    virtual bool is_integer() const { return false; }

    //! @return
    //!     true if the argument is implicitly convertible to a string
    virtual bool is_string() const { return false; }

    //! @return
    //!     the argument value as a boolean if `is_boolean` is true
    virtual bool value_boolean() const;

    //! @return
    //!     the argument value as an integer if `is_integer` is true
    virtual int64_t value_integer() const;

    //! @return
    //!     the argument value as a string if `is_string` is true
    virtual std::string const& value_string() const;

    //! User-defined conversions to a boolean value
    operator bool() const { return value_boolean(); }

    //! User-defined conversions to an integer value
    operator int64_t() const { return value_integer(); }

    //! User-defined conversions to an integer value. This extra method is
    //! necessary because a cast to int otherwise chooses the bool overload.
    operator int() const { return static_cast<int>(value_integer()); }

    //! User-defined conversions to a string value
    operator std::string const&() const { return value_string(); }

  protected:
    friend class parser;

    std::string _name;          //!< name of this argument
    std::string _short_name;    //!< single-character name of the argument
    bool _is_set;               //!< true if the argument was set by the command line

  protected:
    //! @param[in] name
    //!     name of the argument
    argument(char const* name);

    //! @param[in] name
    //!     name of the argument
    //! @param[in] short_name
    //!     short name of the argument
    argument(char const* name, char const* short_name);

    //! Process the command line arguments for this argument.
    //! @param[in] argc
    //!     the number of arguments in the argv array
    //! @param[in] argv
    //!     an array of arguments starting at this argument's name
    //! @param[out] errors
    //!     a string describing any errors that occurred during parsing
    //! @return
    //!     the number of command line arguments consumed by this argument,
    //!      0 if the argument is unmatched, or
    //!     -1 if the argument is invalid.
    virtual int parse(int argc, char const* const* argv, std::string* errors) = 0;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class parser
 *
 * @brief Populates argument values from a command line
 *
 * Arguments are added to the parser prior to parsing a command line. The parser
 * holds a reference to the arguments. When a command line is parsed the
 * arguments are then populated with values from the command line. Arguments
 * can be accessed from the parser by name or by the original argument object
 * passed to the parser.
 */
class parser
{
  public:
    //! Constructs a default instance of parser.
    parser();

    //! Add an argument to the parser.
    //! @param[in] arg
    //!     an argument, a reference is held by the parser
    //! @return
    //!     true if the argument was added to the parser
    bool add_argument(argument* arg);

    //! Parse the command line using the arguments added to the parser.
    //! @param[in] command_line
    //!     command line to be parsed
    //! @param[out] errors
    //!     a string describing any errors that occurred during parsing
    //! @return
    //!     true if the command line was parsed successfully
    bool parse(command_line const& command_line, std::string* errors);

    //! @param[in] name
    //!     name of the argument to return
    //! @return
    //!     the argument with the given name
    argument const& operator[](std::string const& name) const;

    //! @param[in] name
    //!     name of the argument to return
    //! @return
    //!     the argument with the given name
    argument const& operator[](char const* name) const;

    //! @return
    //!     number of argument strings.
    int argc() const { return _command_line.argc(); }

    //! @return
    //!     array of UTF-8 encoded argument strings.
    char const* const* argv() const { return _command_line.argv(); }

    //! @return
    //!     specified argument by index as a UTF-8 encoded string.
    char const* argv(int argn) const { return _command_line.argv(argn); }

    //! @return
    //!     number of argument strings remaining after parsing.
    int remaining_argc() const { return _remaining.argc(); }

    //! @return
    //!     array of UTF-8 encoded argument strings remaining after parsing.
    char const* const* remaining_argv() const { return _remaining.argv(); }

    //! @return
    //!     specified argument by index as a UTF-8 encoded string.
    char const* remaining_argv(int argn) const { return _remaining.argv(argn); }

  private:
    //! A copy of the original command line provided to `parse`.
    command_line _command_line;

    //! The command line after removing parsed arguments.
    command_line _remaining;

    //! All arguments added to the parser keyed by their names.
    std::map<std::string, argument*> _arguments;

    //! All arguments with short names keyed by their short names.
    std::map<std::string, argument*> _short_arguments;

    //! A list of all command arguments added to the parser.
    std::vector<argument*> _commands;

    //! A list of all flag arguments added to the parser.
    std::vector<argument*> _flags;

  private:
    //! Attempt to parse the given arguments as a command argument.
    //! @param[in] argc
    //!     the number of arguments in the argv array
    //! @param[in] argv
    //!     an array of arguments starting at this argument's name
    //! @param[out] errors
    //!     a string describing any errors that occurred during parsing
    //! @return
    //!     the number of command line arguments consumed by this argument,
    //!      0 if the argument is unmatched, or
    //!     -1 if the argument is invalid.
    int parse_command(int argc, char const* const* argv, std::string* errors);

    //! Attempt to parse the given argument as a short flag argument or a group
    //! of short flag arguments.
    //! @param[in] arg
    //!     a short flag argument or a group of short flag arguments
    //! @param[out] errors
    //!     a string describing any errors that occurred during parsing
    //! @return
    //!     false if the argument was invalid, otherwise true
    bool parse_flags(char const* arg, std::string* errors);
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class command
 *
 * @brief Sub-command argument for hierarchical command line parsing
 *
 * Arguments can be added to the command argument as a separate parser. If a
 * parser encounters a command argument then the remainder of the command line
 * arguments are parsed by the subcommand.
 */
class command
    : public argument
    , public parser
{
  public:
    //! Construct a command argument with the given name.
    //! @param[in] name
    //!     name of the command argument
    command(char const* name);

    //! @return true
    virtual bool is_command() const final { return true; }

    //! Allow commands to be implicitly converted to a boolean.
    //! @return true
    virtual bool is_boolean() const final { return true; }

    //! @return
    //!     true if the command was invoked by the command line
    virtual bool value_boolean() const final { return _is_set; }

  protected:
    virtual int parse(int argc, char const* const* argv, std::string* errors) override;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class flag
 *
 * @brief Simple argument which evaluates to true if found on the command line.
 *
 * Flags can be specified by using their name as a long option (e.g. "--foo") or
 * by an optional short name (e.g. "-f"). Flags can also be set in a group using
 * their short names (e.g. "-fgh").
 *
 * It is an error for a flag group to contain a character that does not
 * correspond to any argument's short name and the parser will fail to parse a
 * command line in that case.
 */
class flag
    : public argument
{
  public:
    //! Construct a flag argument with the given name.
    //! @param[in] name
    //!     name of the flag argument
    flag(char const* name);

    //! Construct a flag argument with the given name and short name.
    //! @param[in] name
    //!     name of the flag argument
    //! @param[in] short_name
    //!     short name of the flag argument for use in a flag group
    flag(char const* name, char const* short_name);

    //! @return true
    virtual bool is_flag() const final { return true; }

    //! @return true
    virtual bool is_boolean() const final { return true; }

    //! @return
    //!     true if the flag was explicitly set by the command line
    virtual bool value_boolean() const final { return _is_set; }

  protected:
    virtual int parse(int argc, char const* const* argv, std::string* errors) override;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class option
 *
 * @brief Base class for arguments with command-line specified values
 *
 * All option arguments take the form "--<name> <value>" or "--<name>=<value>".
 */
class option
    : public argument
{
  public:
    //! All options are implicitly convertible to a string.
    //! @return true
    virtual bool is_string() const final { return true; }

    //! @return
    //!     argument value as a string
    virtual std::string const& value_string() const override { return _value_string; }

  protected:
    std::string _value_string; //!< Argument value as a string.

  protected:
    //! Construct an option with the given name and default value.
    //! @param[in] name
    //!     name of the option argument
    //! @param[in] default_value
    //!     default value of the option argument as a string
    option(char const* name, char const* default_value = "") :
        argument(name),
        _value_string(default_value ? default_value : "") {}

    //! Construct an option with the given name and default value.
    //! @param[in] name
    //!     name of the option argument
    //! @param[in] short_name
    //!     short name of the option argument
    //! @param[in] default_value
    //!     default value of the option argument as a string
    option(char const* name, char const* short_name, char const* default_value) :
        argument(name, short_name),
        _value_string(default_value) {}

    //! Process the command line arguments for this argument as a string. If the
    //! argument is unmatched or invalid then value_string is not modified.
    //! @param[in] argc
    //!     the number of arguments in the argv array
    //! @param[in] argv
    //!     an array of arguments starting at this argument's name
    //! @param[out] value_string
    //!     the argument value as a string
    //! @param[out] errors
    //!     a string describing any errors that occurred during parsing
    //! @return
    //!     the number of command line arguments consumed by this argument,
    //!      0 if the argument is unmatched, or
    //!     -1 if the argument is invalid.
    int _parse_string(int argc,
                      char const* const* argv,
                      std::string* value_string,
                      std::string* errors) const;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class string_option
 *
 * @brief Simple option which takes a value as a string
 */
class string_option
    : public option
{
  public:
    //! Construct a string option with the given name and default value.
    //! @param[in] name
    //!     name of the string option
    //! @param[in] default_value
    //!     default value of the string option as a string
    string_option(char const* name, char const* default_value = "");

    //! Construct a string option with the given name and default value.
    //! @param[in] name
    //!     name of the string option
    //! @param[in] short_name
    //!     short name of the string option
    //! @param[in] default_value
    //!     default value of the string option as a string
    string_option(char const* name, char const* short_name, char const* default_value);

  protected:
    std::string _default_value; //!< Default value as a string.

  protected:
    virtual int parse(int argc, char const* const* argv, std::string* errors) override;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class boolean_option
 *
 * @brief Simple option which takes a value as a boolean
 *
 * The following strings evaluate to `true`:
 *  - "true"
 *  - "True"
 *  - "TRUE"
 *  - "1"
 *
 * The following strings evaluate to `false`:
 *  - "false"
 *  - "False"
 *  - "FALSE"
 *  - "0"
 *
 * All other strings are invalid and will result in a parsing error.
 */
class boolean_option
    : public option
{
  public:
    //! Construct a boolean option with the given name and default value.
    //! @param[in] name
    //!     name of the boolean option
    //! @param[in] default_value
    //!     default value of the boolean option as a boolean
    boolean_option(char const* name, bool default_value = false);

    //! Construct a boolean option with the given name and default value.
    //! @param[in] name
    //!     name of the boolean option
    //! @param[in] short_name
    //!     short name of the boolean option
    //! @param[in] default_value
    //!     default value of the boolean option as a boolean
    boolean_option(char const* name, char const* short_name, bool default_value = false);

    //! @return true
    virtual bool is_boolean() const final { return true; }

    //! @return
    //!     argument value as a boolean
    bool value_boolean() const override { return _value_boolean; }

  protected:
      bool _value_boolean; //!< Argument value as a boolean.
      bool _default_value; //!< Default value as a boolean.

  protected:
    virtual int parse(int argc, char const* const* argv, std::string* errors) override;

    //! Process the command line arguments for this argument as a boolean.
    //! If the argument is invalid then the value stored in value_string
    //! is undefined. If the argument is unmatched then value_string and
    //! value_boolean are unmodified.
    //! @param[in] argc
    //!     the number of arguments in the argv array
    //! @param[in] argv
    //!     an array of arguments starting at this argument's name
    //! @param[out] value_string
    //!     the argument value as a string
    //! @param[out] value_boolean
    //!     the argument value as a boolean
    //! @param[out] errors
    //!     a string describing any errors that occurred during parsing
    //! @return
    //!     the number of command line arguments consumed by this argument,
    //!      0 if the argument is unmatched, or
    //!     -1 if the argument is invalid.
    int _parse_boolean(int argc,
                       char const* const* argv,
                       std::string* value_string,
                       bool* value_boolean,
                       std::string* errors) const;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class integer_option
 *
 * @brief Simple option which takes a value as an integer
 */
class integer_option
    : public option
{
  public:
    //! Construct an integer option with the given name and default value.
    //! @param[in] name
    //!     name of the integer option
    //! @param[in] default_value
    //!     default value of the integer option as an integer
    integer_option(char const* name, int64_t default_value = 0);

    //! Construct an integer option with the given name and default value.
    //! @param[in] name
    //!     name of the integer option
    //! @param[in] short_name
    //!     short name of the integer option
    //! @param[in] default_value
    //!     default value of the integer option as an integer
    integer_option(char const* name, char const* short_name, int64_t default_value = 0);

    //! @return true
    virtual bool is_integer() const final { return true; }

    //! @return
    //!     the argument value as an integer
    virtual int64_t value_integer() const override { return _value_integer; }

  protected:
    int64_t _value_integer; //!< Argument value as an integer.
    int64_t _default_value; //!< Default value as an integer.

  protected:
    virtual int parse(int argc, char const* const* argv, std::string* errors) override;

    //! Process the command line arguments for this argument as an integer.
    //! If the argument is invalid then the value stored in value_string
    //! is undefined. If the argument is unmatched then value_string and
    //! value_integer are unmodified.
    //! @param[in] argc
    //!     the number of arguments in the argv array
    //! @param[in] argv
    //!     an array of arguments starting at this argument's name
    //! @param[out] value_string
    //!     the argument value as a string
    //! @param[out] value_integer
    //!     the argument value as an integer
    //! @param[out] errors
    //!     a string describing any errors that occurred during parsing
    //! @return
    //!     the number of command line arguments consumed by this argument,
    //!      0 if the argument is unmatched, or
    //!     -1 if the argument is invalid.
    int _parse_integer(int argc,
                       char const* const* argv,
                       std::string* value_string,
                       int64_t* value_integer,
                       std::string* errors) const;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class choice_option
 *
 * @brief String-valued option which only accepts a specific set of values
 */
class choice_option
    : public string_option
{
  public:
    //! Construct a choice option with the given name and permissible values.
    //! @param[in] name
    //!     name of the choice option
    //! @param[in] choices
    //!     a set of permissible values of the choice option
    //! @param[in] default_value
    //!     default value of the choice option
    choice_option(char const* name,
                  std::initializer_list<char const*>&& choices,
                  char const* default_value);

    //! Construct a choice option with the given name and permissible values.
    //! @param[in] name
    //!     name of the choice option
    //! @param[in] choices
    //!     a set of permissible values of the choice option
    //! @param[in] default_value
    //!     default value of the choice option
    choice_option(char const* name,
                  std::set<std::string> const& choices,
                  char const* default_value);

    //! Construct a choice option with the given name and permissible values.
    //! @param[in] name
    //!     name of the choice option
    //! @param[in] short_name
    //!     short name of the choice option
    //! @param[in] choices
    //!     a set of permissible values of the choice option
    //! @param[in] default_value
    //!     default value of the choice option
    choice_option(char const* name,
                  char const* short_name,
                  std::initializer_list<char const*>&& choices,
                  char const* default_value);

    //! Construct a choice option with the given name and permissible values.
    //! @param[in] name
    //!     name of the choice option
    //! @param[in] short_name
    //!     short name of the choice option
    //! @param[in] choices
    //!     a set of permissible values of the choice option
    //! @param[in] default_value
    //!     default value of the choice option
    choice_option(char const* name,
                  char const* short_name,
                  std::set<std::string> const& choices,
                  char const* default_value);

  protected:
    //! A set of permissible values for this argument.
    std::set<std::string> _choices;

  protected:
    virtual int parse(int argc, char const* const* argv, std::string* errors) override;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class range_option
 *
 * @brief Integer-valued option which only accepts a specific range of values
 */
class range_option
    : public integer_option
{
  public:
    //! Construct a range option with the given name and permissible values.
    //! @param[in] name
    //!     name of the range option
    //! @param[in] choices
    //!     a set of permissible values of the range option
    //! @param[in] default_value
    //!     default value of the range option as an integer
    range_option(char const* name,
                 std::initializer_list<int64_t>&& choices,
                 int64_t default_value);

    //! Construct a range option with the given name and permissible values.
    //! @param[in] name
    //!     name of the range option
    //! @param[in] minimum_value
    //!     the minimum permissible value of the range option
    //! @param[in] maximum_value
    //!     the maximum permissible value of the range option
    //! @param[in] default_value
    //!     default value of the range option as an integer
    range_option(char const* name,
                 int64_t minimum_value,
                 int64_t maximum_value,
                 int64_t default_value);

    //! Construct a range option with the given name and permissible values.
    //! @param[in] name
    //!     name of the range option
    //! @param[in] short_name
    //!     short name of the range option
    //! @param[in] choices
    //!     a set of permissible values of the range option
    //! @param[in] default_value
    //!     default value of the range option as an integer
    range_option(char const* name,
                 char const* short_name,
                 std::initializer_list<int64_t>&& choices,
                 int64_t default_value);

    //! Construct a range option with the given name and permissible values.
    //! @param[in] name
    //!     name of the range option
    //! @param[in] short_name
    //!     short name of the range option
    //! @param[in] minimum_value
    //!     the minimum permissible value of the range option
    //! @param[in] maximum_value
    //!     the maximum permissible value of the range option
    //! @param[in] default_value
    //!     default value of the range option as an integer
    range_option(char const* name,
                 char const* short_name,
                 int64_t minimum_value,
                 int64_t maximum_value,
                 int64_t default_value);

  protected:
    int64_t _minimum_value; //!< Minimum permissible value for this argument.
    int64_t _maximum_value; //!< Maximum permissible value for this argument.

    //! A set of permissible values for this argument.
    std::set<int64_t> _choices;

  protected:
    virtual int parse(int argc, char const* const* argv, std::string* errors) override;
};

} // namespace qflags

#if defined(QFLAGS_STATIC)

#   define QFLAGS_INLINE

#else //!defined(QFLAGS_STATIC)

// Include implementation for use as a header-only library.
#   define QFLAGS_INLINE inline
#   include <qflags/command_line.hpp>
#   include <qflags/argument.hpp>
#   include <qflags/flag.hpp>
#   include <qflags/string_option.hpp>
#   include <qflags/boolean_option.hpp>
#   include <qflags/integer_option.hpp>
#   include <qflags/choice_option.hpp>
#   include <qflags/range_option.hpp>
#   include <qflags/parser.hpp>
#   include <qflags/command.hpp>

#endif //!defined(QFLAGS_STATIC)
