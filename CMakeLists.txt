cmake_minimum_required(VERSION 2.8.11)

project(qflags)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    # /W4   Warning level 4
    # /WX   Warnings as errors
    # /MP   Multi-process compilation
    # /sdl  Additional security checks
    set(compiler_flags "/W4 /WX /MP /sdl")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compiler_flags}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${compiler_flags}")

    # Disable C++ exceptions
    string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

    # Disable RTTI
    string(REGEX REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

    # Enable Unicode
    add_definitions(-D_UNICODE -DUNICODE)
endif()

set(INCLUDES
    include/qflags/qflags.h
)

set(SOURCES
    src/qflags.cpp
)

add_library(qflags STATIC
    ${INCLUDES}
    ${SOURCES}
)

target_include_directories(qflags
    PUBLIC
        include
)
