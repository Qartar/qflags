#include <gtest/gtest.h>
#include <qflags/qflags.h>

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle an empty command list.
 */
TEST(parser_test, parse_empty)
{
    char const* argv[] = { "" };
    auto command_line = qflags::command_line(0, argv);

    auto parser = qflags::parser();

    std::string errors;

    EXPECT_EQ(true, parser.parse(command_line, &errors));
    EXPECT_EQ(0, parser.argc());
    EXPECT_EQ(0, parser.remaining_argc());
    EXPECT_EQ(0, errors.length());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle a command list with no parsed arguments.
 */
TEST(parser_test, parse_no_arguments)
{
    char const* argv[] = { "one", "two", "three" };
    auto command_line = qflags::command_line(_countof(argv), argv);

    auto parser = qflags::parser();

    std::string errors;

    EXPECT_EQ(true, parser.parse(command_line, &errors));
    EXPECT_EQ(3, parser.argc());
    EXPECT_EQ(3, parser.remaining_argc());
    EXPECT_EQ(0, errors.length());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle a command list with non-option arguments.
 */
TEST(parser_test, parse_skipped_arguments)
{
    char const* argv[] = { "one", "--two", "three" };
    auto command_line = qflags::command_line(_countof(argv), argv);

    auto parser = qflags::parser();

    std::string errors;

    auto flag = qflags::flag("two");

    ASSERT_EQ(true, parser.add_argument(&flag, &errors));
    ASSERT_EQ(0, errors.length());

    EXPECT_EQ(true, parser.parse(command_line, &errors));
    EXPECT_EQ(3, parser.argc());
    EXPECT_EQ(2, parser.remaining_argc());
    EXPECT_EQ(0, errors.length());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly handles terminator arguments.
 */
TEST(parser_test, parse_terminator_argument)
{
    char const* argv[] = { "--foo", "--", "--bar" };
    auto command_line = qflags::command_line(_countof(argv), argv);

    auto parser = qflags::parser();

    std::string errors;

    auto foo = qflags::flag("foo");
    auto bar = qflags::flag("bar");

    ASSERT_EQ(true, parser.add_argument(&foo, &errors));
    ASSERT_EQ(true, parser.add_argument(&bar, &errors));
    ASSERT_EQ(0, errors.length());

    EXPECT_EQ(true, parser.parse(command_line, &errors));
    EXPECT_EQ(true, static_cast<bool>(foo));
    EXPECT_EQ(false, static_cast<bool>(bar));
    EXPECT_EQ(3, parser.argc());
    EXPECT_EQ(1, parser.remaining_argc());
    EXPECT_EQ(0, errors.length());
}
