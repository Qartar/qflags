#include <gtest/gtest.h>
#include <qflags/qflags.h>

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that string_option correctly reports its capabilities.
 */
TEST(integer_option_test, capabilities)
{
    auto name = std::string("foo");
    auto short_name = std::string("f");
    auto option = qflags::integer_option(name.c_str(), short_name.c_str());

    EXPECT_EQ(name, option.name());
    EXPECT_EQ(short_name, option.short_name());
    EXPECT_EQ(false, option.is_set());
    EXPECT_EQ(false, option.is_flag());
    EXPECT_EQ(false, option.is_command());
    EXPECT_EQ(false, option.is_boolean());
    EXPECT_EQ(true, option.is_integer());
    EXPECT_EQ(true, option.is_string());

    EXPECT_THROW(option.value_boolean(), std::logic_error);
    EXPECT_EQ(0, option.value_integer());
    EXPECT_EQ("0", option.value_string());

    EXPECT_THROW(static_cast<bool>(option), std::logic_error);
    EXPECT_EQ(0, static_cast<int64_t>(option));
    EXPECT_EQ(0, static_cast<int>(option));
    EXPECT_EQ("0", static_cast<std::string>(option));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that integer_option correctly initializes its default value.
 */
TEST(integer_option_test, default_value)
{
    auto name = std::string("foo");
    auto option = qflags::integer_option(name.c_str(), 123);

    EXPECT_EQ(name, option.name());
    EXPECT_EQ(false, option.is_set());
    EXPECT_EQ(123, option.value_integer());
    EXPECT_EQ("123", option.value_string());
    EXPECT_EQ(123, static_cast<int64_t>(option));
    EXPECT_EQ("123", static_cast<std::string>(option));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly parses an integer value using a variety of formats.
 */
class parse_integer
    : public ::testing::TestWithParam<
    std::tuple<std::string, int64_t>> {};

TEST_P(parse_integer, p)
{
    std::string const& value_str = std::get<0>(GetParam());
    int64_t const value_int = std::get<1>(GetParam());

    auto parser = qflags::parser();

    char const* argv[] = { "--foo", value_str.c_str() };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::integer_option("foo");

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(value_int, static_cast<int64_t>(parser["foo"]));
        EXPECT_EQ(value_str, static_cast<std::string>(parser["foo"]));
        EXPECT_EQ(value_int, static_cast<int64_t>(option));
        EXPECT_EQ(value_str, static_cast<std::string>(option));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());
    }
}

std::tuple<std::string, int64_t> const integer_arguments[] = {
    // Decimal
    std::make_tuple("123", 123),
    std::make_tuple("+123", 123),
    std::make_tuple("-123", -123),
    // Octal
    std::make_tuple("0123", 83),
    std::make_tuple("+0123", 83),
    std::make_tuple("-0123", -83),
    // Hexadecimal (0x prefix)
    std::make_tuple("0xabc", 2748),
    std::make_tuple("+0xabc", 2748),
    std::make_tuple("-0xabc", -2748),
    // Hexadecimal (0X prefix)
    std::make_tuple("0XABC", 2748),
    std::make_tuple("+0XABC", 2748),
    std::make_tuple("-0XABC", -2748),
    // 64-bit Decimal
    std::make_tuple("4294967296", 4294967296ll),
    std::make_tuple("+4294967296", 4294967296ll),
    std::make_tuple("-4294967296", -4294967296ll),
};

INSTANTIATE_TEST_CASE_P(integer_option_test,
                        parse_integer,
                        ::testing::ValuesIn(integer_arguments));

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly detects a missing integer value.
 */
TEST(integer_option_test, parse_integer_no_value)
{
    auto parser = qflags::parser();

    char const* argv[] = { "--foo" };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::integer_option("foo");

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        EXPECT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0u, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly detects an invalid integer value.
 */
class parse_integer_bad_value
    : public ::testing::TestWithParam<std::string> {};

TEST_P(parse_integer_bad_value, p)
{
    std::string const& value_str = GetParam();

    auto parser = qflags::parser();

    char const* argv[] = { "--foo", value_str.c_str() };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::integer_option("foo");

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        ASSERT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0u, errors.length());
    }
}

std::string const integer_bad_arguments[] = {
    "bar",          // Cannot parse string as an integer.
    "-bar",         // Cannot parse string as a negative integer.
    "123bar",       // Invalid decimal characters.
    "123abc",       // Hexadecimal characters without prefix.
    "0xabcxyz",     // Invalid hexadecimal characters.
    "0xxdef",       // Extra 'x' in hexadecimal prefix.
    "--3",          // Double dash indicates an option or flag.
    "123 456",      // Cannot parse multiple integers out of a single argument.
    "0123456789",   // Invalid octal characters. (9)
};

INSTANTIATE_TEST_CASE_P(integer_option_test,
                        parse_integer_bad_value,
                        ::testing::ValuesIn(integer_bad_arguments));
