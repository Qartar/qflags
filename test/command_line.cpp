#include <gtest/gtest.h>
#include <qflags/qflags.h>

////////////////////////////////////////////////////////////////////////////////
/**
 * Test handling of unusual and invalid parameters.
 */
TEST(command_list_test, construct_argc_zero)
{
    char const* argv[] = { "" };
    auto command_line = qflags::command_line(0, argv);

    ASSERT_EQ(0, command_line.argc());
    EXPECT_EQ(nullptr, command_line.argv(0));
}

TEST(command_list_test, construct_argv_null_argc_zero)
{
    char const** argv = nullptr;
    auto command_line = qflags::command_line(0, argv);

    ASSERT_EQ(0, command_line.argc());
    EXPECT_EQ(nullptr, command_line.argv(0));
}

TEST(command_list_test, construct_argv_pnull_argc_zero)
{
    char const* args = nullptr;
    auto command_line = qflags::command_line(0, &args);

    ASSERT_EQ(0, command_line.argc());
    EXPECT_EQ(nullptr, command_line.argv(0));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that a single wide-character string is correctly delimited and converted
 * to UTF-8.
 */
TEST(command_line_test, wargs)
{
    wchar_t const* wargs = L"test.exe -abc --one --two --three";

    auto command_line = qflags::command_line(wargs);

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string("test.exe"), command_line.argv(0));
    EXPECT_EQ(std::string("-abc"), command_line.argv(1));
    EXPECT_EQ(std::string("--one"), command_line.argv(2));
    EXPECT_EQ(std::string("--two"), command_line.argv(3));
    EXPECT_EQ(std::string("--three"), command_line.argv(4));
    EXPECT_EQ(nullptr, command_line.argv(5));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that wide-character argument arrays are correctly converted to UTF-8.
 */
TEST(command_line_test, wargv)
{
    wchar_t const* wargv[] = 
    {
        L"test.exe",
        L"-abc",
        L"--one",
        L"--two",
        L"--three",
    };
    auto command_line = qflags::command_line(wargv);

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string("test.exe"), command_line.argv(0));
    EXPECT_EQ(std::string("-abc"), command_line.argv(1));
    EXPECT_EQ(std::string("--one"), command_line.argv(2));
    EXPECT_EQ(std::string("--two"), command_line.argv(3));
    EXPECT_EQ(std::string("--three"), command_line.argv(4));
    EXPECT_EQ(nullptr, command_line.argv(5));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that UTF-16 code points are correctly preserved when converted to UTF-8.
 */
TEST(command_list_test, wargv_utf16)
{
    wchar_t const* wargv[] = 
    {
        L"\u0444\u0435\u0443\u0444.exe",                // Cyrillic
        L"-\u05d0\u05d1\u05d2",                         // Hebrew
        L"--\u13c0\u13ec",                              // Cherokee
        L"--\u0926\u094b",                              // Devanagari
        L"--\U00010338\U00010342\U00010334\U00010339\U00010343",  // Gothic (non-BMP)
    };
    auto command_line = qflags::command_line(wargv);

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string("\xd1\x84\xd0\xb5\xd1\x83\xd1\x84.exe"), command_line.argv(0));
    EXPECT_EQ(std::string("-\xd7\x90\xd7\x91\xd7\x92"), command_line.argv(1));
    EXPECT_EQ(std::string("--\xe1\x8f\x80\xe1\x8f\xac"), command_line.argv(2));
    EXPECT_EQ(std::string("--\xe0\xa4\xa6\xe0\xa5\x8b"), command_line.argv(3));
    EXPECT_EQ(std::string("--\xf0\x90\x8c\xb8\xf0\x90\x8d\x82\xf0\x90\x8c\xb4\xf0\x90\x8c\xb9\xf0\x90\x8d\x83"), command_line.argv(4));
    EXPECT_EQ(nullptr, command_line.argv(5));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that a single character string is correctly delimited and converted to
 * UTF-8.
 */
TEST(command_line_test, args)
{
    char const* args = "test.exe -abc --one --two --three";

    auto command_line = qflags::command_line(args);

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string("test.exe"), command_line.argv(0));
    EXPECT_EQ(std::string("-abc"), command_line.argv(1));
    EXPECT_EQ(std::string("--one"), command_line.argv(2));
    EXPECT_EQ(std::string("--two"), command_line.argv(3));
    EXPECT_EQ(std::string("--three"), command_line.argv(4));
    EXPECT_EQ(nullptr, command_line.argv(5));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that ANSI Cyrillic encoded strings are passed through correctly.
 */
TEST(command_list_test, args_1251)
{
    char const* args =
        "\xf2\xe5\xf1\xf2.exe " // Russian 'тест.exe'
        "-\xe0\xe1\xe2 "        // Russian '-абв'
        "--\xee\xe4\xe8\xed "   // Russian '--одно'
        "--\xe4\xe2\xe0 "       // Russian '--два'
        "--\xf2\xf0\xe8 "       // Russian '--три'
    ;
    auto command_line = qflags::command_line(args, "ru-RU");

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string("\xd1\x82\xd0\xb5\xd1\x81\xd1\x82.exe"), command_line.argv(0));
    EXPECT_EQ(std::string("-\xd0\xb0\xd0\xb1\xd0\xb2"), command_line.argv(1));
    EXPECT_EQ(std::string("--\xd0\xbe\xd0\xb4\xd0\xb8\xd0\xbd"), command_line.argv(2));
    EXPECT_EQ(std::string("--\xd0\xb4\xd0\xb2\xd0\xb0"), command_line.argv(3));
    EXPECT_EQ(std::string("--\xd1\x82\xd1\x80\xd0\xb8"), command_line.argv(4));
    EXPECT_EQ(nullptr, command_line.argv(5));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that ANSI Latin-1 encoded strings are passed through correctly.
 */
TEST(command_list_test, args_1252)
{
    char const* args =
        "pr\xf8ve.exe "     // Danish 'prøve.exe'
        "-\x8a\x8e\x9c "    // Differences from ISO-8859-1 '-ŠŽŸ'
        "--amh\xe1in "      // Irish '--amháin'
        "--k\xe9t "         // Hungarian '--két'
        "--\xfer\xedr "     // Icelandic '--þrír'
    ;
    auto command_line = qflags::command_line(args, "en-US");

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string("pr\xc3\xb8ve.exe"), command_line.argv(0));
    EXPECT_EQ(std::string("-\xc5\xa0\xc5\xbd\xc5\x93"), command_line.argv(1));
    EXPECT_EQ(std::string("--amh\xc3\xa1in"), command_line.argv(2));
    EXPECT_EQ(std::string("--k\xc3\xa9t"), command_line.argv(3));
    EXPECT_EQ(std::string("--\xc3\xber\xc3\xadr"), command_line.argv(4));
    EXPECT_EQ(nullptr, command_line.argv(5));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that UTF-8 encoded strings are passed through correctly.
 */
TEST(command_list_test, argv_utf8)
{
    char const* argv[] =
    {
        "test.exe",
        "-abc",
        "--one",
        "--two",
        "--three",
    };
    auto command_line = qflags::command_line(argv, nullptr);

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string("test.exe"), command_line.argv(0));
    EXPECT_EQ(std::string("-abc"), command_line.argv(1));
    EXPECT_EQ(std::string("--one"), command_line.argv(2));
    EXPECT_EQ(std::string("--two"), command_line.argv(3));
    EXPECT_EQ(std::string("--three"), command_line.argv(4));
    EXPECT_EQ(nullptr, command_line.argv(5));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that ANSI Cyrillic encoded strings are passed through correctly.
 */
TEST(command_list_test, argv_1251)
{
    char const* argv[] =
    {
        "\xf2\xe5\xf1\xf2.exe", // Russian 'тест.exe'
        "-\xe0\xe1\xe2",        // Russian '-абв'
        "--\xee\xe4\xe8\xed",   // Russian '--одно'
        "--\xe4\xe2\xe0",       // Russian '--два'
        "--\xf2\xf0\xe8",       // Russian '--три'
    };
    auto command_line = qflags::command_line(argv, "ru-RU");

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string("\xd1\x82\xd0\xb5\xd1\x81\xd1\x82.exe"), command_line.argv(0));
    EXPECT_EQ(std::string("-\xd0\xb0\xd0\xb1\xd0\xb2"), command_line.argv(1));
    EXPECT_EQ(std::string("--\xd0\xbe\xd0\xb4\xd0\xb8\xd0\xbd"), command_line.argv(2));
    EXPECT_EQ(std::string("--\xd0\xb4\xd0\xb2\xd0\xb0"), command_line.argv(3));
    EXPECT_EQ(std::string("--\xd1\x82\xd1\x80\xd0\xb8"), command_line.argv(4));
    EXPECT_EQ(nullptr, command_line.argv(5));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that ANSI Latin-1 encoded strings are passed through correctly.
 */
TEST(command_list_test, argv_1252)
{
    char const* argv[] =
    {
        "pr\xf8ve.exe",     // Danish 'prøve.exe'
        "-\x8a\x8e\x9c",    // Differences from ISO-8859-1 '-ŠŽŸ'
        "--amh\xe1in",      // Irish '--amháin'
        "--k\xe9t",         // Hungarian '--két'
        "--\xfer\xedr",     // Icelandic '--þrír'
    };
    auto command_line = qflags::command_line(argv, "en-US");

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string("pr\xc3\xb8ve.exe"), command_line.argv(0));
    EXPECT_EQ(std::string("-\xc5\xa0\xc5\xbd\xc5\x93"), command_line.argv(1));
    EXPECT_EQ(std::string("--amh\xc3\xa1in"), command_line.argv(2));
    EXPECT_EQ(std::string("--k\xc3\xa9t"), command_line.argv(3));
    EXPECT_EQ(std::string("--\xc3\xber\xc3\xadr"), command_line.argv(4));
    EXPECT_EQ(nullptr, command_line.argv(5));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test copy assignment.
 */
TEST(command_list_test, copy_assignment)
{
    const char** dst_argv = nullptr;
    auto dst = qflags::command_line(0, dst_argv);
    {
        char const* argv[] = { "one", "two", "three" };
        auto src = qflags::command_line(argv);
        dst = src; // copy assignment
    }
    ASSERT_EQ(3, dst.argc());
    EXPECT_EQ(std::string("one"), dst.argv(0));
    EXPECT_EQ(std::string("two"), dst.argv(1));
    EXPECT_EQ(std::string("three"), dst.argv(2));
    EXPECT_EQ(nullptr, dst.argv(3));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test copy construction.
 */
TEST(command_list_test, copy_construction)
{
    char const* argv[] = { "one", "two", "three" };
    auto src = qflags::command_line(argv);
    auto dst = qflags::command_line(src); // copy construction
    src = qflags::command_line(0, argv); // reset src

    ASSERT_EQ(3, dst.argc());
    EXPECT_EQ(std::string("one"), dst.argv(0));
    EXPECT_EQ(std::string("two"), dst.argv(1));
    EXPECT_EQ(std::string("three"), dst.argv(2));
    EXPECT_EQ(nullptr, dst.argv(3));
}
