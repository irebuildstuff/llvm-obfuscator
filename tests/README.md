# CLI vs Web App Verification Test Suite

This test suite verifies that the web application produces identical obfuscation results as the CLI tool (`llvm-obfuscator.exe`) for all techniques, presets, and pipeline stages.

## Overview

The test suite:
- Runs obfuscation using both CLI and Web App
- Compares IR files, executables, and reports
- Verifies all 15+ obfuscation techniques individually
- Tests all 3 presets (Basic, Medium, Heavy)
- Validates the complete pipeline (C/C++ → IR → Obfuscated IR → Executable)

## Prerequisites

1. **LLVM/Clang** installed and accessible
   - Default path: `C:\Program Files\LLVM\bin\clang.exe`
   - Or set `CLANG_PATH` environment variable

2. **CLI Tool** built and available
   - Default path: `output/llvm-obfuscator.exe`
   - Or set `OBFUSCATOR_PATH` environment variable

3. **Web App Backend** running
   - Default URL: `http://localhost:3001`
   - Or set `API_URL` environment variable
   - **Important**: Restart the backend server after any backend code changes to ensure tests use the latest configuration

4. **Node.js** dependencies installed
   ```bash
   cd tests
   npm install
   ```

## Configuration

Edit `tests/config.js` or set environment variables:

- `OBFUSCATOR_PATH` - Path to CLI tool executable
- `CLANG_PATH` - Path to clang executable
- `API_URL` - Web app API URL (default: http://localhost:3001)

## Running Tests

### Run All Tests
```bash
cd tests
npm test
```

### Run Specific Test Categories
```bash
# Only technique tests
npm run test:techniques

# Only preset tests
npm run test:presets

# Only pipeline tests
npm run test:pipeline
```

### Run Individual Test
```bash
node -e "import('./techniques/test-control-flow.js').then(m => m.testControlFlow().then(console.log))"
```

## Test Structure

### Technique Tests (`tests/techniques/`)
Tests each obfuscation technique individually:
- `test-control-flow.js` - Control Flow Obfuscation
- `test-string-encryption.js` - String Encryption
- `test-bogus-code.js` - Bogus Code Insertion
- `test-fake-loops.js` - Fake Loops
- `test-instruction-sub.js` - Instruction Substitution
- `test-flattening.js` - Control Flow Flattening
- `test-mba.js` - Mixed Boolean Arithmetic
- `test-anti-debug.js` - Anti-Debugging
- `test-indirect-calls.js` - Indirect Calls
- `test-constant-obf.js` - Constant Obfuscation
- `test-anti-tamper.js` - Anti-Tampering
- `test-virtualization.js` - Virtualization
- `test-polymorphic.js` - Polymorphic Code
- `test-anti-analysis.js` - Anti-Analysis
- `test-metamorphic.js` - Metamorphic
- `test-dynamic.js` - Dynamic Obfuscation

### Preset Tests (`tests/presets/`)
Tests the three preset configurations:
- `test-basic-preset.js` - Basic preset
- `test-medium-preset.js` - Medium preset
- `test-heavy-preset.js` - Heavy preset

### Pipeline Tests (`tests/pipeline/`)
Tests the complete obfuscation pipeline:
- `test-full-pipeline.js` - Full pipeline (source → IR → obfuscated IR → executable)
- `test-exception-handling.js` - Exception handling with `-fno-exceptions`

## Comparison Methods

### IR File Comparison
- Normalizes whitespace and comments
- Compares function structures
- Compares instruction sequences
- Allows 15% tolerance for minor differences

### Executable Comparison
- Runs both executables
- Compares stdout output
- Verifies exit codes match

### Report Comparison
- Parses metrics from both reports
- Compares technique application counts
- Allows 20% tolerance for metrics

## Output

Test results are saved to:
- `tests/output/cli/` - CLI tool outputs
- `tests/output/webapp/` - Web app outputs

Each test creates:
- `test_<name>.ll` - Original IR
- `test_<name>_obfuscated.ll` - Obfuscated IR
- `test_<name>_obfuscated.exe` - Compiled executable
- `test_<name>_report.txt` - Obfuscation report

## Success Criteria

Expected test results:
- ✅ 13-15/16 technique tests (some techniques have known bugs: flattening, anti-tamper, virtualization)
- ✅ 2-3/3 preset tests (heavy preset may run out of memory)
- ✅ 1-2/2 pipeline tests
- ✅ IR files match (within 50% tolerance - obfuscation can vary)
- ✅ Reports show matching techniques applied
- ✅ Executables produce identical output (when compilation succeeds)

**Note**: Some failures are expected due to:
- Control Flow Flattening: Known LLVM IR corruption issue
- Anti-Tampering: Instruction numbering bug in obfuscator
- Virtualization: CLI tool crash (known bug)
- Heavy Preset: Runs out of memory (expected with all techniques)

## Troubleshooting

### CLI Tool Not Found
```
Error: CLI tool not found: output/llvm-obfuscator.exe
```
Solution: Build the CLI tool or set `OBFUSCATOR_PATH` environment variable.

### Clang Not Found
```
Error: Clang not found: C:\Program Files\LLVM\bin\clang.exe
```
Solution: Install LLVM/Clang or set `CLANG_PATH` environment variable.

### Web App Not Responding
```
Error: Failed to start obfuscation: 500
```
Solution: Ensure the backend server is running on the configured URL.

### Test Timeout
```
Error: Job timeout after 300000ms
```
Solution: Increase `WEBAPP_TIMEOUT` in `config.js` for slow systems.

## Contributing

When adding new obfuscation techniques:
1. Add configuration to `tests/config.js` in `TECHNIQUE_CONFIGS`
2. Create test file in `tests/techniques/test-<name>.js`
3. Import and add to `techniqueTests` array in `tests/run-tests.js`


