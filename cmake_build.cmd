echo off

call set_qt_env_var.cmd

SET BUILD_DIR=".\build_root"

SET SRC_DIR="%~dp0"
SET PATH=%QT_DIR%;%PATH%

mkdir %BUILD_DIR%
cd %BUILD_DIR%

rem host x86 arch x86
rem cmake -G "Visual Studio 15" %SRC_DIR% 

rem host x64 arch x86
rem cmake -T host=x64 -G "Visual Studio 15" %SRC_DIR% 

rem host x64 arch x64
cmake -T host=x64 -G "Visual Studio 15 Win64" %SRC_DIR% 

rem cmake --build . --config Debug 
cmake --build . --config Release