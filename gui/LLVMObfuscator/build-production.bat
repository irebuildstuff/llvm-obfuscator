@echo off
REM ========================================
REM LLVM Code Obfuscator v4.0
REM Production Build Script
REM ========================================

echo.
echo ====================================
echo   LLVM Code Obfuscator v4.0
echo   Production Build Script
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

echo [1/5] Checking .NET SDK version...
dotnet --version
echo.

echo [2/5] Restoring NuGet packages...
dotnet restore
if %errorlevel% neq 0 (
    echo ERROR: Failed to restore packages!
    pause
    exit /b 1
)
echo.

echo [3/5] Cleaning previous builds...
dotnet clean -c Release
echo.

echo [4/5] Building Release configuration...
dotnet build -c Release --no-restore
if %errorlevel% neq 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)
echo.

echo [5/5] Publishing self-contained executable...
dotnet publish -c Release -r win-x64 --self-contained -p:PublishSingleFile=true -p:PublishTrimmed=true
if %errorlevel% neq 0 (
    echo ERROR: Publish failed!
    pause
    exit /b 1
)
echo.

echo ====================================
echo   BUILD SUCCESSFUL!
echo ====================================
echo.
echo Output location:
echo   bin\Release\net8.0\win-x64\publish\
echo.
echo Files created:
echo   - LLVMObfuscatorAvalonia.exe (main executable)
echo   - appsettings.json (configuration)
echo   - runtimes\ (native dependencies)
echo.
echo Ready for deployment!
echo.
pause

