@echo off
echo Building LLVM Obfuscator Avalonia UI...

cd LLVMObfuscatorAvalonia

echo Restoring packages...
dotnet restore

echo Building application...
dotnet build --configuration Release

echo Publishing application...
dotnet publish --configuration Release --runtime win-x64 --self-contained true --output ../publish-avalonia

echo Build complete! Executable is in ../publish-avalonia/
pause
