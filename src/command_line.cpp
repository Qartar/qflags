// command_line.cpp
//

#include <qflags/qflags.h>

#if defined(_WINDOWS)
#include <Windows.h>
#endif //defined(_WINDOWS)

namespace qflags {

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
command_line::command_line(wchar_t const* args)
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
command_line::command_line(int argc, wchar_t const* const* argv)
{
    _init(argc, argv);
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
command_line::command_line(int argc, char const* const* argv, char const* locale)
{
    // Specified locale is UTF-8 so no conversion is neccesary.
    if (std::locale(locale) == std::locale("en_US.UTF8")) {
        _init(argc, argv);
    } else {
        // ...
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
void command_line::_init(int argc, wchar_t const* const* wargv) {
#if defined(_WINDOWS)
    // Determine required size.
    int total_length_in_bytes = 0;
    for (int ii = 0; ii < argc; ++ii) {
        total_length_in_bytes += WideCharToMultiByte(
            CP_UTF8,            // CodePage
            0,                  // dwFlags
            wargv[ii],          // lpWideCharStr
            -1,                 // ccWideChar
            NULL,               // lpMultiByteStr
            0,                  // cbMultiByte
            NULL,               // lpDefaultChar
            NULL);              // lpUsedDefaultChar
    }

    _argv.reserve(argc);
    _args.resize(total_length_in_bytes);
    char* args_ptr = _args.data();
    char const* const args_end = _args.data() + _args.size();

    // Convert arguments to UTF-8
    for (int ii = 0; ii < argc; ++ii) {
        int bytes_remaining = static_cast<int>(args_end - args_ptr);
        int length_in_bytes = WideCharToMultiByte(
            CP_UTF8,            // CodePage
            0,                  // dwFlags
            wargv[ii],          // lpWideCharStr
            -1,                 // ccWideChar
            args_ptr,           // lpMultiByteStr
            bytes_remaining,    // cbMultiByte
            NULL,               // lpDefaultChar
            NULL);              // lpUsedDefaultChar

        _argv.push_back(args_ptr);
        args_ptr += length_in_bytes;
    }
#endif //defined(_WINDOWS)
}

////////////////////////////////////////////////////////////////////////////////
/**
 *
 */
void command_line::_init(int, char const* const*)
{
}

} // namespace qflags
