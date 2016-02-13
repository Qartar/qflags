// command_line.hpp
//

#include <qflags/qflags.h>

#if defined(_WINDOWS)
#include <Windows.h>
#include <mbctype.h>
#endif //defined(_WINDOWS)

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE command_line::command_line(wchar_t const* args)
{
#if defined(_WINDOWS)
    // Convert command line string to argv arguments.
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(args, &argc);

    // Parse the command line arguments.
    _init(argc, argv);

    // Free the memory allocated by CommandLineArgvW
    LocalFree(argv);
#endif //!defined(_WINDOWS)
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
QFLAGS_INLINE command_line::command_line(int argc, char const* const* argv, char const* locale)
{
#if defined(_WINDOWS)
    // Determine the code page for the argument strings.
    UINT code_page = CP_UTF8; // default, no conversion
    if (locale && *locale == '\0') {
        code_page = GetACP(); // Use the system's default ANSI code page.
    } else if (locale) {
        wchar_t wlocale[1024]; // Inappropriately large buffer.

        // Convert locale to a wide-character string using default encoding.
        swprintf_s(wlocale, L"%S", locale);

        // Retrieve the default ANSI code page for the specified locale.
        GetLocaleInfoEx(wlocale,
                        LOCALE_IDEFAULTANSICODEPAGE|LOCALE_RETURN_NUMBER,
                        reinterpret_cast<wchar_t*>(&code_page),
                        sizeof(code_page) / sizeof(wchar_t));
    }

    // Convert argument strings to UTF-16 and then to UTF-8 since converting
    // directly to UTF-8 from other single-byte or multi-byte encodings is not
    // supported by the Windows API.
    if (code_page != CP_UTF8) {
        std::vector<wchar_t> wargs;
        std::vector<wchar_t*> wargv;

        // Determine required size.
        int total_length_in_characters = 0;
        for (int ii = 0; ii < argc; ++ii) {
            total_length_in_characters += MultiByteToWideChar(
                code_page,              // CodePage
                0,                      // dwFlags
                argv[ii],               // lpMultiByteStr
                -1,                     // cbMultiByte
                NULL,                   // lpWideCharStr
                0);                     // cchWideChar
        }

        wargv.reserve(argc);
        wargs.resize(total_length_in_characters, 0);
        wchar_t* wargs_ptr = wargs.data();
        wchar_t const* const wargs_end = wargs.data() + wargs.size();

        // Convert arguments to UTF-16.
        for (int ii = 0; ii < argc; ++ii) {
            int characters_remaining = static_cast<int>(wargs_end - wargs_ptr);
            int length_in_characters = MultiByteToWideChar(
                code_page,              // CodePage
                0,                      // dwFlags
                argv[ii],               // lpMultiByteStr
                -1,                     // cbMultiByte
                wargs_ptr,              // lpWideCharStr
                characters_remaining);  // cchWideChar

            wargv.push_back(wargs_ptr);
            wargs_ptr += length_in_characters;
        }

        // Initialize from UTF-16
        _init(argc, wargv.data());
    } else {
        _init(argc, argv);
    }
#else //defined(_WINDOWS)
    _init(argc, argv); locale;
#endif //!defined(_WINDOWS)
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
QFLAGS_INLINE command_line::command_line(command_line const& other) {
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
QFLAGS_INLINE command_line& command_line::operator=(command_line const& other) {
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
QFLAGS_INLINE void command_line::_init(int argc, wchar_t const* const* wargv) {
#if defined(_WINDOWS)
    // Determine required size.
    int total_length_in_bytes = 0;
    for (int ii = 0; ii < argc; ++ii) {
        total_length_in_bytes += WideCharToMultiByte(
            CP_UTF8,            // CodePage
            0,                  // dwFlags
            wargv[ii],          // lpWideCharStr
            -1,                 // cchWideChar
            NULL,               // lpMultiByteStr
            0,                  // cbMultiByte
            NULL,               // lpDefaultChar
            NULL);              // lpUsedDefaultChar
    }

    _argv.reserve(argc + 1);
    // Since the string is NUL delimited add an extra NUL to mark the end.
    _args.resize(total_length_in_bytes + 1, 0);
    char* args_ptr = _args.data();
    char const* const args_end = _args.data() + _args.size();

    // Convert arguments to UTF-8.
    for (int ii = 0; ii < argc; ++ii) {
        int bytes_remaining = static_cast<int>(args_end - args_ptr);
        int length_in_bytes = WideCharToMultiByte(
            CP_UTF8,            // CodePage
            0,                  // dwFlags
            wargv[ii],          // lpWideCharStr
            -1,                 // cchWideChar
            args_ptr,           // lpMultiByteStr
            bytes_remaining,    // cbMultiByte
            NULL,               // lpDefaultChar
            NULL);              // lpUsedDefaultChar

        _argv.push_back(args_ptr);
        args_ptr += length_in_bytes;
    }
#endif //defined(_WINDOWS)

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
    char const* const args_end = _args.data() + _args.size();

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
