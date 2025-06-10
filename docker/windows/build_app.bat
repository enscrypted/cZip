@echo off
setlocal

:: This script runs inside the container to build the application.

:: Set environment variables for the build
set "QT_DIR_CONTAINER=C:\Qt\5.15.2\msvc2019_64"
set "VCVARS_PATH_CONTAINER=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

echo "--- Setting up Visual Studio build environment..."
call "%VCVARS_PATH_CONTAINER%"
if %errorlevel% neq 0 (
    echo "ERROR: Failed to call vcvars64.bat"
    exit /b 1
)

echo "--- Finding Windows 10 SDK..."
for /f "delims=" %%i in ('dir /b /ad /o-n "C:\Program Files (x86)\Windows Kits\10\Include"') do (
    set SDK_VERSION=%%i
    goto :found_sdk
)

:found_sdk
if not defined SDK_VERSION (
    echo ERROR: Could not find Windows 10 SDK version directory.
    exit /b 1
)
echo "--- Found Windows SDK Version: %SDK_VERSION%"
set SDK_INCLUDE_PATH=C:\Program Files (x86)\Windows Kits\10\Include\%SDK_VERSION%
set SDK_LIB_PATH=C:\Program Files (x86)\Windows Kits\10\Lib\%SDK_VERSION%
set SDK_BIN_PATH=C:\Program Files (x86)\Windows Kits\10\bin\%SDK_VERSION%\x64
echo "--- Adding SDK paths to environment..."
set INCLUDE=%SDK_INCLUDE_PATH%\ucrt;%SDK_INCLUDE_PATH%\shared;%SDK_INCLUDE_PATH%\um;%INCLUDE%
set LIB=%SDK_LIB_PATH%\ucrt\x64;%SDK_LIB_PATH%\um\x64;%LIB%
set PATH=%SDK_BIN_PATH%;%PATH%

echo "--- Changing to build directory..."
cd build
if %errorlevel% neq 0 (
    echo "ERROR: Failed to change to C:\src\build directory."
    exit /b 1
)

echo "--- Running qmake..."
"%QT_DIR_CONTAINER%\bin\qmake.exe" ..\czip.pro -spec win32-msvc "QT_INSTALL_PATH=%QT_DIR_CONTAINER%"
if %errorlevel% neq 0 (
    echo "ERROR: qmake failed."
    exit /b 1
)

echo "--- Running nmake..."
nmake
if %errorlevel% neq 0 (
    echo "ERROR: nmake failed."
    exit /b 1
)

echo "--- Application build successful."
exit /b 0