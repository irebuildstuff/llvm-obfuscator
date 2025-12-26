# Installing LLVM/Clang for Windows

The obfuscation service requires LLVM/Clang to compile C/C++ code to LLVM IR.

## Quick Installation

1. **Download LLVM for Windows:**
   - Visit: https://github.com/llvm/llvm-project/releases
   - Download the latest Windows installer (e.g., `LLVM-18.x.x-win64.exe`)
   - Or use the official builds: https://llvm.org/builds/

2. **Install LLVM:**
   - Run the installer
   - **Important:** Check "Add LLVM to the system PATH" during installation
   - Default installation path: `C:\Program Files\LLVM`

3. **Verify Installation:**
   ```powershell
   clang --version
   ```

4. **If clang is not in PATH:**
   - Set the `CLANG_PATH` environment variable:
   ```powershell
   $env:CLANG_PATH = "C:\Program Files\LLVM\bin\clang.exe"
   ```
   - Or add to your `.env` file in the backend directory:
   ```
   CLANG_PATH=C:\Program Files\LLVM\bin\clang.exe
   ```

## Alternative: Using Chocolatey

```powershell
choco install llvm
```

## Restart Backend

After installing LLVM, restart the backend server for the changes to take effect.


