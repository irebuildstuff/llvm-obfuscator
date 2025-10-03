@echo off
REM ========================================
REM LLVM Code Obfuscator v4.0
REM Debug Build & Run Script
REM ========================================

echo.
echo ====================================
echo   LLVM Code Obfuscator v4.0
echo   Debug Build & Run
echo ====================================
echo.

REM Check if .NET 8 SDK is installed
dotnet --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: .NET 8 SDK is not installed!
    echo Please install .NET 8 SDK from: https://dotnet.microsoft.com/download
    pause
    exit /b 1
)

echo [1/4] Checking .NET SDK version...
dotnet --version
echo.

echo [2/4] Restoring NuGet packages...
dotnet restore
if %errorlevel% neq 0 (
    echo ERROR: Failed to restore packages!
    pause
    exit /b 1
)
echo.

echo [3/4] Building Debug configuration...
dotnet build -c Debug --no-restore
if %errorlevel% neq 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)
echo.

echo [4/4] Running application...
echo.
echo ====================================
echo   Starting Application...
echo ====================================
echo.
dotnet run --no-build
echo.
echo Application closed.
pause

