// command_line.hpp
//

#include <qflags/qflags.h>

#include <locale>
#include <codecvt>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE command_line::command_line(wchar_t const* wargs)
{
    _init(wargs);
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE command_line::command_line(int argc, wchar_t const* const* argv)
{
    _init(argc, argv);
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE command_line::command_line(char const* args, char const* locale)
{
    // Suppress encoding conversion.
    if (locale == nullptr) {
        _init(args);
    }
    // Convert multibyte string to UTF-16 and then to UTF-8 since codecvt does
    // not support conversions between multibyte encodings.
    else {
        auto conv = new std::codecvt_byname<wchar_t, char, std::mbstate_t>(locale);
        std::mbstate_t state = std::mbstate_t();

        // Count up the number of wide characters required.
        auto args_len = strlen(args);
        int total_length_in_characters = 1 + conv->length(state,
                                                          args,
                                                          args + args_len,
                                                          SIZE_MAX);

        std::vector<wchar_t> wargs(total_length_in_characters, 0);
        wchar_t* wargs_ptr = wargs.data();

        // Convert argument string to UTF-16.
        conv->in(state,
                 args,
                 args + args_len,
                 args,
                 wargs.data(),
                 wargs.data() + wargs.size(),
                 wargs_ptr);

        *wargs_ptr++ = '\0';

        _init(wargs.data());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE command_line::command_line(int argc, char const* const* argv, char const* locale)
{
    // Suppress encoding conversion.
    if (locale == nullptr) {
        _init(argc, argv);
    }
    // Convert multibyte strings to UTF-16 and then to UTF-8 since codecvt does
    // not support conversions between multibyte encodings.
    else {
        auto conv = new std::codecvt_byname<wchar_t, char, std::mbstate_t>(locale);
        std::mbstate_t state = std::mbstate_t();

        // Count up the number of wide characters required.
        int total_length_in_characters = 0;
        for (int ii = 0; ii < argc; ++ii) {
            auto arg_len = strlen(argv[ii]);
            total_length_in_characters += 1 + conv->length(state,
                                                           argv[ii],
                                                           argv[ii] + arg_len,
                                                           SIZE_MAX);
        }

        std::vector<wchar_t> wargs(total_length_in_characters, 0);
        std::vector<wchar_t*> wargv(argc, nullptr);
        wchar_t* wargs_ptr = wargs.data();
        wchar_t* wargs_end = wargs.data() + wargs.size();

        // Convert arguments to UTF-16.
        for (int ii = 0; ii < argc; ++ii) {
            auto arg_len = strlen(argv[ii]);
            char const* arg_next = argv[ii];

            wargv[ii] = wargs_ptr;

            conv->in(state,
                     argv[ii],
                     argv[ii] + arg_len,
                     arg_next,
                     wargs_ptr,
                     wargs_end,
                     wargs_ptr);

            *wargs_ptr++ = '\0';
        }

        _init(argc, wargv.data());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE command_line::command_line(command_line const& other)
{
    _args = other._args;
    // Initial argument pointers to local argument string.
    _argv.resize(other._argv.size());
    for (size_t ii = 0; ii < other._argv.size() - 1; ++ii) {
        ptrdiff_t offset = other._argv[ii] - other._args.data();
        _argv[ii] = _args.data() + offset;
    }

    // Follow C++ standard which specifies the value of argv[argc] shall be 0.
    _argv.back() = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE command_line& command_line::operator=(command_line const& other)
{
    _args = other._args;
    // Initial argument pointers to local argument string.
    _argv.resize(other._argv.size());
    for (size_t ii = 0; ii < other._argv.size() - 1; ++ii) {
        ptrdiff_t offset = other._argv[ii] - other._args.data();
        _argv[ii] = _args.data() + offset;
    }

    // Follow C++ standard which specifies the value of argv[argc] shall be 0.
    _argv.back() = nullptr;

    return *this;
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE void command_line::_init(wchar_t const* wargs)
{
    // Convert wide-character argument string to UTF-8 and parse.
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::string args = conv.to_bytes(wargs);

    _init(args.c_str());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Rules for parsing commmand line arguments taken from MSDN:
 *
 * "Parsing C Command-Line Arguments"
 * https://msdn.microsoft.com/en-us/library/a1y7w461.aspx
 */
QFLAGS_INLINE void command_line::_init(char const* args)
{
    // Skip leading whitespace.
    while (args && isblank((unsigned char)*args)) {
        ++args;
    }

    auto args_end = args + strlen(args);
    _args.resize((args_end - args) + 2);
    auto argv_ptr = _args.data();

    while (args && *args) {

        bool inside_quotes = false;
        _argv.push_back(argv_ptr);

        while (*args) {
            // "Arguments are delimited by whitespace, which is either a space
            // or a tab."
            if (!inside_quotes && isblank((unsigned char)*args)) {
                *argv_ptr++ = '\0';
                break;
            }
            // "A string surrounded by double quotation marks is interpreted as
            // a single argument, regardless of white space contained within. A
            // quoted string can be embedded in an argument."
            else if (*args == '\"') {
                inside_quotes = !inside_quotes;
                ++args;
            }
            else if (*args == '\\') {

                // Count the number of consecutive backslashes.
                int ns = 1;
                while (*(args+ns) == '\\') {
                    ++ns;
                }

                // "A double quotation mark preceded by a backslash, \", is
                // interpreted as a literal double quotation mark (")."
                if (*(args+1) == '\"') {
                    ++args; // Skip escaping backslash.
                    *argv_ptr++ = *args++;
                }
                // "Backslashes are interpreted literally, unless they
                // immediately precede a double quotation mark.
                else if (*(args+ns) != '\"') {
                    while (ns--) {
                        *argv_ptr++ = *args++;
                    }
                }
                // "If an even number of backslashes is followed by a double
                // quotation mark, then one backslash (\) is placed in the argv
                // array for every pair of backslashes (\\), and the double
                // quotation mark (") is interpreted as a string delimiter."
                else if ((ns & 1) == 0) {
                    while ((ns -= 2) > -1) {
                        ++args; // Skip escaping backslash.
                        *argv_ptr++ = *args++;
                    }
                    inside_quotes = !inside_quotes;
                    ++args; // Skip string delimiter.
                }
                // "If an odd number of backslashes is followed by a double
                // quotation mark, then one backslash (\) is placed in the argv
                // array for every pair of backslashes (\\) and the double
                // quotation mark is interpreted as an escape sequence by the
                // remaining backslash, causing a literal double quotation mark
                // (") to be placed in argv."
                else {
                    while ((ns -= 2) > -2) {
                        ++args; // Skip escaping backslash.
                        *argv_ptr++ = *args++;
                    }
                    *argv_ptr++ = *args++;
                }
            // Otherwise copy the character.
            } else {
                *argv_ptr++ = *args++;
            }
        }

        // Skip trailing whitespace.
        while (isblank((unsigned char)*args)) {
            ++args;
        }
    }

    // The original NUL terminator is not copied in the loop above.
    *argv_ptr++ = '\0';

    // Since the string is NUL delimited add an extra NUL to mark the end.
    *argv_ptr++ = '\0';

    // Follow C++ standard which specifies the value of argv[argc] shall be 0.
    _argv.push_back(nullptr);
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE void command_line::_init(int argc, wchar_t const* const* wargv)
{
    auto conv = new std::codecvt_utf8_utf16<wchar_t>();
    std::mbstate_t state = std::mbstate_t();

    // codecvt cannot find the length of an output string so count up the number
    // of wide characters, reserve the 'worst-case' size and shrink afterwards.
    size_t total_length_in_characters = 0;
    for (int ii = 0; ii < argc; ++ii) {
        total_length_in_characters += wcslen(wargv[ii]);
    }

    std::vector<char> args(total_length_in_characters * 4 + 1, 0);
    std::vector<char*> argv(argc);
    char* args_ptr = args.data();
    char* args_end = args.data() + args.size();

    // Convert arguments to UTF-8
    for (int ii = 0; ii < argc; ++ii) {
        auto warg_len = wcslen(wargv[ii]);
        wchar_t const* warg_next = wargv[ii];

        argv[ii] = args_ptr;

        conv->out(state,
                  wargv[ii],
                  wargv[ii] + warg_len,
                  warg_next,
                  args_ptr,
                  args_end,
                  args_ptr);

        *args_ptr++ = '\0';
    }

    // Since the string is NUL delimited add an extra NUL to mark the end.
    *args_ptr++ = '\0';

    // Shrink args to fit.
    _args.assign(args.data(), args_ptr);
    _argv.reserve(argc + 1);

    // Update argv array.
    for (int ii = 0; ii < argc; ++ii) {
        _argv.push_back(_args.data() + (argv[ii] - args.data()));
    }

    // Follow C++ standard which specifies the value of argv[argc] shall be 0.
    _argv.push_back(nullptr);
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE void command_line::_init(int argc, char const* const* argv)
{
    // Determine required size.
    size_t total_length_in_bytes = 0;
    for (int ii = 0; ii < argc; ++ii) {
        total_length_in_bytes += 1 + strlen(argv[ii]);
    }

    _argv.reserve(argc + 1);
    // Since the string is NUL delimited add an extra NUL to mark the end.
    _args.resize(total_length_in_bytes + 1, 0);
    char* args_ptr = _args.data();
#if defined(_WINDOWS)
    char const* const args_end = _args.data() + _args.size();
#endif //defined(_WINDOWS)

    // Copy arguments into string buffer.
    for (int ii = 0; ii < argc; ++ii) {
        size_t length_in_bytes = 1 + strlen(argv[ii]);
#if defined(_WINDOWS)
        strncpy_s(args_ptr,
                  args_end - args_ptr,
                  argv[ii],
                  length_in_bytes);
#else //defined(_WINDOWS)
        strncpy(args_ptr, argv[ii], length_in_bytes);
#endif //!defined(_WINDOWS)
        _argv.push_back(args_ptr);
        args_ptr += length_in_bytes;
    }

    // Follow C++ standard which specifies the value of argv[argc] shall be 0.
    _argv.push_back(nullptr);
}

} // namespace qflags
