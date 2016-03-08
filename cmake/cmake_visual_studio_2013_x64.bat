@echo off

cmake -G "Visual Studio 12 2013 Win64"  %"Set the generator to Visual Studio 2013 x64"% ^
      -H%~dp0\..                        %"Set the source directory to the parent directory"% ^
      -B%~dp0\..\_build                 %"Set the build directory to the _build subdirectory"% ^
      %*                                %"Forward additional command line arguments to CMake"%

@echo on
