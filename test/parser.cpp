#include <qflags/qflags.h>
#include "qflags_gtest.h"

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser detects when an argument is added multiple times.
 */
TEST(parser_test, add_multiple)
{
    auto flag = qflags::flag("flag");
    auto parser = qflags::parser();

    // This function is used internally by gtest to perform ASSERT_DEATH and
    // related tests. It is used here to capture stderr and verify that a
    // warning message is generated and also prevents output to stderr from
    // otherwise failing the test suite. This means that ASSERT_DEATH tests
    // cannot be used between the Capture/GetCaptured calls.
    testing::internal::CaptureStderr();

    EXPECT_EQ(true, parser.add_argument(&flag));
    EXPECT_EQ(true, parser.add_argument(&flag));
    EXPECT_NE(0u, testing::internal::GetCapturedStderr().length());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser detects multiple arguments with the same name.
 */
TEST(parser_test, add_duplicate)
{
    auto flag1 = qflags::flag("flag");
    auto flag2 = qflags::flag("flag");
    auto parser = qflags::parser();

    // See note in the parser_test.add_multiple test.
    testing::internal::CaptureStderr();

    EXPECT_EQ(true, parser.add_argument(&flag1));
    EXPECT_EQ(false, parser.add_argument(&flag2));
    EXPECT_NE(0u, testing::internal::GetCapturedStderr().length());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser detects multiple arguments with the same short name.
 */
TEST(parser_test, add_short_duplicate)
{
    auto flag1 = qflags::flag("flag1", "f");
    auto flag2 = qflags::flag("flag2", "f");
    auto parser = qflags::parser();

    // See note in the parser_test.add_multiple test.
    testing::internal::CaptureStderr();

    EXPECT_EQ(true, parser.add_argument(&flag1));
    EXPECT_EQ(false, parser.add_argument(&flag2));
    EXPECT_NE(0u, testing::internal::GetCapturedStderr().length());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser detects arguments with invalid names.
 */
TEST(parser_test, add_invalid_name)
{
    auto equals = qflags::flag("fl=ag");
    auto parser = qflags::parser();

    // See note in the parser_test.add_multiple test.
    testing::internal::CaptureStderr();

    EXPECT_EQ(false, parser.add_argument(&equals));
    EXPECT_NE(0u, testing::internal::GetCapturedStderr().length());
}

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
    EXPECT_EQ(0u, errors.length());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle a command list with no parsed arguments.
 */
TEST(parser_test, parse_no_arguments)
{
    char const* argv[] = { "one", "two", "three" };
    auto command_line = qflags::command_line(argv);

    auto parser = qflags::parser();

    std::string errors;

    EXPECT_EQ(true, parser.parse(command_line, &errors));
    EXPECT_EQ(3, parser.argc());
    EXPECT_EQ(3, parser.remaining_argc());
    EXPECT_EQ(0u, errors.length());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle a command list with non-option arguments.
 */
TEST(parser_test, parse_skipped_arguments)
{
    char const* argv[] = { "one", "--two", "three" };
    auto command_line = qflags::command_line(argv);

    auto parser = qflags::parser();

    std::string errors;

    auto flag = qflags::flag("two");

    ASSERT_EQ(true, parser.add_argument(&flag));

    EXPECT_EQ(true, parser.parse(command_line, &errors));
    EXPECT_EQ(3, parser.argc());
    EXPECT_EQ(2, parser.remaining_argc());
    EXPECT_EQ(0u, errors.length());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly handles terminator arguments.
 */
TEST(parser_test, parse_terminator_argument)
{
    char const* argv[] = { "--foo", "--", "--bar" };
    auto command_line = qflags::command_line(argv);

    auto parser = qflags::parser();

    std::string errors;

    auto foo = qflags::flag("foo");
    auto bar = qflags::flag("bar");

    ASSERT_EQ(true, parser.add_argument(&foo));
    ASSERT_EQ(true, parser.add_argument(&bar));

    EXPECT_EQ(true, parser.parse(command_line, &errors));
    EXPECT_EQ(true, static_cast<bool>(foo));
    EXPECT_EQ(false, static_cast<bool>(bar));
    EXPECT_EQ(3, parser.argc());
    EXPECT_EQ(1, parser.remaining_argc());
    EXPECT_EQ(0u, errors.length());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser correctly warns when a flag is set multiple times.
 */
TEST(parser_test, parse_duplicate_flags)
{
    char const* argv[] = { "-f", "-f" };
    auto command_line = qflags::command_line(argv);

    auto parser = qflags::parser();

    std::string errors;

    auto foo = qflags::flag("foo", "f");

    ASSERT_EQ(true, parser.add_argument(&foo));

    EXPECT_EQ(true, parser.parse(command_line, &errors));
    EXPECT_NE(0u, errors.length());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that `is_set` is set properly after parsing.
 */
TEST(parser_test, parse_is_set)
{
    char const* argv[] = { "-a", "10", "-b", "20" };
    auto command_line = qflags::command_line(argv);

    auto parser = qflags::parser();

    std::string errors;

    auto foo = qflags::integer_option("foo", "a");
    auto bar = qflags::integer_option("bar", "b");

    ASSERT_EQ(true, parser.add_argument(&foo));
    ASSERT_EQ(true, parser.add_argument(&bar));
    ASSERT_EQ(true, parser.parse(command_line,&errors));

    EXPECT_TRUE(foo.is_set());
    EXPECT_EQ(10, static_cast<int>(foo));
    EXPECT_TRUE(bar.is_set());
    EXPECT_EQ(20, static_cast<int>(bar));
}
