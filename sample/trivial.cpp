#include <qflags/qflags.h>
#include <cstdio>

int main(int argc, char* argv[])
{
    // Normalize command-line.
    auto command_line = qflags::command_line(argc, argv);
    // Create parser.
    auto parser = qflags::parser();
    
    // Define arguments.
    auto foo = qflags::flag("foo");
    auto bar = qflags::string_option("bar");
    
    // Add arguments to parser.
    parser.add_argument(&foo);
    parser.add_argument(&bar);
    
    // Parse.
    std::string errors;
    if (parser.parse(command_line, &errors)) {
        fprintf(stdout, "'foo' is '%s'\n", foo.value_boolean() ? "true" : "false");
        fprintf(stdout, "'bar' is '%s'\n", bar.value_string().c_str());
    } else {
        fprintf(stdout, "%s", errors.c_str());
    }
    return 0;
}
