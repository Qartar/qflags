cmake_minimum_required(VERSION 2.8.11)

project(qflags)

################################################################################

set(GOOGLETEST_PATH ${CMAKE_SOURCE_DIR}/googletest)
if(IS_DIRECTORY ${GOOGLETEST_PATH})
    add_subdirectory(${GOOGLETEST_PATH})
endif()

################################################################################

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    # /W4   Warning level 4
    # /WX   Warnings as errors
    # /MP   Multi-process compilation
    # /sdl  Additional security checks
    set(compiler_flags "/W4 /WX /MP /sdl")

    # For MSVC, CMake sets certain flags to defaults we want to override.
    # This replacement code is taken from sample in the CMake Wiki at
    # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
    foreach (flag_var
             CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
             CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)

        # Set base compiler flags
        set(${flag_var} "${${flag_var}} ${compiler_flags}")

        # Disable dynamic linking to C++ runtime libraries
        string(REPLACE "/MD" "-MT" ${flag_var} "${${flag_var}}")

        # Disable C++ exceptions
        string(REGEX REPLACE "/EH[a-z]+" "" ${flag_var} "${${flag_var}}")

        # Disable RTTI
        string(REPLACE "/GR" "/GR-" ${flag_var} "${${flag_var}}")

    endforeach()

    # Disable C++ exceptions
    add_definitions(-D_HAS_EXCEPTIONS=0)

    # Enable Unicode
    add_definitions(-D_UNICODE -DUNICODE)
endif()

set(INCLUDES
    include/qflags/qflags.h
)

set(SOURCES
    src/argument.cpp
    src/flag.cpp
    src/string_option.cpp
    src/parser.cpp
    src/command_line.cpp
)

add_library(qflags STATIC
    ${INCLUDES}
    ${SOURCES}
)

target_include_directories(qflags
    PUBLIC
        include
)

################################################################################

if(TARGET gtest)
    add_executable(qflags_test
                   test/command_line.cpp
                   test/flag.cpp
                   test/string_option.cpp
                   test/parser.cpp)

    target_link_libraries(qflags_test qflags gtest gtest_main)

    add_custom_command(TARGET qflags_test
                       POST_BUILD
                       COMMAND qflags_test)
endif()
