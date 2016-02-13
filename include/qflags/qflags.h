// qflags.h
//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>

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

////////////////////////////////////////////////////////////////////////////////
/**
 * @class argument
 */
class argument
{
  public:
    //! @return
    //!     name of the argument
    std::string const& name() const { return _name; }

    //! @return
    //!     true if the argument was explicitly set by the command line
    bool is_set() const { return _is_set; }

    //! @return
    //!     true if the argument is a flag
    virtual bool is_flag() const { return false; }

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

    std::string _name;  //!< name of this argument
    bool _is_set;       //!< true if the argument was set by the command line

  protected:
    argument(char const* name);

    //! Process the command line arguments for this argument.
    //! @param[in] argc
    //!     the number of arguments in the argv array
    //! @param[in] argv
    //!     an array of arguments starting at this argument's name
    //! @param[out] errors
    //!     a string describing any errors that occurred during parsing
    //! @return
    //!     the number of command line arguments consumed by this argument, or
    //!     -1 if parsing failed.
    virtual int parse(int argc, char const* const* argv, std::string* errors) = 0;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class parser
 */
class parser
{
  public:
    parser();

    //! add an argument to the parser
    //! @param[in] arg
    //!     an argument, a reference is held by the parser
    bool add_argument(argument* arg);

    /**
     * parse the command line.
     */
    bool parse(command_line const& command_line, std::string* errors);

    //!
    argument const& operator[](std::string const& name) const;

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
    command_line _command_line;
    command_line _remaining;
    std::map<std::string, argument*> _arguments;
    std::vector<argument*> _commands;
    std::vector<argument*> _flags;

  private:
    bool parse_command(int argc, char const* const* argv, std::string* errors);
    bool parse_flags(char const* arg, std::string* errors);
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class flag
 */
class flag
    : public argument
{
  public:
    flag(char const* name);
    flag(char const* name, char const* short_name);

    virtual bool is_flag() const final { return true; }

    virtual bool is_boolean() const final { return true; }

    //! @return
    //!     true if the flag was explicitly set by the command line
    virtual bool value_boolean() const final { return _is_set; }

  protected:
    virtual int parse(int argc, char const* const* argv, std::string* errors) override;

  private:
    std::string _short_name;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class option
 */
class option
    : public argument
{
  public:

    //! All options are implicitly convertible to a string.
    virtual bool is_string() const final { return true; }

    //! @return
    //!     argument value as a string
    virtual std::string const& value_string() const override { return _value_string; }

  protected:
    std::string _value_string;

  protected:
    option(char const* name, char const* default_value = "") :
        argument(name),
        _value_string(default_value ? default_value : "") {}
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class string_option
 */
class string_option
    : public option
{
  public:
    string_option(char const* name, char const* default_value = "");

  protected:
    std::string _default_value;

  protected:
    virtual int parse(int argc, char const* const* argv, std::string* errors) override;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class boolean_option
 */
class boolean_option
    : public option
{
  public:
    boolean_option(char const* name, bool default_value = false);

    virtual bool is_boolean() const final { return true; }

    //! @return
    //!     argument value as a boolean
    bool value_boolean() const override { return _value_boolean; }

  protected:
      bool _value_boolean;
      bool _default_value;

  protected:
    virtual int parse(int argc, char const* const* argv, std::string* errors) override;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class integer_option
 */
class integer_option
    : public option
{
  public:
    integer_option(char const* name, int64_t default_value = 0);

    virtual bool is_integer() const final { return true; }

    //! @return
    //!     the argument value as an integer if `is_integer` is true
    virtual int64_t value_integer() const override { return _value_integer; }

  protected:
    int64_t _value_integer;
    int64_t _default_value;

  protected:
    virtual int parse(int argc, char const* const* argv, std::string* errors) override;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class choice_option
 */
class choice_option
    : public string_option
{
  public:
    choice_option(char const* name,
                  std::initializer_list<char const*>&& choices,
                  char const* default_value);

    choice_option(char const* name,
                  std::set<std::string> const& choices,
                  char const* default_value);

  protected:
    std::set<std::string> _choices;

  protected:
    virtual int parse(int argc, char const* const* argv, std::string* errors) override;
};

////////////////////////////////////////////////////////////////////////////////
/**
 * @class range_option
 */
class range_option
    : public integer_option
{
  public:
    range_option(char const* name,
                 std::initializer_list<int64_t>&& choices,
                 int64_t default_value);

    range_option(char const* name,
                 int64_t minimum_value,
                 int64_t maximum_value,
                 int64_t default_value);

  protected:
    int64_t _minimum_value;
    int64_t _maximum_value;
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

#endif //!defined(QFLAGS_STATIC)
