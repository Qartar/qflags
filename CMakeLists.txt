cmake_minimum_required(VERSION 2.8.11)

project(qflags)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

# CMake by default links targets to the shared runtime library but this is
# overridden for qflags unless this option is enabled. This may be necessary
# if qflags is being linked to other targets which use the shared runtime.
option(qflags_use_shared_crt
       "Use shared rather than static runtime library."
       OFF
)

################################################################################

find_package(googletest)

################################################################################

if(MSVC)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        # /W4       Warning level 4
        # /WX       Warnings as errors
        # /MP       Multi-process compilation
        # /sdl      Additional security checks
        set(compiler_flags "/W4 /WX /MP /sdl")

    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        # -Wall     Enable (almost) all warnings
        # -Wextra   Enable additional warnings
        # -Werror   Warnings as errors
        # /MP       Multi-process compilation
        # /sdl      Additional security checks
        set(compiler_flags "-Wall -Wextra -Werror -Woverloaded-virtual -Wtype-limits /MP /sdl")

        # Set gmock and gtest as system headers in order to suppress warnings.
        set(compiler_flags "${compiler_flags} --system-header-prefix=gtest/")
        set(compiler_flags "${compiler_flags} --system-header-prefix=gmock/")
    endif()

    # For MSVC, CMake sets certain flags to defaults we want to override.
    # This replacement code is taken from sample in the CMake Wiki at
    # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
    foreach (flag_var
             CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
             CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)

        # Set base compiler flags
        set(${flag_var} "${${flag_var}} ${compiler_flags}")

        # Disable dynamic linking to C++ runtime libraries
        if(NOT qflags_use_shared_crt)
            string(REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()

        # Disable RTTI
        string(REPLACE "/GR" "/GR-" ${flag_var} "${${flag_var}}")

    endforeach()

    # Enable Unicode
    add_definitions(-D_UNICODE -DUNICODE)

elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU OR ${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)

    # -Wall         Enable (almost) all warnings
    # -Wextra       Enable additional warnings
    # -Werror       Warnings as errors
    # -std=c++11    Use C++11 language features
    set(compiler_flags "-Wall -Wextra -Werror -Woverloaded-virtual -Wtype-limits -Wunused-parameter -std=c++11")

    foreach (flag_var
             CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
             CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)

        # Set base compiler flags
        set(${flag_var} "${${flag_var}} ${compiler_flags}")
    endforeach()

endif()

################################################################################

set(INCLUDES
    include/qflags/qflags.h

    include/qflags/argument.hpp
    include/qflags/flag.hpp
    include/qflags/string_option.hpp
    include/qflags/boolean_option.hpp
    include/qflags/integer_option.hpp
    include/qflags/choice_option.hpp
    include/qflags/range_option.hpp
    include/qflags/parser.hpp
    include/qflags/command.hpp
    include/qflags/command_line.hpp
)

set(SOURCES
    src/qflags-all.cpp
)

add_library(qflags-static STATIC
    ${INCLUDES}
    ${SOURCES}
)

target_include_directories(qflags-static
    PUBLIC
        include
)

target_compile_definitions(qflags-static
    PUBLIC
        -DQFLAGS_STATIC
)

################################################################################

add_library(qflags INTERFACE)

target_include_directories(qflags
    INTERFACE
        include
)

################################################################################

if(TARGET gtest)
    set(TEST_SOURCES
        test/command_line.cpp
        test/flag.cpp
        test/string_option.cpp
        test/boolean_option.cpp
        test/integer_option.cpp
        test/choice_option.cpp
        test/range_option.cpp
        test/parser.cpp
        test/command.cpp
    )

    add_executable(qflags_test ${TEST_SOURCES})

    target_link_libraries(qflags_test qflags gtest gtest_main)

    add_executable(qflags-static_test ${TEST_SOURCES})

    target_link_libraries(qflags-static_test qflags-static gtest gtest_main)

    # Set some Clang specific compiler options
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        set(compile_options -Wno-unused-value
                            -fstack-protector-all
        )
        # Some options do not work with Clang/C2
        if(NOT MSVC)
            set(compile_options ${compile_options}
                                -fsanitize-undefined-trap-on-error
            )
        endif()
    # Set some GCC specific compiler options
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        set(compile_options -Wno-conversion-null
                            -fsanitize=undefined
                            -fsanitize-undefined-trap-on-error
        )
        # Some options do not work with MinGW
        if(NOT MINGW)
            set(compile_options ${compile_options}
                                -fstack-protector-all
            )
        endif()
    endif()

    foreach(target
            qflags_test qflags-static_test)

        add_custom_command(TARGET ${target}
                           POST_BUILD
                           COMMAND ${target})

        target_compile_options(${target}
            PUBLIC
                ${compile_options}
        )
    endforeach()

endif()

################################################################################

add_executable(levenshtein sample/levenshtein.cpp)

target_link_libraries(levenshtein qflags-static)
