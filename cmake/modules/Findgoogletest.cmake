# Findgoogletest

# Set the default path to the googletest submodule
set(GOOGLETEST_PATH "${CMAKE_SOURCE_DIR}/googletest")

# Allow qflags to be built without Google Test
if(EXISTS ${GOOGLETEST_PATH}/CMakeLists.txt)

    # GTest and MinGW don't play nicely when PTHREADS are enabled.
    if(MINGW)
        set(gtest_disable_pthreads ON)
    endif()

    add_subdirectory(${GOOGLETEST_PATH})

    # If the toolchain was set to use Microsoft's Clang/C2 compilers then force
    # gmock and gtest to use the normal C1/C2 compiler because otherwise all of
    # their Microsoft specific configuration would need to be redone.
    if(MSVC AND ${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        foreach(target gtest gtest_main gmock gmock_main)
            set_target_properties(${target} PROPERTIES VS_PLATFORM_TOOLSET_OVERRIDE "v140")
        endforeach()
    endif()
endif()
