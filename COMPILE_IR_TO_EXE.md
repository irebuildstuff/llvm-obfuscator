# Compiling Obfuscated LLVM IR to Executable

## Quick Command

```bash
clang -fno-exceptions <input.ll> -o <output.exe>
```

## Example

```bash
# Compile obfuscated IR to executable
clang -fno-exceptions output/test_complete.ll -o output/test_complete.exe

# Run the executable
./output/test_complete.exe
```

## Important Flag: `-fno-exceptions`

The `-fno-exceptions` flag is **required** because:
- Our obfuscation pass removes exception handling from LLVM IR
- Without this flag, clang will try to link exception handling code that doesn't exist
- This prevents "Broken module found" errors during compilation

## Full Path (Windows)

If clang is not in your PATH:

```bash
"C:\Program Files\LLVM\bin\clang.exe" -fno-exceptions input.ll -o output.exe
```

## Verification

After compilation, verify the executable works:

```bash
# Check if file exists
Test-Path output/test_complete.exe

# Run the executable
./output/test_complete.exe
```

## Test Results

✅ **Successfully compiled:**
- `output/test_complete.ll` → `output/test_complete.exe` (143 KB)
- `examples/test_fixed_path.ll` → `examples/test_fixed_path.exe` (142.5 KB)
- `test_simple.ll` → `test_simple.exe` (142.5 KB)

✅ **All executables run correctly:**
- Output: "Protected Application v1.0"
- Output: "Invalid license key!"

## Using the CLI Tool with Auto-Compile

The CLI tool can automatically compile to executable:

```bash
# Auto-compile to .exe (creates both .ll and .exe)
llvm-obfuscator.exe input.c -o output.exe --cf --str --cycles 1

# Or use --auto-compile flag
llvm-obfuscator.exe input.ll -o output.exe --auto-compile --cf --str
```

## Troubleshooting

### Error: "Broken module found"
- **Solution**: Add `-fno-exceptions` flag

### Error: "clang not found"
- **Solution**: Use full path: `"C:\Program Files\LLVM\bin\clang.exe"`

### Error: "Cannot open input file"
- **Solution**: Check file path and ensure .ll file exists

## Notes

- The obfuscated executables are fully functional
- All obfuscation techniques are preserved in the final executable
- File size is typically 140-150 KB for simple programs
- The executable behavior matches the original program

