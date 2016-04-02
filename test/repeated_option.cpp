#include <qflags/qflags.h>
#include "qflags_gtest.h"

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that repeated_option correctly reports its capabilities.
 */
TEST(repeated_option_test, capabilities)
{
    auto name = std::string("foo");
    auto short_name = std::string("f");
    auto option = qflags::repeated_option<qflags::integer_option>(name.c_str(), short_name.c_str());

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
 *
 */
TEST(repeated_option_test, parse_repeated_integer)
{
    auto parser = qflags::parser();

    char const* argv[] = { "--foo", "1", "--foo", "2" };
    auto command_line = qflags::command_line(argv);

    auto option = qflags::repeated_option<qflags::integer_option>("foo");

    ASSERT_EQ(true, parser.add_argument(&option));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        ASSERT_EQ(2u, option.length());
        ASSERT_EQ(1, static_cast<int64_t>(option[0]));
        ASSERT_EQ(1, option[0].value_integer());
        ASSERT_EQ(2, static_cast<int64_t>(option[1]));
        ASSERT_EQ(2, option[1].value_integer());
        EXPECT_EQ(0u, errors.length());
    }
}
