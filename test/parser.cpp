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
    EXPECT_EQ(0, errors.length());
}
