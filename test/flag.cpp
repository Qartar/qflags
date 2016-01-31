#include <gtest/gtest.h>
#include <qflags/qflags.h>

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that flag correctly reports its capabilities.
 */
TEST(flag_test, values)
{
    auto name = std::string("flag");
    auto flag = qflags::flag(name.c_str());

    EXPECT_EQ(name, flag.name());
    EXPECT_EQ(false, flag.is_set());
    EXPECT_EQ(true, flag.is_flag());
    EXPECT_EQ(true, flag.is_boolean());
    EXPECT_EQ(false, flag.is_integer());
    EXPECT_EQ(false, flag.is_string());

    EXPECT_EQ(false, flag.value_boolean());
    EXPECT_DEATH(flag.value_integer(), "");
    EXPECT_DEATH(flag.value_string(), "");

    EXPECT_EQ(false, static_cast<bool>(flag));
    EXPECT_DEATH(static_cast<int64_t>(flag), "");
    EXPECT_DEATH(static_cast<std::string>(flag), "");
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle a flag using short form arguments.
 */
TEST(flag_test, parse_short_flag)
{
    auto parser = qflags::parser();

    char const* argv[] = { "-f" };
    auto command_line = qflags::command_line(_countof(argv), argv);

    auto flag = qflags::flag("flag", "f");

    {
        std::string errors;

        ASSERT_EQ(true, parser.add_argument(&flag, &errors));
        EXPECT_EQ(0, errors.length());
    }

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
    auto command_line = qflags::command_line(_countof(argv), argv);

    auto flag_m = qflags::flag("flag_m", "m");
    auto flag_i = qflags::flag("flag_i", "i");
    auto flag_a = qflags::flag("flag_a", "a");

    {
        std::string errors;

        ASSERT_EQ(true, parser.add_argument(&flag_m, &errors));
        ASSERT_EQ(true, parser.add_argument(&flag_i, &errors));
        ASSERT_EQ(true, parser.add_argument(&flag_a, &errors));
        EXPECT_EQ(0, errors.length());
    }

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
    auto command_line = qflags::command_line(_countof(argv), argv);

    auto flag_a = qflags::flag("flag_a", "a");
    auto flag_b = qflags::flag("flag_b", "b");
    auto flag_c = qflags::flag("flag_c", "c");
    auto flag_d = qflags::flag("flag_d", "d");
    auto flag_e = qflags::flag("flag_e", "e");
    auto flag_f = qflags::flag("flag_f", "f");

    {
        std::string errors;

        ASSERT_EQ(true, parser.add_argument(&flag_a, &errors));
        ASSERT_EQ(true, parser.add_argument(&flag_b, &errors));
        ASSERT_EQ(true, parser.add_argument(&flag_c, &errors));
        ASSERT_EQ(true, parser.add_argument(&flag_d, &errors));
        ASSERT_EQ(true, parser.add_argument(&flag_e, &errors));
        ASSERT_EQ(true, parser.add_argument(&flag_f, &errors));
        EXPECT_EQ(0, errors.length());
    }

    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(true, static_cast<bool>(parser["flag_a"]));
        EXPECT_EQ(false, static_cast<bool>(parser["flag_b"]));
        EXPECT_EQ(true, static_cast<bool>(parser["flag_c"]));
        EXPECT_EQ(true, static_cast<bool>(parser["flag_d"]));
        EXPECT_EQ(false, static_cast<bool>(parser["flag_e"]));
        EXPECT_EQ(true, static_cast<bool>(parser["flag_f"]));
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
    auto command_line = qflags::command_line(_countof(argv), argv);

    auto flag = qflags::flag("flag", "f");

    {
        std::string errors;

        ASSERT_EQ(true, parser.add_argument(&flag, &errors));
        EXPECT_EQ(0, errors.length());
    }

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
