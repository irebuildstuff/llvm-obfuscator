@echo off
REM Build and obfuscate example programs

set LLVM_DIR=C:\Program Files\LLVM
set CLANG="%LLVM_DIR%\bin\clang.exe"
set CLANGPP="%LLVM_DIR%\bin\clang++.exe"
set OBFUSCATOR="..\output\llvm-obfuscator.exe"

echo === Building Example Programs ===
echo.

REM Check if obfuscator exists
if not exist %OBFUSCATOR% (
    echo Error: Obfuscator not found. Please build the project first.
    echo Run: build.bat in the parent directory
    exit /b 1
)

REM Compile simple C example
echo Compiling simple_example.c...
%CLANG% -S -emit-llvm simple_example.c -o simple_example.ll
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile simple_example.c
    exit /b 1
)

REM Obfuscate simple example
echo Obfuscating simple_example...
%OBFUSCATOR% simple_example.ll -o simple_example_obf.ll --cf --str --bogus --loops --report simple_report.txt
if %ERRORLEVEL% NEQ 0 (
    echo Failed to obfuscate simple_example
    exit /b 1
)

REM Build final executable
echo Building simple_example.exe...
%CLANG% simple_example_obf.ll -o simple_example_protected.exe
echo.

REM Compile advanced C++ example
echo Compiling advanced_example.cpp...
%CLANGPP% -S -emit-llvm advanced_example.cpp -o advanced_example.ll -std=c++17
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile advanced_example.cpp
    exit /b 1
)

REM Obfuscate advanced example with more techniques
echo Obfuscating advanced_example...
%OBFUSCATOR% advanced_example.ll -o advanced_example_obf.ll ^
    --cf --str --bogus --loops --subs --mba ^
    --anti-debug --indirect --cycles 5 ^
    --report advanced_report.txt
if %ERRORLEVEL% NEQ 0 (
    echo Failed to obfuscate advanced_example
    exit /b 1
)

REM Build final executable
echo Building advanced_example.exe...
%CLANGPP% advanced_example_obf.ll -o advanced_example_protected.exe -std=c++17
echo.

echo === Build Complete ===
echo.
echo Protected executables:
echo   - simple_example_protected.exe
echo   - advanced_example_protected.exe
echo.
echo Obfuscation reports:
echo   - simple_report.txt
echo   - advanced_report.txt
echo.
pause
