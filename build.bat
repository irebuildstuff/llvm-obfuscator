@echo off
REM Fixed build script for LLVM Obfuscator (Windows)
REM This uses Clang directly to avoid Visual Studio DIA SDK issues

echo === Building LLVM Obfuscator (Fixed Version) ===

set LLVM_DIR=C:\Program Files\LLVM
set CLANG="%LLVM_DIR%\bin\clang++.exe"

REM Check if LLVM is installed
if not exist %CLANG% (
    echo Error: LLVM not found at %LLVM_DIR%
    echo Please install LLVM from: https://releases.llvm.org/
    exit /b 1
)

echo Using LLVM from: %LLVM_DIR%
%CLANG% --version

REM Create output directory
if not exist output mkdir output

echo.
echo === Compiling Source Files ===

REM Compile ObfuscationPass.cpp
echo Compiling ObfuscationPass.cpp...
%CLANG% -c src/ObfuscationPass.cpp -o output/ObfuscationPass.o ^
    -I"include" ^
    -I"%LLVM_DIR%\include" ^
    -std=c++17 ^
    -D_CRT_SECURE_NO_WARNINGS ^
    -O2

if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to compile ObfuscationPass.cpp
    exit /b 1
)

REM Compile llvm-obfuscator.cpp
echo Compiling llvm-obfuscator.cpp...
%CLANG% -c tools/llvm-obfuscator.cpp -o output/llvm-obfuscator.o ^
    -I"include" ^
    -I"%LLVM_DIR%\include" ^
    -std=c++17 ^
    -D_CRT_SECURE_NO_WARNINGS ^
    -O2

if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to compile llvm-obfuscator.cpp
    exit /b 1
)

echo.
echo === Linking Executable ===
%CLANG% output/ObfuscationPass.o output/llvm-obfuscator.o -o output/llvm-obfuscator.exe ^
    -L"%LLVM_DIR%\lib" ^
    "%LLVM_DIR%\lib\*.lib" ^
    -lntdll ^
    -lVersion ^
    -lole32 ^
    -loleaut32 ^
    -luuid ^
    -ladvapi32 ^
    -lshell32 ^
    -luser32 ^
    -lkernel32

if %ERRORLEVEL% NEQ 0 (
    echo Error: Linking failed
    exit /b 1
)

echo.
echo ========================================
echo Build successful!
echo Executable: output\llvm-obfuscator.exe
echo ========================================

REM Test the build
echo.
echo === Testing the Obfuscator ===

REM Create a simple test if it doesn't exist
if not exist test_program.c (
    echo Creating test program...
    (
        echo #include ^<stdio.h^>
        echo.
        echo int main^(^) {
        echo     printf^("Hello, World!\n"^);
        echo     return 0;
        echo }
    ) > test_program.c
)

REM Compile test to LLVM IR
echo Compiling test program to LLVM IR...
"%LLVM_DIR%\bin\clang.exe" -S -emit-llvm test_program.c -o test_program.ll

if %ERRORLEVEL% NEQ 0 (
    echo Warning: Could not compile test program
    goto :end
)

REM Run obfuscator
echo Running obfuscator on test program...
output\llvm-obfuscator.exe test_program.ll -o test_obfuscated.ll --cf --bogus --loops --polymorphic --poly-variants 2 --report test_report.txt

if %ERRORLEVEL% EQU 0 (
    echo.
    echo === Test Successful! ===
    echo Obfuscated file: test_obfuscated.ll
    echo Report: test_report.txt
    type test_report.txt | findstr "Total"
) else (
    echo Warning: Obfuscation test failed
)

:end
echo.
echo === Build Complete ===
echo.
echo To use the obfuscator:
echo   output\llvm-obfuscator.exe input.ll -o output.ll [options]
echo.
echo Options:
echo   --cf       Enable control flow obfuscation
echo   --bogus    Enable bogus code insertion
echo   --loops    Enable fake loop insertion
echo   --str      Enable string encryption
echo   --help     Show all options
echo.
pause
