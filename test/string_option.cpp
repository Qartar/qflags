#include <gtest/gtest.h>
#include <qflags/qflags.h>

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that string_option correctly reports its capabilities.
 */
TEST(string_option_test, capabilities)
{
    auto name = std::string("foo");
    auto short_name = std::string("f");
    auto option = qflags::string_option(name.c_str(), short_name.c_str(), "");

    EXPECT_EQ(name, option.name());
    EXPECT_EQ(short_name, option.short_name());
    EXPECT_EQ(false, option.is_set());
    EXPECT_EQ(false, option.is_flag());
    EXPECT_EQ(false, option.is_command());
    EXPECT_EQ(false, option.is_boolean());
    EXPECT_EQ(false, option.is_integer());
    EXPECT_EQ(true, option.is_string());

    EXPECT_THROW(option.value_boolean(), std::logic_error);
    EXPECT_THROW(option.value_integer(), std::logic_error);
    EXPECT_EQ("", option.value_string());

    EXPECT_THROW(static_cast<bool>(option), std::logic_error);
    EXPECT_THROW(static_cast<int64_t>(option), std::logic_error);
    EXPECT_THROW(static_cast<int>(option), std::logic_error);
    EXPECT_EQ("", static_cast<std::string>(option));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that string_option correctly initializes its default value.
 */
TEST(string_option_test, default_value)
{
    auto name = std::string("foo");
    auto option = qflags::string_option(name.c_str(), "bar");

    EXPECT_EQ(name, option.name());
    EXPECT_EQ(false, option.is_set());
    EXPECT_EQ("bar", option.value_string());
    EXPECT_EQ("bar", static_cast<std::string>(option));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly ignores a string argument which contains the
 * name of a different argument as a prefix.
 */
TEST(string_option_test, parse_substring)
{
    auto parser = qflags::parser();

    char const* argv[] = { "--foobar", "baz" };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::string_option("foo");

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        ASSERT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0u, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly parses a string value.
 */
TEST(string_option_test, parse_string)
{
    auto parser = qflags::parser();

    char const* argv[] = { "--foo", "bar" };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::string_option("foo");

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ("bar", static_cast<std::string>(parser["foo"]));
        EXPECT_EQ("bar", static_cast<std::string>(option));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly parses a string value using an equality sign.
 */
TEST(string_option_test, parse_string_equals)
{
    auto parser = qflags::parser();

    char const* argv[] = { "--foo=bar" };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::string_option("foo");

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ("bar", static_cast<std::string>(parser["foo"]));
        EXPECT_EQ("bar", static_cast<std::string>(option));
        EXPECT_EQ(1, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly parses a string value using its short name.
 */
TEST(string_option_test, parse_short_string)
{
    auto parser = qflags::parser();

    char const* argv[] = { "-f", "bar" };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::string_option("foo", "f", "");

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ("bar", static_cast<std::string>(parser["foo"]));
        EXPECT_EQ("bar", static_cast<std::string>(option));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly parses a string value using its short name
 * concatenated with its value.
 */
TEST(string_option_test, parse_short_string_concat)
{
    auto parser = qflags::parser();

    char const* argv[] = { "-fbar" };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::string_option("foo", "f", "");

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ("bar", static_cast<std::string>(parser["foo"]));
        EXPECT_EQ("bar", static_cast<std::string>(option));
        EXPECT_EQ(1, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly detects a missing string value.
 */
TEST(string_option_test, parse_string_no_value)
{
    auto parser = qflags::parser();

    char const* argv[] = { "--foo" };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::string_option("foo");

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        EXPECT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0u, errors.length());
    }
}
