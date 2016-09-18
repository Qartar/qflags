@echo off

cmake -G "Visual Studio 14 2015 Win64"  %"Set the generator to Visual Studio 2015 x64"% ^
      -T "v140_clang_c2"                %"Set the compiler toolchain to Clang/C2"% ^
      -H%~dp0\..                        %"Set the source directory to the parent directory"% ^
      -B%~dp0\..\_build_clang           %"Set the build directory to the _build_clang subdirectory"% ^
      %*                                %"Forward additional command line arguments to CMake"%

@echo on
