#include <qflags/qflags.h>
#include "qflags_gtest.h"

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that command correctly reports its capabilities.
 */
TEST(command_test, capabilities)
{
    auto name = std::string("command");
    auto command = qflags::command(name.c_str());

    EXPECT_EQ(name, command.name());
    EXPECT_EQ(false, command.is_set());
    EXPECT_EQ(false, command.is_flag());
    EXPECT_EQ(true, command.is_command());
    EXPECT_EQ(false, command.is_array());
    EXPECT_EQ(true, command.is_boolean());
    EXPECT_EQ(false, command.is_integer());
    EXPECT_EQ(false, command.is_string());
    EXPECT_EQ(0u, command.array_size());

    EXPECT_EQ(false, command.value_boolean());
    EXPECT_THROW(command.value_integer(), std::logic_error);
    EXPECT_THROW(command.value_string(), std::logic_error);
    EXPECT_THROW(command.value_array(0), std::logic_error);

    EXPECT_EQ(false, static_cast<bool>(command));
    EXPECT_THROW(static_cast<int64_t>(command), std::logic_error);
    EXPECT_THROW(static_cast<int>(command), std::logic_error);
    EXPECT_THROW(static_cast<std::string>(command), std::logic_error);

    EXPECT_EQ(0, command.argc());
    EXPECT_NE(nullptr, command.argv());
    EXPECT_EQ(nullptr, command.argv(0));

    EXPECT_EQ(0, command.remaining_argc());
    EXPECT_NE(nullptr, command.remaining_argv());
    EXPECT_EQ(nullptr, command.remaining_argv(0));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that command correctly initializes its default value.
 */
TEST(command_test, default_value)
{
    auto name = std::string("foo");
    auto command = qflags::command(name.c_str());

    EXPECT_EQ(name, command.name());
    EXPECT_EQ(false, command.is_set());
    EXPECT_EQ(false, command.value_boolean());
    EXPECT_EQ(false, static_cast<bool>(command));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle a command argument.
 */
TEST(command_test, parse_command)
{
    auto parser = qflags::parser();

    char const* argv[] = { "foo" };
    auto command_line = qflags::command_line(argv);

    auto command = qflags::command("foo");

    ASSERT_EQ(true, parser.add_argument(&command));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(true, static_cast<bool>(parser["foo"]));
        EXPECT_EQ(true, static_cast<bool>(command));
        EXPECT_EQ(1, parser.argc());
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that command parsing failures are correctly propogated to the parser.
 */
TEST(command_test, parse_bad_command)
{
    auto parser = qflags::parser();

    char const* argv[] = { "foo", "--bar" };
    auto command_line = qflags::command_line(argv);

    auto command = qflags::command("foo");

    ASSERT_EQ(true, parser.add_argument(&command));
    {
        std::string errors;

        ASSERT_EQ(false, parser.parse(command_line, &errors));
        EXPECT_NE(0u, errors.length());
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle a command argument with subarguments.
 */
TEST(command_test, parse_command_arguments)
{
    auto parser = qflags::parser();

    char const* argv[] = { "foo", "--bar" };
    auto command_line = qflags::command_line(argv);

    auto command = qflags::command("foo");

    ASSERT_EQ(true, parser.add_argument(&command));

    auto flag = qflags::flag("bar");

    ASSERT_EQ(true, command.add_argument(&flag));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(true, static_cast<bool>(parser["foo"]));
        EXPECT_EQ(true, static_cast<bool>(command));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(std::string("foo"), parser.argv(0));
        EXPECT_EQ(std::string("--bar"), parser.argv(1));
        EXPECT_EQ(0, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());

        EXPECT_EQ(true, static_cast<bool>(command["bar"]));
        EXPECT_EQ(true, static_cast<bool>(flag));
        EXPECT_EQ(2, command.argc());
        EXPECT_EQ(std::string("foo"), command.argv(0));
        EXPECT_EQ(std::string("--bar"), command.argv(1));
        EXPECT_EQ(1, command.remaining_argc());
        EXPECT_EQ(std::string("foo"), command.remaining_argv(0));
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Test that the parser can handle a command argument following unbound arguments.
 */
TEST(command_test, parse_command_unbound_arguments)
{
    auto parser = qflags::parser();

    char const* argv[] = { "foo", "bar" };
    auto command_line = qflags::command_line(argv);

    auto command = qflags::command("bar");

    ASSERT_EQ(true, parser.add_argument(&command));
    {
        std::string errors;

        ASSERT_EQ(true, parser.parse(command_line, &errors));
        EXPECT_EQ(true, static_cast<bool>(parser["bar"]));
        EXPECT_EQ(true, static_cast<bool>(command));
        EXPECT_EQ(2, parser.argc());
        EXPECT_EQ(std::string("foo"), parser.argv(0));
        EXPECT_EQ(std::string("bar"), parser.argv(1));
        EXPECT_EQ(1, parser.remaining_argc());
        EXPECT_EQ(0u, errors.length());

        EXPECT_EQ(1, command.argc());
        EXPECT_EQ(std::string("bar"), command.argv(0));
        EXPECT_EQ(1, command.remaining_argc());
        EXPECT_EQ(std::string("bar"), command.remaining_argv(0));
    }
}
