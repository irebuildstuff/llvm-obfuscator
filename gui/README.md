# LLVM Code Obfuscator GUI

A modern, user-friendly Windows desktop application for the LLVM Code Obfuscator. This GUI makes it easy for everyone to use the powerful obfuscation tool without needing command-line knowledge.

## 🚀 Features

- **Native Windows Application**: Built with .NET Windows Forms for optimal performance
- **Drag & Drop Interface**: Simply browse and select your LLVM IR files
- **Preset Configurations**: Choose from Basic, Standard, or Aggressive obfuscation presets
- **Real-time Progress**: Watch the obfuscation process with live progress updates
- **Interactive Controls**: Fine-tune all obfuscation parameters with sliders and checkboxes
- **File Management**: Automatic file handling with download capabilities
- **Comprehensive Reports**: Detailed obfuscation statistics and results
- **Professional UI**: Modern, intuitive interface following Windows design guidelines

## 📋 Requirements

- **Windows 10/11** (64-bit)
- **Built LLVM Obfuscator** (llvm-obfuscator.exe in the output folder)
- **.NET 9.0 Runtime** (included in standalone executable)

## 🔧 Building the Application

### Prerequisites

1. **Install .NET 9.0 SDK**:
   - Download from: https://dotnet.microsoft.com/download
   - Make sure .NET is added to your system PATH

2. **Build the LLVM Obfuscator** (if not already done):
   ```bash
   cd ..
   build.bat
   ```

### Build Steps

1. **Run the build script**:
   ```bash
   cd LLVMObfuscatorGUI
   build.bat
   ```

2. **The executable will be created** in the `publish` folder:
   - `LLVMObfuscatorGUI.exe` - Standalone executable
   - `llvm-obfuscator.exe` - Obfuscator executable (copied automatically)

## 📖 How to Use

### Step 1: Launch the Application
- Double-click `LLVMObfuscatorGUI.exe`
- The application will automatically find the obfuscator executable

### Step 2: Select Your File
- Click "Browse..." to select your LLVM IR file (.ll or .bc)
- The file information will be displayed
- Supported formats: `.ll` (LLVM IR text), `.bc` (LLVM IR binary)

### Step 3: Choose Obfuscation Settings

#### Presets
- **Basic**: Essential obfuscation with minimal performance impact
- **Standard**: Balanced obfuscation for most use cases  
- **Aggressive**: Maximum protection with higher performance cost
- **Custom**: Manual configuration of all options

#### Core Techniques
- **Control Flow Obfuscation**: Inserts opaque predicates to confuse analysis
- **String Encryption**: Encrypts string literals with XOR cipher
- **Bogus Code Insertion**: Adds meaningless instructions
- **Fake Loop Insertion**: Creates dead loops with false conditions

#### Advanced Techniques
- **Instruction Substitution**: Replaces simple operations with complex ones
- **Control Flow Flattening**: Transforms functions into switch dispatchers
- **Mixed Boolean Arithmetic**: Replaces arithmetic with boolean expressions
- **Code Virtualization**: Implements VM-based instruction interpretation

#### Security Features
- **Anti-Debugging**: Detects and responds to debugger presence
- **Anti-Tampering**: Adds integrity checks
- **Anti-Analysis**: Detects common analysis tools
- **Polymorphic Code**: Creates multiple function variants

### Step 4: Configure Output
- Set output filename (default: obfuscated_output.ll)
- Set report filename (default: obfuscation_report.txt)
- Choose to generate Windows/Linux binaries

### Step 5: Start Obfuscation
- Click "Start Obfuscation" to begin the process
- Switch to "Progress & Results" tab to monitor progress
- Download results when complete

## 🔧 Configuration Options

### Parameters
- **Obfuscation Cycles**: Number of obfuscation passes (1-10)
- **Bogus Code %**: Percentage of bogus code per basic block (10-80%)
- **Fake Loops**: Number of fake loops to insert (1-20)

### Output Options
- **Output Filename**: Name for the obfuscated LLVM IR file
- **Report Filename**: Name for the obfuscation report
- **Generate Windows Binary**: Compile to .exe file
- **Generate Linux Binary**: Compile to Linux executable

## 📊 Understanding Results

The GUI provides detailed statistics about the obfuscation process:

- **Control Flow Obfuscations**: Opaque predicates inserted
- **String Encryptions**: Strings encrypted with XOR
- **Bogus Instructions**: Meaningless instructions added
- **Fake Loops**: Dead loops inserted
- **Obfuscation Cycles**: Number of passes completed
- **Processing Time**: Time taken for obfuscation

## 🛠️ Technical Details

### Architecture
- **Framework**: .NET 9.0 Windows Forms
- **Language**: C# 12
- **UI**: Native Windows Forms with modern styling
- **Process Management**: Async/await for non-blocking UI
- **File Handling**: Secure file operations with validation

### Key Components
- **MainForm**: Main application window with tabbed interface
- **File Selection**: Browse and validate LLVM IR files
- **Options Panel**: Interactive controls for all obfuscation settings
- **Progress Monitor**: Real-time progress tracking
- **Results Display**: Comprehensive obfuscation statistics

### Security Features
- File type validation (.ll, .bc only)
- File size limits
- Process isolation
- Error handling and recovery
- Safe file operations

## 🐛 Troubleshooting

### Common Issues

1. **"Obfuscator executable not found"**
   - Ensure you've built the obfuscator first
   - Check that `llvm-obfuscator.exe` exists in the output folder
   - Copy `llvm-obfuscator.exe` to the same directory as the GUI

2. **".NET not found"**
   - Install .NET 9.0 Runtime from Microsoft's website
   - Or use the standalone executable (includes runtime)

3. **"File upload fails"**
   - Check file size (no specific limit, but very large files may be slow)
   - Ensure file is .ll or .bc format
   - Check that the file is not corrupted

4. **"Obfuscation process fails"**
   - Check that the input file is valid LLVM IR
   - Ensure sufficient disk space
   - Check Windows Event Viewer for detailed error messages

5. **"Application won't start"**
   - Ensure Windows 10/11 (64-bit)
   - Check that all dependencies are installed
   - Run as administrator if needed

### Getting Help

- Check the Help button in the application
- Ensure all dependencies are installed correctly
- Verify the obfuscator executable works from command line
- Check Windows Event Viewer for system-level errors

## 🔄 Example Workflow

1. **Compile your C/C++ code to LLVM IR**:
   ```bash
   clang -S -emit-llvm source.c -o source.ll
   ```

2. **Launch the GUI**:
   ```bash
   # Navigate to publish folder
   cd LLVMObfuscatorGUI\publish
   # Double-click LLVMObfuscatorGUI.exe
   ```

3. **Use the application**:
   - Upload `source.ll`
   - Select "Standard" preset
   - Click "Start Obfuscation"
   - Monitor progress in the Progress & Results tab

4. **Download results**:
   - Download `obfuscated_output.ll`
   - Download `obfuscation_report.txt`

5. **Compile obfuscated code**:
   ```bash
   clang obfuscated_output.ll -o protected_program.exe
   ```

## 📝 Development

### Building from Source

1. **Clone/Download the project**
2. **Install .NET 9.0 SDK**
3. **Build the obfuscator** (parent directory)
4. **Run build script**:
   ```bash
   build.bat
   ```

### Project Structure

```
LLVMObfuscatorGUI/
├── LLVMObfuscatorGUI/
│   ├── MainForm.cs          # Main application logic
│   ├── Form1.Designer.cs    # Form designer code
│   ├── Program.cs           # Application entry point
│   └── LLVMObfuscatorGUI.csproj  # Project file
├── build.bat               # Build script
└── README.md              # This file
```

## 📄 License

This GUI is part of the LLVM Obfuscator project and is licensed under the MIT License.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

---

**Note**: This GUI significantly simplifies the use of the LLVM obfuscator while maintaining all its powerful features. The obfuscated code may have performance implications, so always test thoroughly before deployment.

