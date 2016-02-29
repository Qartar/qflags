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
        # -Wall     Enable all warnings
        # -Werror   Warnings as errors
        # /MP       Multi-process compilation
        # /sdl      Additional security checks
        set(compiler_flags "-Wall -Werror /MP /sdl")

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

    foreach(target
            qflags_test qflags-static_test)

        add_custom_command(TARGET ${target}
                           POST_BUILD
                           COMMAND ${target})

        # Disable unused value warnings when using Clang
        if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
            target_compile_options(${target}
                PUBLIC
                    -Wno-unused-value
            )
        endif()
    endforeach()

endif()

################################################################################

add_executable(levenshtein sample/levenshtein.cpp)

target_link_libraries(levenshtein qflags-static)
