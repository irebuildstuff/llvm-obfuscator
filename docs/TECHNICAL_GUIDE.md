# Technical Guide

## Architecture Overview

The LLVM Obfuscator is built as a modular pass-based system that operates on LLVM Intermediate Representation (IR). This design allows for:
- Language-agnostic obfuscation
- Composable transformation passes
- Integration with existing LLVM toolchains

## Core Components

### 1. ObfuscationPass Class
The main transformation engine that implements the LLVM `ModulePass` interface.

**Key Methods:**
- `runOnModule(Module &M)` - Entry point for obfuscation
- `obfuscateControlFlow(Function &F)` - Control flow transformations
- `obfuscateStrings(Module &M)` - String encryption
- `insertBogusCode(Function &F)` - Dead code insertion

### 2. Configuration System
Flexible configuration through `ObfuscationConfig` struct:
```cpp
struct ObfuscationConfig {
    bool enableControlFlowObfuscation;
    bool enableStringEncryption;
    int obfuscationCycles;
    int bogusCodePercentage;
    // ... more options
};
```

### 3. Metrics and Reporting
Comprehensive metrics tracking:
- Transformation counts
- Performance impact
- Code size changes
- Detailed HTML/text reports

## Obfuscation Techniques

### Control Flow Obfuscation

#### Opaque Predicates
Creates conditions that are always true/false but difficult to analyze statically:
```llvm
%n = load i32, i32* %alloc
%n_plus_1 = add i32 %n, 1
%prod = mul i32 %n, %n_plus_1
%mod = urem i32 %prod, 2
%cond = icmp eq i32 %mod, 0  ; Always true
```

#### Control Flow Flattening
Transforms natural control flow into a switch-based dispatcher:
```
Original:          Flattened:
  A                  dispatcher:
  ├─B                 switch %state
  │ └─D                 case 0: goto A
  └─C                   case 1: goto B
    └─D                 case 2: goto C
                        case 3: goto D
```

### String Encryption

1. **Static Analysis Phase**
   - Identifies string constants in global variables
   - Marks strings for encryption

2. **Encryption Phase**
   - XOR cipher with configurable keys
   - Replaces original strings with encrypted versions

3. **Runtime Decryption**
   - Global constructor injected
   - Decrypts strings before main() execution

### Code Virtualization

Transforms native instructions into bytecode for a custom VM:

1. **Instruction Encoding**
   ```
   ADD r1, r2 → [0x01, 0x01, 0x02]
   SUB r1, r2 → [0x02, 0x01, 0x02]
   ```

2. **VM Interpreter**
   - Fetch-decode-execute loop
   - Stack-based or register-based architecture
   - Obfuscated dispatcher

### Mixed Boolean Arithmetic (MBA)

Replaces arithmetic with boolean equivalents:
```
a + b = (a ⊕ b) + 2(a ∧ b)
a - b = (a ⊕ b) - 2(¬a ∧ b)
a × 2 = (a << 1)
```

## Performance Considerations

### Optimization Levels

1. **Light** - Minimal impact (5-10% overhead)
   - Basic control flow obfuscation
   - Limited bogus code

2. **Medium** - Moderate impact (20-40% overhead)
   - String encryption
   - Control flow flattening
   - Instruction substitution

3. **Heavy** - Significant impact (50-200% overhead)
   - Code virtualization
   - Multiple obfuscation cycles
   - Anti-debugging/tampering

### Best Practices

1. **Selective Obfuscation**
   - Focus on critical functions
   - Skip performance-critical paths
   - Use attributes to mark functions

2. **Iterative Testing**
   - Test after each obfuscation level
   - Profile performance impact
   - Validate functionality

3. **Combination Strategies**
   - Layer multiple techniques
   - Randomize technique selection
   - Vary parameters per function

## Integration Guide

### With Build Systems

#### CMake Integration
```cmake
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/obfuscated.ll
    COMMAND llvm-obfuscator 
            ${CMAKE_BINARY_DIR}/original.ll
            -o ${CMAKE_BINARY_DIR}/obfuscated.ll
            --cf --str --bogus
    DEPENDS original.ll
)
```

#### Makefile Integration
```makefile
%.obf.ll: %.ll
    llvm-obfuscator $< -o $@ --cf --str --bogus

%.exe: %.obf.ll
    clang $< -o $@
```

### With CI/CD Pipelines

```yaml
# GitHub Actions example
- name: Obfuscate Code
  run: |
    llvm-obfuscator input.ll -o output.ll \
      --cf --str --bogus --report report.txt
    
- name: Upload Report
  uses: actions/upload-artifact@v2
  with:
    name: obfuscation-report
    path: report.txt
```

## Debugging Obfuscated Code

### Techniques

1. **Conditional Obfuscation**
   ```bash
   # Debug build - no obfuscation
   cmake -DCMAKE_BUILD_TYPE=Debug
   
   # Release build - full obfuscation
   cmake -DCMAKE_BUILD_TYPE=Release
   ```

2. **Symbol Preservation**
   - Keep debug symbols separate
   - Use symbol servers
   - Map obfuscated to original

3. **Logging and Tracing**
   - Insert trace points before obfuscation
   - Use runtime flags to enable logging
   - Correlate with original source

## Security Analysis

### Effectiveness Metrics

1. **Static Analysis Resistance**
   - Cyclomatic complexity increase: 300-500%
   - Control flow graph edges: 5-10x increase
   - Pattern matching failure rate: >80%

2. **Dynamic Analysis Resistance**
   - Debugger detection rate: 70-90%
   - Execution trace complexity: 10-20x
   - Symbolic execution timeout: >95%

### Known Limitations

1. **Performance Overhead**
   - Cannot eliminate all overhead
   - Trade-off between security and speed

2. **Advanced Attacks**
   - Differential analysis
   - Side-channel attacks
   - Machine learning deobfuscation

3. **Maintenance Burden**
   - Debugging complexity
   - Update challenges
   - Documentation needs

## Future Enhancements

- **Machine Learning Integration**
  - Adaptive obfuscation strategies
  - Attack pattern learning
  
- **Hardware-Assisted Obfuscation**
  - CPU feature utilization
  - Trusted execution environments
  
- **Cloud-Based Obfuscation**
  - Distributed processing
  - Real-time updates
  - Analytics and monitoring
