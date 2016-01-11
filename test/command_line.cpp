#include <gtest/gtest.h>
#include <qflags/qflags.h>

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
    EXPECT_EQ(std::string(u8"test.exe"), command_line.argv(0));
    EXPECT_EQ(std::string(u8"-abc"), command_line.argv(1));
    EXPECT_EQ(std::string(u8"--one"), command_line.argv(2));
    EXPECT_EQ(std::string(u8"--two"), command_line.argv(3));
    EXPECT_EQ(std::string(u8"--three"), command_line.argv(4));
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
    int argc = _countof(wargv);

    auto command_line = qflags::command_line(argc, wargv);

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string(u8"test.exe"), command_line.argv(0));
    EXPECT_EQ(std::string(u8"-abc"), command_line.argv(1));
    EXPECT_EQ(std::string(u8"--one"), command_line.argv(2));
    EXPECT_EQ(std::string(u8"--two"), command_line.argv(3));
    EXPECT_EQ(std::string(u8"--three"), command_line.argv(4));
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
        L"--\u010338\u010342\u010334\u010339\u010343",  // Gothic (non-BMP)
    };
    int argc = _countof(wargv);

    auto command_line = qflags::command_line(argc, wargv);

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string(u8"\u0444\u0435\u0443\u0444.exe"), command_line.argv(0));
    EXPECT_EQ(std::string(u8"-\u05d0\u05d1\u05d2"), command_line.argv(1));
    EXPECT_EQ(std::string(u8"--\u13c0\u13ec"), command_line.argv(2));
    EXPECT_EQ(std::string(u8"--\u0926\u094b"), command_line.argv(3));
    EXPECT_EQ(std::string(u8"--\u010338\u010342\u010334\u010339\u010343"), command_line.argv(4));
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
        u8"test.exe",
        u8"-abc",
        u8"--one",
        u8"--two",
        u8"--three",
    };
    int argc = _countof(argv);

    auto command_line = qflags::command_line(argc, argv, nullptr);

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string(u8"test.exe"), command_line.argv(0));
    EXPECT_EQ(std::string(u8"-abc"), command_line.argv(1));
    EXPECT_EQ(std::string(u8"--one"), command_line.argv(2));
    EXPECT_EQ(std::string(u8"--two"), command_line.argv(3));
    EXPECT_EQ(std::string(u8"--three"), command_line.argv(4));
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
    int argc = _countof(argv);

    auto command_line = qflags::command_line(argc, argv, "ru-RU");

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string(u8"\u0442\u0435\u0441\u0442.exe"), command_line.argv(0));
    EXPECT_EQ(std::string(u8"-\u0430\u0431\u0432"), command_line.argv(1));
    EXPECT_EQ(std::string(u8"--\u043e\u0434\u0438\u043d"), command_line.argv(2));
    EXPECT_EQ(std::string(u8"--\u0434\u0432\u0430"), command_line.argv(3));
    EXPECT_EQ(std::string(u8"--\u0442\u0440\u0438"), command_line.argv(4));
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
    int argc = _countof(argv);

    auto command_line = qflags::command_line(argc, argv, "en-US");

    ASSERT_EQ(5, command_line.argc());
    EXPECT_EQ(std::string(u8"pr\u00f8ve.exe"), command_line.argv(0));
    EXPECT_EQ(std::string(u8"-\u0160\u017d\u0153"), command_line.argv(1));
    EXPECT_EQ(std::string(u8"--amh\u00e1in"), command_line.argv(2));
    EXPECT_EQ(std::string(u8"--k\u00e9t"), command_line.argv(3));
    EXPECT_EQ(std::string(u8"--\u00fer\u00edr"), command_line.argv(4));
    EXPECT_EQ(nullptr, command_line.argv(5));
}
