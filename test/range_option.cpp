#include <qflags/qflags.h>
#include "qflags_gtest.h"

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that range_option correctly reports its capabilities.
 */
TEST(range_option_test, capabilities)
{
    auto name = std::string("foo");
    auto short_name = std::string("f");
    auto option = qflags::range_option(name.c_str(), short_name.c_str(), -1, 1, 0);

    EXPECT_EQ(name, option.name());
    EXPECT_EQ(short_name, option.short_name());
    EXPECT_EQ(false, option.is_set());
    EXPECT_EQ(false, option.is_flag());
    EXPECT_EQ(false, option.is_command());
    EXPECT_EQ(false, option.is_array());
    EXPECT_EQ(false, option.is_boolean());
    EXPECT_EQ(true, option.is_integer());
    EXPECT_EQ(true, option.is_string());
    EXPECT_EQ(0u, option.array_size());

    EXPECT_THROW(option.value_boolean(), std::logic_error);
    EXPECT_EQ(0, option.value_integer());
    EXPECT_EQ("0", option.value_string());
    EXPECT_THROW(option.value_array(0), std::logic_error);

    EXPECT_THROW(static_cast<bool>(option), std::logic_error);
    EXPECT_EQ(0, static_cast<int64_t>(option));
    EXPECT_EQ(0, static_cast<int>(option));
    EXPECT_EQ("0", static_cast<std::string>(option));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that range_option correctly validates its contruction parameters.
 */
TEST(range_option_test, parameters)
{
    EXPECT_DEBUG_DEATH(qflags::range_option("foo", {}, 0), "")
        << "Failed to assert when no range values were provided.";

    EXPECT_DEBUG_DEATH(qflags::range_option("foo", { 0, 1 }, 2), "")
        << "Failed to assert when default value is not a range value.";

    EXPECT_DEBUG_DEATH(qflags::range_option("foo", 0, 1, 2), "")
        << "Failed to assert when range values are not distinct.";
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly parses an enumerated range value.
 */
TEST(range_option_test, parse_range_enumerated)
{
    auto parser = qflags::parser();
    auto option = qflags::range_option("foo", {0, -1, 2}, 0);

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        char const* argv[] = { "--foo", "-1" };
        auto command_line = qflags::command_line(argv);

        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(-1, static_cast<int64_t>(parser["foo"]));
        EXPECT_EQ(-1, static_cast<int64_t>(option));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());
    }

    {
        char const* argv[] = { "--foo", "2" };
        auto command_line = qflags::command_line(argv);

        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(2, static_cast<int64_t>(parser["foo"]));
        EXPECT_EQ(2, static_cast<int64_t>(option));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly parses a bounded range value.
 */
TEST(range_option_test, parse_range_bounded)
{
    auto parser = qflags::parser();
    auto option = qflags::range_option("foo", -1, 2, 0);

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        char const* argv[] = { "--foo", "-1" };
        auto command_line = qflags::command_line(argv);

        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(-1, static_cast<int64_t>(parser["foo"]));
        EXPECT_EQ(-1, static_cast<int64_t>(option));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());
    }

    {
        char const* argv[] = { "--foo", "2" };
        auto command_line = qflags::command_line(argv);

        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(2, static_cast<int64_t>(parser["foo"]));
        EXPECT_EQ(2, static_cast<int64_t>(option));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly detects a missing range value.
 */
TEST(range_option_test, parse_range_no_value)
{
    auto parser = qflags::parser();

    char const* argv[] = { "--foo" };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::range_option("foo", { 0 }, 0);

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        EXPECT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0u, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly requires that range_option is set to one of
 * its value ranges.
 */
TEST(range_option_test, parse_invalid_range_enumerated)
{
    auto parser = qflags::parser();

    char const* argv[] = { "--foo", "1" };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::range_option("foo", { 0 }, 0);

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        ASSERT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0u, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly requires that range_option is set to a value
 * within its minimum and maximum values.
 */
TEST(range_option_test, parse_invalid_range_bounded)
{
    auto parser = qflags::parser();

    auto option = qflags::range_option("foo", -1, 1, 0);

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        char const* argv[] = { "--foo", "-2" };
        auto command_line = qflags::command_line(argv);

        std::string errors;

        ASSERT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0u, errors.length());
    }

    {
        char const* argv[] = { "--foo", "2" };
        auto command_line = qflags::command_line(argv);

        std::string errors;

        ASSERT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0u, errors.length());
    }
}
