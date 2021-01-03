echo off

call set_qt_env_var.cmd

SET QT_CREATOR_DIR=D:\Qt\qtcreator-4.4.1\bin
SET PATH=%PATH%;%QT_CREATOR_DIR%
start %QT_CREATOR_DIR%\qtcreator.exe %~dp0\CMakeLists.txt