#include <gtest/gtest.h>
#include <qflags/qflags.h>

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that choice_option correctly reports its capabilities.
 */
TEST(choice_option_test, capabilities)
{
    auto name = std::string("foo");
    auto option = qflags::choice_option(name.c_str(), {"bar", "baz"}, "bar");

    EXPECT_EQ(name, option.name());
    EXPECT_EQ(false, option.is_set());
    EXPECT_EQ(false, option.is_flag());
    EXPECT_EQ(false, option.is_boolean());
    EXPECT_EQ(false, option.is_integer());
    EXPECT_EQ(true, option.is_string());

    EXPECT_DEATH(option.value_boolean(), "");
    EXPECT_DEATH(option.value_integer(), "");
    EXPECT_EQ("bar", option.value_string());

    EXPECT_DEATH(static_cast<bool>(option), "");
    EXPECT_DEATH(static_cast<int64_t>(option), "");
    EXPECT_EQ("bar", static_cast<std::string>(option));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that choice_option correctly validates its contruction parameters.
 */
TEST(choice_option_test, parameters)
{
    EXPECT_DEBUG_DEATH(qflags::choice_option("foo", { "bar" }, "baz"), "")
        << "Failed to assert when default value is not a choice value.";

    EXPECT_DEBUG_DEATH(qflags::choice_option("foo", { "bar", "bar" }, "bar"), "")
        << "Failed to assert when choice values are not distinct.";
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly parses a choice value.
 */
TEST(choice_option_test, parse_choice)
{
    auto parser = qflags::parser();
    auto option = qflags::choice_option("foo", {"bar", "baz"}, "bar");

    {
        std::string errors;

        ASSERT_EQ(true, parser.add_argument(&option, &errors));
        EXPECT_EQ(0, errors.length());
    }

    {
        char const* argv[] = { "--foo", "bar" };
        auto command_line = qflags::command_line(_countof(argv), argv);

        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ("bar", static_cast<std::string>(parser["foo"]));
        EXPECT_EQ("bar", static_cast<std::string>(option));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0, errors.length());
    }

    {
        char const* argv[] = { "--foo", "baz" };
        auto command_line = qflags::command_line(_countof(argv), argv);

        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ("baz", static_cast<std::string>(parser["foo"]));
        EXPECT_EQ("baz", static_cast<std::string>(option));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly detects a missing choice value.
 */
TEST(choice_option_test, parse_choice_no_value)
{
    auto parser = qflags::parser();

    char const* argv[] = { "--foo" };
    auto command_line = qflags::command_line(_countof(argv), argv);

    auto option = qflags::choice_option("foo", { "bar" }, "bar");

    {
        std::string errors;

        ASSERT_EQ(true, parser.add_argument(&option, &errors));
        EXPECT_EQ(0, errors.length());
    }

    {
        std::string errors;

        EXPECT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly requires that choice_option is set to one of
 * its value choices.
 */
TEST(choice_option_test, parse_invalid_choice)
{
    auto parser = qflags::parser();

    char const* argv[] = { "--foo", "baz" };
    auto command_line = qflags::command_line(_countof(argv), argv);

    auto option = qflags::choice_option("foo", { "bar" }, "bar");

    {
        std::string errors;

        ASSERT_EQ(true, parser.add_argument(&option, &errors));
        EXPECT_EQ(0, errors.length());
    }

    {
        std::string errors;

        ASSERT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0, errors.length());
    }
}
