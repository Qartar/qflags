#include <gtest/gtest.h>
#include <qflags/qflags.h>

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that flag correctly reports its capabilities.
 */
TEST(flag_test, capabilities)
{
    auto name = std::string("flag");
    auto flag = qflags::flag(name.c_str());

    EXPECT_EQ(name, flag.name());
    EXPECT_EQ(false, flag.is_set());
    EXPECT_EQ(true, flag.is_flag());
    EXPECT_EQ(false, flag.is_command());
    EXPECT_EQ(true, flag.is_boolean());
    EXPECT_EQ(false, flag.is_integer());
    EXPECT_EQ(false, flag.is_string());

    EXPECT_EQ(false, flag.value_boolean());
    EXPECT_THROW(flag.value_integer(), std::logic_error);
    EXPECT_THROW(flag.value_string(), std::logic_error);

    EXPECT_EQ(false, static_cast<bool>(flag));
    EXPECT_THROW(static_cast<int64_t>(flag), std::logic_error);
    EXPECT_THROW(static_cast<int>(flag), std::logic_error);
    EXPECT_THROW(static_cast<std::string>(flag), std::logic_error);
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle a flag using short form arguments.
 */
TEST(flag_test, parse_short_flag)
{
    auto parser = qflags::parser();

    char const* argv[] = { "-f" };
    auto command_line = qflags::command_line(argv);

    auto flag = qflags::flag("flag", "f");

    ASSERT_EQ(true, parser.add_argument(&flag));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(true, static_cast<bool>(parser["flag"]));
        EXPECT_EQ(true, static_cast<bool>(flag));
        EXPECT_EQ(1, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle multiple flags using short form arguments.
 */
TEST(flag_test, parse_short_flags)
{
    auto parser = qflags::parser();

    char const* argv[] = { "-ma" };
    auto command_line = qflags::command_line(argv);

    auto flag_m = qflags::flag("flag_m", "m");
    auto flag_i = qflags::flag("flag_i", "i");
    auto flag_a = qflags::flag("flag_a", "a");

    ASSERT_EQ(true, parser.add_argument(&flag_m));
    ASSERT_EQ(true, parser.add_argument(&flag_i));
    ASSERT_EQ(true, parser.add_argument(&flag_a));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(true, static_cast<bool>(parser["flag_m"]));
        EXPECT_EQ(false, static_cast<bool>(parser["flag_i"]));
        EXPECT_EQ(true, static_cast<bool>(parser["flag_a"]));
        EXPECT_EQ(true, static_cast<bool>(flag_m));
        EXPECT_EQ(false, static_cast<bool>(flag_i));
        EXPECT_EQ(true, static_cast<bool>(flag_a));
        EXPECT_EQ(1, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle multiple groups of flags using short form
 * arguments.
 */
TEST(flag_test, parse_short_flags_groups)
{
    auto parser = qflags::parser();

    char const* argv[] = { "-ac", "-df" };
    auto command_line = qflags::command_line(argv);

    auto flag_a = qflags::flag("flag_a", "a");
    auto flag_b = qflags::flag("flag_b", "b");
    auto flag_c = qflags::flag("flag_c", "c");
    auto flag_d = qflags::flag("flag_d", "d");
    auto flag_e = qflags::flag("flag_e", "e");
    auto flag_f = qflags::flag("flag_f", "f");
    auto flag_g = qflags::flag("flag_g");

    ASSERT_EQ(true, parser.add_argument(&flag_g));
    ASSERT_EQ(true, parser.add_argument(&flag_f));
    ASSERT_EQ(true, parser.add_argument(&flag_e));
    ASSERT_EQ(true, parser.add_argument(&flag_d));
    ASSERT_EQ(true, parser.add_argument(&flag_a));
    ASSERT_EQ(true, parser.add_argument(&flag_b));
    ASSERT_EQ(true, parser.add_argument(&flag_c));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(true, static_cast<bool>(parser["flag_a"]));
        EXPECT_EQ(false, static_cast<bool>(parser["flag_b"]));
        EXPECT_EQ(true, static_cast<bool>(parser["flag_c"]));
        EXPECT_EQ(true, static_cast<bool>(parser["flag_d"]));
        EXPECT_EQ(false, static_cast<bool>(parser["flag_e"]));
        EXPECT_EQ(true, static_cast<bool>(parser["flag_f"]));
        EXPECT_EQ(false, static_cast<bool>(parser["flag_g"]));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle a flag using long form arguments.
 */
TEST(flag_test, parse_long_flag)
{
    auto parser = qflags::parser();

    char const* argv[] = { "--flag" };
    auto command_line = qflags::command_line(argv);

    auto flag = qflags::flag("flag", "f");

    ASSERT_EQ(true, parser.add_argument(&flag));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(true, static_cast<bool>(parser["flag"]));
        EXPECT_EQ(true, static_cast<bool>(flag));
        EXPECT_EQ(1, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly detects an invalid short flag.
 */
TEST(flag_test, parse_invalid_short_flag)
{
    auto parser = qflags::parser();

    char const* argv[] = { "-f" };
    auto command_line = qflags::command_line(argv);

    {
        std::string errors;

        EXPECT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly detects an invalid short flag in a group.
 */
TEST(flag_test, parse_invalid_short_flag_group)
{
    auto parser = qflags::parser();

    char const* argv[] = { "-adc" };
    auto command_line = qflags::command_line(argv);

    auto flag_a = qflags::flag("flag_a", "a");
    auto flag_b = qflags::flag("flag_b", "b");
    auto flag_c = qflags::flag("flag_c", "c");

    ASSERT_EQ(true, parser.add_argument(&flag_a));
    ASSERT_EQ(true, parser.add_argument(&flag_b));
    ASSERT_EQ(true, parser.add_argument(&flag_c));
    {
        std::string errors;

        EXPECT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly handles multibyte short flag names.
 */
TEST(flag_test, parse_multibyte_short_flag)
{
    auto parser = qflags::parser();

    wchar_t const* argv[] = { L"-\U00010338" };
    auto command_line = qflags::command_line(argv);

    auto flag = qflags::flag("flag", "\xf0\x90\x8c\xb8");

    ASSERT_EQ(true, parser.add_argument(&flag));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(true, static_cast<bool>(parser["flag"]));
        EXPECT_EQ(true, static_cast<bool>(flag));
        EXPECT_EQ(1, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle multiple multibyte short flag names.
 */
TEST(flag_test, parse_multibyte_short_flags)
{
    auto parser = qflags::parser();

    wchar_t const* argv[] = { L"-\U00010338\U00010342\U00010334" };
    auto command_line = qflags::command_line(argv);

    auto thiuth = qflags::flag("thiuth", "\xf0\x90\x8c\xb8");
    auto raida = qflags::flag("raida", "\xf0\x90\x8d\x82");
    auto aihvus = qflags::flag("aihvus", "\xf0\x90\x8c\xb4");

    ASSERT_EQ(true, parser.add_argument(&thiuth));
    ASSERT_EQ(true, parser.add_argument(&raida));
    ASSERT_EQ(true, parser.add_argument(&aihvus));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(true, static_cast<bool>(parser["thiuth"]));
        EXPECT_EQ(true, static_cast<bool>(parser["raida"]));
        EXPECT_EQ(true, static_cast<bool>(parser["aihvus"]));
        EXPECT_EQ(true, static_cast<bool>(thiuth));
        EXPECT_EQ(true, static_cast<bool>(raida));
        EXPECT_EQ(true, static_cast<bool>(aihvus));
        EXPECT_EQ(1, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0, errors.length());
    }
}
