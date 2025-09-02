# PowerShell build script for quickView
# Requires Visual Studio or Visual Studio Build Tools

Write-Host "Building quickView for Windows..." -ForegroundColor Green

# Create build directory
if (!(Test-Path "build_windows")) {
    New-Item -ItemType Directory -Path "build_windows"
}
Set-Location "build_windows"

# Configure with CMake
Write-Host "Configuring with CMake..." -ForegroundColor Yellow
$configResult = & cmake -G "Visual Studio 16 2019" -A x64 ..
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Build the project
Write-Host "Building project..." -ForegroundColor Yellow
$buildResult = & cmake --build . --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "Build successful!" -ForegroundColor Green
Write-Host "Executable location: build_windows\Release\quickView.exe" -ForegroundColor Cyan
Write-Host ""
Write-Host "To run: cd build_windows\Release && .\quickView.exe" -ForegroundColor Yellow
Read-Host "Press Enter to exit"
