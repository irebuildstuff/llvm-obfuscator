# LLVM Obfuscator: CLI vs Web App Workflow

This document explains how both the CLI tool and Web App process obfuscation requests.

## Overview

Both the CLI tool and Web App use the **same underlying obfuscation engine** (`ObfuscationPass`), but they differ in:
- **Input method**: CLI uses command-line arguments, Web App uses HTTP API
- **Execution**: CLI runs synchronously, Web App runs asynchronously with job management
- **Output**: CLI writes directly to files, Web App stores outputs and serves via API

---

## CLI Tool Workflow

### 1. **Command Parsing** (`tools/llvm-obfuscator.cpp`)

```
User runs: llvm-obfuscator.exe input.c --cf --str --bogus -o output.ll
```

**Process:**
- Parses command-line arguments using LLVM's `cl::ParseCommandLineOptions`
- Extracts flags: `--cf`, `--str`, `--bogus`, etc.
- Builds `ObfuscationConfig` object from flags
- Determines input/output file paths

**Key Flags:**
- `--cf` → Control Flow Obfuscation
- `--str` → String Encryption
- `--bogus` → Bogus Code Insertion
- `--cycles=N` → Number of obfuscation cycles
- `-o <file>` → Output file path
- `--report=<file>` → Report file path

### 2. **Input Processing**

**If input is C/C++ source file** (`.c`, `.cpp`):
```
input.c → [clang -S -emit-llvm] → input.ll (LLVM IR)
```

**If input is already LLVM IR** (`.ll`):
```
input.ll → Use directly
```

### 3. **Module Loading**

```cpp
LLVMContext Context;
SMDiagnostic Err;
std::unique_ptr<Module> M = parseIRFile(actualInputFile, Err, Context);
```

- Loads LLVM IR file into memory
- Creates LLVM `Module` object
- Validates IR syntax

### 4. **Obfuscation Execution**

```cpp
ObfuscationConfig config;
// ... set config from command-line flags ...

ObfuscationPass obfuscationPass(config);
bool modified = obfuscationPass.runOnModule(*M);
```

**What happens:**
- Creates `ObfuscationPass` instance with configuration
- Runs obfuscation passes on the module
- Applies selected techniques (control flow, string encryption, etc.)
- Returns whether module was modified

### 5. **Output Writing**

```cpp
raw_fd_ostream OutFile(outputFile, EC);
M->print(OutFile, nullptr);
```

- Writes obfuscated LLVM IR to output file
- Generates obfuscation report (if `--report` specified)
- Optionally compiles to executable (if `--compile` or `-o output.exe`)

### 6. **Optional: Compile to Executable**

```
obfuscated.ll → [clang] → obfuscated.exe
```

**Complete CLI Flow:**
```
User Command
    ↓
Parse Arguments
    ↓
[If C/C++] Compile to IR (clang)
    ↓
Load LLVM IR Module
    ↓
Create ObfuscationPass(config)
    ↓
Run Obfuscation (runOnModule)
    ↓
Write Obfuscated IR
    ↓
[Optional] Compile to Executable
    ↓
Generate Report
    ↓
Done (synchronous)
```

---

## Web App Workflow

### 1. **HTTP Request** (`backend/src/server.js`)

```
POST /api/obfuscate
Content-Type: multipart/form-data
- file: <uploaded file>
- preset: "basic" | "medium" | "heavy" | "custom"
- options: JSON string with technique flags
```

**Process:**
- Receives file upload via Multer middleware
- Parses `preset` and `options` from request body
- Calls `parseObfuscationConfig(preset, options)` to build config
- Returns `jobId` immediately (async processing)

### 2. **Job Creation** (`backend/src/job-manager.js`)

```javascript
const jobId = await createJob({
  inputPath: req.file.path,
  originalFilename: req.file.originalname,
  config
});
```

- Creates unique job ID (UUID)
- Stores job metadata in memory/state
- Initial status: `'processing'`

### 3. **Asynchronous Pipeline** (`backend/src/obfuscation-service.js`)

The pipeline runs in the background:

#### **Step 1: Compile C/C++ to LLVM IR** (Progress: 10-25%)

```javascript
await compileToLLVMIR(inputFilePath, irPath);
// Uses: clang -S -emit-llvm -fno-exceptions input.c -o input.ll
```

- Compiles source file to LLVM IR
- Uses `-fno-exceptions` flag to avoid exception handling issues
- Saves IR to: `{jobId}-{filename}.ll`

#### **Step 1.5: Strip Exception Handling** (Progress: 25-30%)

```javascript
await stripExceptionHandling(irPath, jobId);
// Uses: opt -strip-eh input.ll -o cleaned.ll
```

- Removes exception handling from IR (prevents obfuscation bugs)
- Uses LLVM `opt` tool with `-strip-eh` pass

#### **Step 2: Run Obfuscator** (Progress: 30-60%)

```javascript
await runObfuscator(irToObfuscate, obfuscatedIrPath, reportPath, config);
// Executes: llvm-obfuscator.exe input.ll -o output.ll --report=report.txt [flags]
```

**What happens:**
- Builds command-line arguments from config
- Executes CLI tool as subprocess
- CLI tool loads IR, runs obfuscation, writes output
- Saves obfuscated IR to: `{jobId}-{filename}_obfuscated.ll`
- Saves report to: `{jobId}-obfuscation_report.txt`

#### **Step 3: Compile to Executable** (Progress: 60-80%)

```javascript
await compileToExecutable(obfuscatedIrPath, executablePath);
// Uses: clang obfuscated.ll -o obfuscated.exe -fno-exceptions -lstdc++
```

- Compiles obfuscated IR to executable
- Links with standard libraries
- Saves executable to: `{jobId}-{filename}_obfuscated.exe`

#### **Step 4: Parse Report** (Progress: 80-100%)

```javascript
const reportData = await parseReport(reportPath);
```

- Parses obfuscation report (metrics, techniques applied)
- Stores report data in job status

#### **Step 5: Mark Complete**

```javascript
await updateJob(jobId, {
  status: 'completed',
  progress: 100,
  report: reportData
});
```

### 4. **Status Polling** (Frontend)

```
GET /api/status/:jobId
```

**Response:**
```json
{
  "status": "processing" | "completed" | "failed",
  "progress": 0-100,
  "message": "Compiling C/C++ to LLVM IR...",
  "irPath": "...",
  "obfuscatedIrPath": "...",
  "executablePath": "...",
  "reportPath": "...",
  "report": { ... }
}
```

- Frontend polls this endpoint every 2 seconds
- Updates UI with progress and status
- When `status === 'completed'`, shows download links

### 5. **File Downloads** (Frontend)

```
GET /api/download/:jobId/executable
GET /api/download/:jobId/ir
GET /api/download/:jobId/report
GET /api/view/:jobId/ir  (returns IR content as JSON)
```

- Serves generated files to frontend
- Files are stored in `backend/output/` directory

**Complete Web App Flow:**
```
User Uploads File
    ↓
POST /api/obfuscate
    ↓
Create Job (jobId)
    ↓
Return jobId (immediate response)
    ↓
[Background] Compile to IR (clang)
    ↓
[Background] Strip Exceptions (opt)
    ↓
[Background] Run Obfuscator (CLI tool subprocess)
    ↓
[Background] Compile to Executable (clang)
    ↓
[Background] Parse Report
    ↓
Update Job Status: completed
    ↓
Frontend Polls Status
    ↓
Frontend Downloads Files
    ↓
Done (asynchronous)
```

---

## Key Differences

| Aspect | CLI Tool | Web App |
|--------|----------|---------|
| **Input** | Command-line arguments | HTTP multipart/form-data |
| **Execution** | Synchronous (blocks until done) | Asynchronous (returns jobId immediately) |
| **Progress** | Progress bar in terminal | HTTP status polling |
| **Output** | Direct file write | Stored in `backend/output/`, served via API |
| **Error Handling** | Immediate error messages | Job status with error field |
| **Configuration** | Command-line flags | Preset + JSON options |
| **File Management** | User manages files | Server manages temporary files |

---

## Shared Components

Both workflows use:

1. **`ObfuscationPass`** (`src/ObfuscationPass.cpp`)
   - Core obfuscation logic
   - Applies techniques to LLVM IR
   - Same code for both CLI and Web App

2. **`ObfuscationConfig`** structure
   - Configuration object with technique flags
   - Same structure, different initialization methods

3. **LLVM Toolchain**
   - `clang` for compilation
   - `opt` for IR manipulation
   - LLVM IR format

---

## Configuration Mapping

### CLI Flags → Web App Config

| CLI Flag | Web App Config Field |
|----------|-------------------|
| `--cf` | `enableControlFlowObfuscation: true` |
| `--str` | `enableStringEncryption: true` |
| `--bogus` | `enableBogusCode: true` |
| `--cycles=3` | `obfuscationCycles: 3` |
| `--bogus-percent=30` | `bogusCodePercentage: 30` |
| `--flatten` | `enableControlFlowFlattening: true` |
| `--mba` | `enableMBA: true` |
| ... | ... |

### Presets

**Basic Preset:**
- CLI: `--cf`
- Web: `{ preset: 'basic' }` → Only control flow, 1 cycle

**Medium Preset:**
- CLI: `--cf --str --bogus --cycles=3`
- Web: `{ preset: 'medium' }` → Control flow + string encryption + bogus code, 3 cycles

**Heavy Preset:**
- CLI: All flags enabled with high settings
- Web: `{ preset: 'heavy' }` → All techniques, 5 cycles

---

## Example: Control Flow Obfuscation

### CLI:
```bash
llvm-obfuscator.exe example.c --cf -o example_obfuscated.ll --report=report.txt
```

**Steps:**
1. Compile `example.c` → `example.ll`
2. Load `example.ll` into LLVM Module
3. Create `ObfuscationPass` with `enableControlFlowObfuscation: true`
4. Run `obfuscationPass.runOnModule(M)`
5. Write obfuscated IR to `example_obfuscated.ll`
6. Write report to `report.txt`

### Web App:
```javascript
POST /api/obfuscate
{
  file: <example.c>,
  preset: 'basic',
  options: {}
}
```

**Steps:**
1. Save uploaded file to `backend/uploads/{jobId}-example.c`
2. Create job with status `'processing'`
3. Return `{ jobId: '...' }`
4. [Background] Compile to IR: `clang example.c → {jobId}-example.ll`
5. [Background] Strip exceptions: `opt -strip-eh → cleaned.ll`
6. [Background] Run obfuscator: `llvm-obfuscator.exe cleaned.ll --cf → obfuscated.ll`
7. [Background] Compile to exe: `clang obfuscated.ll → obfuscated.exe`
8. Update job status: `'completed'`
9. Frontend polls status and downloads files

---

## Error Handling

### CLI:
- Errors printed to stderr immediately
- Exit code indicates success/failure
- User sees error in terminal

### Web App:
- Errors caught in try/catch blocks
- Job status updated to `'failed'` with error message
- Frontend displays error from job status
- Files cleaned up on error

---

## File Lifecycle

### CLI:
- User provides input file path
- Output written to specified path (or default)
- User manages all files

### Web App:
- Uploaded file: `backend/uploads/{jobId}-{filename}`
- IR file: `backend/output/{jobId}-{filename}.ll`
- Obfuscated IR: `backend/output/{jobId}-{filename}_obfuscated.ll`
- Executable: `backend/output/{jobId}-{filename}_obfuscated.exe`
- Report: `backend/output/{jobId}-obfuscation_report.txt`
- Files can be cleaned up after download (optional)

---

## Summary

Both the CLI and Web App achieve the same goal: obfuscate LLVM IR code using the same underlying engine. The CLI is simpler and synchronous, while the Web App provides a user-friendly interface with async processing, progress tracking, and file management.

The test suite (`tests/`) verifies that both produce identical results by:
1. Running the same input through both CLI and Web App
2. Comparing the generated IR files, reports, and executables
3. Ensuring the same techniques are applied with the same configuration

