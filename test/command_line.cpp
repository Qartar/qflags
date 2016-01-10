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
    EXPECT_EQ(std::string("test.exe"), command_line.argv(0));
    EXPECT_EQ(std::string("-abc"), command_line.argv(1));
    EXPECT_EQ(std::string("--one"), command_line.argv(2));
    EXPECT_EQ(std::string("--two"), command_line.argv(3));
    EXPECT_EQ(std::string("--three"), command_line.argv(4));
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
    EXPECT_EQ(std::string("test.exe"), command_line.argv(0));
    EXPECT_EQ(std::string("-abc"), command_line.argv(1));
    EXPECT_EQ(std::string("--one"), command_line.argv(2));
    EXPECT_EQ(std::string("--two"), command_line.argv(3));
    EXPECT_EQ(std::string("--three"), command_line.argv(4));
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
}
