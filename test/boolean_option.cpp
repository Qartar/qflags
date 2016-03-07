#include <gtest/gtest.h>
#include <qflags/qflags.h>

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that boolean_option correctly reports its capabilities.
 */
TEST(boolean_option_test, capabilities)
{
    auto name = std::string("foo");
    auto short_name = std::string("f");
    auto option = qflags::boolean_option(name.c_str(), short_name.c_str());

    EXPECT_EQ(name, option.name());
    EXPECT_EQ(short_name, option.short_name());
    EXPECT_EQ(false, option.is_set());
    EXPECT_EQ(false, option.is_flag());
    EXPECT_EQ(false, option.is_command());
    EXPECT_EQ(true, option.is_boolean());
    EXPECT_EQ(false, option.is_integer());
    EXPECT_EQ(true, option.is_string());

    EXPECT_EQ(false, option.value_boolean());
    EXPECT_THROW(option.value_integer(), std::logic_error);
    EXPECT_EQ("false", option.value_string());

    EXPECT_EQ(false, static_cast<bool>(option));
    EXPECT_THROW(static_cast<int64_t>(option), std::logic_error);
    EXPECT_THROW(static_cast<int>(option), std::logic_error);
    EXPECT_EQ("false", static_cast<std::string>(option));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that boolean_option correctly initializes its default value.
 */
TEST(boolean_option_test, default_value)
{
    auto name = std::string("foo");
    auto option = qflags::boolean_option(name.c_str(), true);

    EXPECT_EQ(name, option.name());
    EXPECT_EQ(false, option.is_set());
    EXPECT_EQ(true, option.value_boolean());
    EXPECT_EQ("true", option.value_string());
    EXPECT_EQ(true, static_cast<bool>(option));
    EXPECT_EQ("true", static_cast<std::string>(option));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly parses an boolean value using valid arguments.
 */
class parse_boolean
    : public ::testing::TestWithParam<
    std::tuple<bool, std::string>> {};

TEST_P(parse_boolean, p)
{
    bool const value_bool = std::get<0>(GetParam());
    std::string const& value_str = std::get<1>(GetParam());

    auto parser = qflags::parser();

    char const* argv[] = { "--foo", value_str.c_str() };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::boolean_option("foo");

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(value_bool, static_cast<bool>(parser["foo"]));
        EXPECT_EQ(value_str, static_cast<std::string>(parser["foo"]));
        EXPECT_EQ(value_bool, static_cast<bool>(option));
        EXPECT_EQ(value_str, static_cast<std::string>(option));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());
    }
}

std::tuple<bool, std::string> const boolean_arguments[] = {
    // true values
    std::make_tuple(true, "true"),
    std::make_tuple(true, "True"),
    std::make_tuple(true, "TRUE"),
    std::make_tuple(true, "1"),
    // false values
    std::make_tuple(false, "false"),
    std::make_tuple(false, "False"),
    std::make_tuple(false, "FALSE"),
    std::make_tuple(false, "0"),
};

INSTANTIATE_TEST_CASE_P(boolean_option_test,
                        parse_boolean,
                        ::testing::ValuesIn(boolean_arguments));

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly detects a missing boolean value.
 */
TEST(boolean_option_test, parse_boolean_no_value)
{
    auto parser = qflags::parser();

    char const* argv[] = { "--foo" };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::boolean_option("foo");

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        EXPECT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0u, errors.length());
    }
}
