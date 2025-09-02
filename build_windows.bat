@echo off
REM Windows build script for quickView
REM Requires Visual Studio or Visual Studio Build Tools

echo Building quickView for Windows...

REM Create build directory
if not exist "build_windows" mkdir build_windows
cd build_windows

REM Configure with CMake for Visual Studio
echo Configuring with CMake...
cmake -G "Visual Studio 16 2019" -A x64 ..
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build the project
echo Building project...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build successful! 
echo Executable location: build_windows\Release\quickView.exe
echo.
echo To run: cd build_windows\Release && quickView.exe
pause
