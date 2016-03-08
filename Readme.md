# qflags

Simple C++ command-line parsing library

## Overview

qflags uses command-line syntax similar but not equivalent to `getopt` and
 `getopt_long`. qflags can be used as a header-only library or compiled into
a static library. [Google Test](https://github.com/google/googletest) is included
as a submodule and will be used to compile and execute gtest binaries if available.

## Getting Started
### Requirements

The following build systems are supported via CMake:

- Visual Studio 2013
- Visual Studio 2015
- Visual Studio 2015 Clang/C2[*](https://github.com/Microsoft/CMake/releases/tag/clang-rel-2 "Requires Microsoft's fork of CMake for Clang/C2 toolchain support")
- GCC (MinGW, Unix)

Other build systems are untested and YMMV.

### Usage

There are three 'framework' classes:

| class name     | description
| ----------     | -----------
| `command_line` | Normalizes a command-line from `argc, argv` or `args` parameters.
| `argument`     | Base object representing an argument and its value in the command-line.
| `parser`       | Holds references to `argument` objects and populates their data using a `command_line`.

A trivial program using qflags:
```
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
```

### Syntax
TODO :(

### Argument Types

| class name       | value type    | description 
| ----------       | ----------    | -----------
| `flag`           | `bool`        | `true` if flag was specified on the command line, otherwise `false`. Does not accept an additional value argument.
| `command`        | `bool`        | `true` if command was specified on the command line, otherwise `false`. Accepts and parses remaining sub-arguments.
| `string_option`  | `std::string` | Accepts a value argument as a string.
| `boolean_option` | `bool`        | Accepts a value argument interpreted as a boolean. An error is emitted if the value argument does not match one of the prescribed string values.<sup>[1](#footnote-1)</sup>
| `integer_option` | `int64_t`     | Accepts a value argument interpreted as an integer. An error is emitted if the value argument cannot be parsed as an integer. 
| `choice_option`  | `std::string` | Accepts a value argument as a string if it exists in a provided set of permissible values.
| `range_option`   | `int64_t`     | Accepts a value argument as an integer if it exists in a provided set or range of permissible values.

<a name="footnote-1">1</a>: The string values "true", "True", "TRUE", and "1" evaluate to `true`. The string values "false", "False", "FALSE", and "0" evaluate to `false`. Any other string value will emit a parsing error. 
