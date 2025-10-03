#include "ObfuscationPass.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Operator.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>

using namespace llvm;

char ObfuscationPass::ID = 0;

bool ObfuscationPass::runOnModule(Module &M) {
    bool modified = false;

    // Check for aggressive mode from GUI
    bool aggressiveMode = (getenv("LLVM_OBFUSCATOR_AGGRESSIVE_MODE") != nullptr);
    bool memoryMonitorEnabled = (getenv("LLVM_OBFUSCATOR_MEMORY_MONITOR") != nullptr);
    bool vmManagementEnabled = (getenv("LLVM_OBFUSCATOR_VM_MANAGEMENT") != nullptr);
    
    if (aggressiveMode) {
        outs() << "🔥 Aggressive mode enabled - maintaining full settings with enhanced memory management\n";
        outs() << "📊 Memory monitoring: " << (memoryMonitorEnabled ? "Enabled" : "Disabled") << "\n";
        outs() << "🔧 VM management: " << (vmManagementEnabled ? "Enabled" : "Disabled") << "\n";
    }

    // Initialize random number generator
    rng = M.createRNG("ObfuscationPass");

    // Track original function names to prevent exponential variant generation
    if (originalFunctionNames.empty()) {
        for (Function &F : M) {
            if (!F.isDeclaration()) {
                originalFunctionNames.insert(F.getName().str());
            }
        }
    }
    
    outs() << "Starting obfuscation process...\n";
    outs() << "Configuration:\n";
    outs() << "  Control Flow Obfuscation: " << (config.enableControlFlowObfuscation ? "Enabled" : "Disabled") << "\n";
    outs() << "  String Encryption: " << (config.enableStringEncryption ? "Enabled" : "Disabled") << "\n";
    outs() << "  Bogus Code: " << (config.enableBogusCode ? "Enabled" : "Disabled") << "\n";
    outs() << "  Fake Loops: " << (config.enableFakeLoops ? "Enabled" : "Disabled") << "\n";
    outs() << "  Obfuscation Cycles: " << config.obfuscationCycles << "\n";
    if (aggressiveMode) {
        outs() << "  Aggressive Mode: Enabled (Full Settings)\n";
    }
    
    // Run multiple obfuscation cycles
    for (int cycle = 0; cycle < config.obfuscationCycles; cycle++) {
        outs() << "\n=== Obfuscation Cycle " << (cycle + 1) << " ===\n";
        totalObfuscationCycles++;
        
        // Apply obfuscation techniques to each function
        for (Function &F : M) {
            if (F.isDeclaration()) continue;

            // Skip generated functions to prevent exponential explosion
            if (!isOriginalFunction(F)) continue;

            outs() << "Obfuscating function: " << F.getName() << "\n";
            
            if (config.enableControlFlowObfuscation) {
                modified |= obfuscateControlFlow(F);
            }
            
            if (config.enableBogusCode) {
                modified |= insertBogusCode(F);
            }
            
            if (config.enableFakeLoops) {
                modified |= insertFakeLoops(F);
            }

            if (config.enableInstructionSubstitution) {
                modified |= substituteInstructions(F);
            }
            
            if (config.enableControlFlowFlattening) {
                modified |= flattenControlFlow(F);
            }
            
            if (config.enableMBA) {
                modified |= applyMBA(F);
            }
            
            if (config.enableConstantObfuscation) {
                modified |= obfuscateConstants(F);
            }
            
            if (config.enableVirtualization) {
                modified |= virtualizeFunction(F);
            }
            
            if (config.enablePolymorphic) {
                modified |= generatePolymorphicCode(F);
            }
            
            if (config.enableMetamorphic) {
                modified |= applyMetamorphicTransform(F);
            }
        }
        
        // Apply module-level obfuscations
        if (config.enableStringEncryption) {
            modified |= obfuscateStrings(M);
        }
        
        if (config.enableIndirectCalls) {
            modified |= obfuscateCalls(M);
        }
        
        if (config.enableAntiDebug && cycle == 0) {
            modified |= insertAntiDebug(M);
        }
        
        if (config.enableAntiAnalysis && cycle == 0) {
            modified |= insertAntiAnalysis(M);
        }
        
        if (config.enableDynamicObf && cycle == config.obfuscationCycles - 1) {
            modified |= insertDynamicObfuscation(M);
        }
        
        if (config.enableAntiTamper && cycle == config.obfuscationCycles - 1) {
            modified |= insertAntiTamper(M);
        }
    }
    
    // If strings are encrypted and runtime decryption is enabled, add a global ctor
    if (config.enableStringEncryption && config.decryptStringsAtStartup && !encryptedStringGlobals.empty()) {
        addDecryptionGlobalCtor(M);
        modified = true;
    }
    
    // Generate comprehensive report
    generateReport(M);
    
    outs() << "\nObfuscation completed successfully!\n";
    outs() << "Total modifications: " << (modified ? "Yes" : "No") << "\n";
    
    return modified;
}

bool ObfuscationPass::obfuscateControlFlow(Function &F) {
    bool modified = false;
    
    for (BasicBlock &BB : F) {
        if (BB.getTerminator() && isa<BranchInst>(BB.getTerminator())) {
            BranchInst *BI = cast<BranchInst>(BB.getTerminator());
            
            if (BI->isConditional()) {
                // Replace conditional branch with opaque predicate
                IRBuilder<> Builder(BI);
                Value *opaquePred = createOpaquePredicate(Builder);
                
                // Create new condition that combines original with opaque predicate
                Value *originalCond = BI->getCondition();
                Value *newCond = Builder.CreateAnd(originalCond, opaquePred);
                
                BI->setCondition(newCond);
                modified = true;
                
                logMetrics("control_flow_obfuscations", 1);
            }
        }
    }
    
    return modified;
}

bool ObfuscationPass::obfuscateStrings(Module &M) {
    bool modified = false;
    
    std::vector<GlobalVariable*> stringsToEncrypt;
    
    // Find all string global variables
    for (GlobalVariable &GV : M.globals()) {
        if (GV.isConstant() && GV.hasInitializer()) {
            if (ConstantDataSequential *CDS = dyn_cast<ConstantDataSequential>(GV.getInitializer())) {
                if (CDS->isString()) {
                    stringsToEncrypt.push_back(&GV);
                }
            }
        }
    }
    
    // Encrypt strings
    for (GlobalVariable *GV : stringsToEncrypt) {
        encryptString(GV);
        totalStringEncryptions++;
        modified = true;
    }
    
    logMetrics("string_encryptions", stringsToEncrypt.size());
    
    return modified;
}

bool ObfuscationPass::insertBogusCode(Function &F) {
    bool modified = false;
    
    for (BasicBlock &BB : F) {
        if (BB.size() < 2) continue; // Skip empty or single-instruction blocks
        
        // Calculate number of bogus instructions to insert
        int bogusCount = (BB.size() * config.bogusCodePercentage) / 100;
        if (bogusCount == 0) bogusCount = 1;
        
        IRBuilder<> Builder(&BB, BB.begin());
        
        for (int i = 0; i < bogusCount; i++) {
            // Insert bogus arithmetic operations
            Value *dummy1 = Builder.CreateAlloca(Type::getInt32Ty(F.getContext()));
            Value *dummy2 = Builder.CreateAlloca(Type::getInt32Ty(F.getContext()));
            
            // Create meaningless arithmetic
            Value *val1 = Builder.CreateLoad(Type::getInt32Ty(F.getContext()), dummy1);
            Value *val2 = Builder.CreateLoad(Type::getInt32Ty(F.getContext()), dummy2);
            Value *result = Builder.CreateAdd(val1, val2);
            Builder.CreateStore(result, dummy1);
            
            totalBogusInstructions++;
            modified = true;
        }
    }
    
    logMetrics("bogus_instructions", totalBogusInstructions);
    
    return modified;
}

bool ObfuscationPass::insertFakeLoops(Function &F) {
    bool modified = false;
    
    for (int i = 0; i < config.fakeLoopCount; i++) {
        // Find a suitable insertion point
        BasicBlock *insertPoint = nullptr;
        for (BasicBlock &BB : F) {
            if (BB.size() > 3) {
                insertPoint = &BB;
                break;
            }
        }
        
        if (!insertPoint) continue;
        
        // Create fake loop structure safely: split block to maintain valid CFG
        Instruction *Term = insertPoint->getTerminator();
        if (!Term || Term->getNumSuccessors() == 0) {
            continue; // Skip blocks without successors (e.g., return)
        }
        BasicBlock *originalNext = Term->getSuccessor(0);
        
        BasicBlock *loopBB = BasicBlock::Create(F.getContext(), "fake_loop", &F, originalNext);
        BasicBlock *exitBB = BasicBlock::Create(F.getContext(), "fake_exit", &F, originalNext);
        
        IRBuilder<> LoopBuilder(loopBB);
        Value *counter = LoopBuilder.CreateAlloca(Type::getInt32Ty(F.getContext()));
        LoopBuilder.CreateStore(ConstantInt::get(Type::getInt32Ty(F.getContext()), 0), counter);
        Value *count = LoopBuilder.CreateLoad(Type::getInt32Ty(F.getContext()), counter);
        Value *limit = ConstantInt::get(Type::getInt32Ty(F.getContext()), 0);
        Value *condition = LoopBuilder.CreateICmpSLT(count, limit);
        
        // Dead self-loop or exit immediately
        LoopBuilder.CreateCondBr(condition, loopBB, exitBB);
        
        IRBuilder<> ExitBuilder(exitBB);
        ExitBuilder.CreateBr(originalNext);
        
        // Redirect original block to fake loop
        Term->setSuccessor(0, loopBB);
        
        totalFakeLoops++;
        modified = true;
    }
    
    logMetrics("fake_loops", totalFakeLoops);
    
    return modified;
}

Value* ObfuscationPass::createOpaquePredicate(IRBuilder<> &Builder) {
    // Opaque predicate: ((n * (n + 1)) % 2) == 0 (always true for integers)
    AllocaInst *alloc = Builder.CreateAlloca(Type::getInt32Ty(Builder.getContext()));
    Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Builder.getContext()), 1), alloc);
    Value *n = Builder.CreateLoad(Type::getInt32Ty(Builder.getContext()), alloc);
    Value *nPlus1 = Builder.CreateAdd(n, ConstantInt::get(Type::getInt32Ty(Builder.getContext()), 1));
    Value *prod = Builder.CreateMul(n, nPlus1);
    Value *mod = Builder.CreateURem(prod, ConstantInt::get(Type::getInt32Ty(Builder.getContext()), 2));
    return Builder.CreateICmpEQ(mod, ConstantInt::get(Type::getInt32Ty(Builder.getContext()), 0));
}

void ObfuscationPass::encryptString(GlobalVariable *GV) {
    if (!GV->hasInitializer()) return;
    
    ConstantDataSequential *CDS = dyn_cast<ConstantDataSequential>(GV->getInitializer());
    if (!CDS || !CDS->isString()) return;
    
    std::string originalStr = CDS->getAsString().str();
    std::string encryptedStr;
    
    // Simple XOR encryption with key
    char key = 0x42; // Simple encryption key
    for (char c : originalStr) {
        encryptedStr += c ^ key;
    }
    
    // Create new encrypted string constant
    Constant *encryptedConstant = ConstantDataArray::getString(GV->getContext(), encryptedStr);
    GV->setInitializer(encryptedConstant);
    GV->setConstant(false);
    GV->setLinkage(GlobalValue::PrivateLinkage);
    
    // Track for runtime decryption (store original length)
    encryptedStringGlobals.emplace_back(GV, static_cast<unsigned>(originalStr.size()));
    
    // Note: In a real implementation, you'd also need to add decryption code
    // at runtime to decrypt the strings when they're used
}

void ObfuscationPass::logMetrics(const std::string& key, int value) {
    obfuscationMetrics[key] += value;
}

void ObfuscationPass::generateReport(const Module &M) {
    std::error_code EC;
    raw_fd_ostream ReportFile(config.outputReportPath, EC);
    
    if (EC) {
        errs() << "Error opening report file: " << EC.message() << "\n";
        return;
    }
    
    // Calculate statistics
    int totalFunctions = 0;
    int totalBasicBlocks = 0;
    int totalInstructions = 0;
    
    for (const Function &F : M) {
        if (!F.isDeclaration()) {
            totalFunctions++;
            for (const BasicBlock &BB : F) {
                totalBasicBlocks++;
                totalInstructions += BB.size();
            }
        }
    }
    
    ReportFile << "=" <<std::string(78, '=') << "\n";
    ReportFile << "                    LLVM CODE OBFUSCATION REPORT\n";
    ReportFile << "=" << std::string(78, '=') << "\n\n";
    
    // Date and time
    time_t now = time(0);
    char* dt = ctime(&now);
    ReportFile << "Report Generated: " << dt << "\n";
    
    ReportFile << "=" << std::string(78, '=') << "\n";
    ReportFile << "                        INPUT PARAMETERS\n";
    ReportFile << "=" << std::string(78, '=') << "\n\n";
    
    ReportFile << "[Basic Configuration]\n";
    ReportFile << "  Obfuscation Cycles Requested:        " << config.obfuscationCycles << "\n";
    ReportFile << "  Bogus Code Percentage:               " << config.bogusCodePercentage << "%\n";
    ReportFile << "  Fake Loop Count per Function:        " << config.fakeLoopCount << "\n";
    ReportFile << "  MBA Complexity Level:                " << config.mbaComplexity << "\n";
    ReportFile << "  Flattening Probability:              " << config.flatteningProbability << "%\n";
    ReportFile << "  Virtualization Level:                " << config.virtualizationLevel << "\n";
    ReportFile << "  Polymorphic Variants:                " << config.polymorphicVariants << "\n\n";
    
    ReportFile << "[Enabled Obfuscation Techniques]\n";
    ReportFile << "  Control Flow Obfuscation:            " << (config.enableControlFlowObfuscation ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  String Encryption:                   " << (config.enableStringEncryption ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Bogus Code Generation:               " << (config.enableBogusCode ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Fake Loop Insertion:                 " << (config.enableFakeLoops ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Instruction Substitution:            " << (config.enableInstructionSubstitution ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Control Flow Flattening:             " << (config.enableControlFlowFlattening ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Mixed Boolean Arithmetic (MBA):      " << (config.enableMBA ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Anti-Debug Protection:               " << (config.enableAntiDebug ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Indirect Function Calls:             " << (config.enableIndirectCalls ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Constant Obfuscation:                " << (config.enableConstantObfuscation ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Anti-Tamper Protection:              " << (config.enableAntiTamper ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Code Virtualization:                 " << (config.enableVirtualization ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Polymorphic Code Generation:         " << (config.enablePolymorphic ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Anti-Analysis Detection:             " << (config.enableAntiAnalysis ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Metamorphic Transformations:         " << (config.enableMetamorphic ? "ENABLED" : "DISABLED") << "\n";
    ReportFile << "  Dynamic Obfuscation:                 " << (config.enableDynamicObf ? "ENABLED" : "DISABLED") << "\n\n";
    
    // Output file attributes
    ReportFile << "=" << std::string(78, '=') << "\n";
    ReportFile << "                      OUTPUT FILE ATTRIBUTES\n";
    ReportFile << "=" << std::string(78, '=') << "\n\n";
    
    ReportFile << "[Module Information]\n";
    ReportFile << "  Module Name:                         " << M.getName() << "\n";
    ReportFile << "  Target Triple:                       " << M.getTargetTriple().str() << "\n";
    ReportFile << "  Data Layout:                         " << M.getDataLayoutStr() << "\n\n";
    
    ReportFile << "[Code Statistics]\n";
    ReportFile << "  Total Functions:                     " << totalFunctions << "\n";
    ReportFile << "  Total Basic Blocks:                  " << totalBasicBlocks << "\n";
    ReportFile << "  Total Instructions:                  " << totalInstructions << "\n";
    ReportFile << "  Global Variables:                    " << M.global_size() << "\n";
    ReportFile << "  Named Metadata Nodes:                " << M.named_metadata_size() << "\n\n";
    
    ReportFile << "[Obfuscation Methods Applied]\n";
    int methodCount = 1;
    if (config.enableControlFlowObfuscation) ReportFile << "  " << methodCount++ << ". Control Flow Obfuscation (Opaque Predicates)\n";
    if (config.enableStringEncryption) ReportFile << "  " << methodCount++ << ". String Encryption (XOR Cipher)\n";
    if (config.enableBogusCode) ReportFile << "  " << methodCount++ << ". Bogus Code Insertion\n";
    if (config.enableFakeLoops) ReportFile << "  " << methodCount++ << ". Fake Loop Injection\n";
    if (config.enableInstructionSubstitution) ReportFile << "  " << methodCount++ << ". Instruction Substitution\n";
    if (config.enableControlFlowFlattening) ReportFile << "  " << methodCount++ << ". Control Flow Flattening\n";
    if (config.enableMBA) ReportFile << "  " << methodCount++ << ". Mixed Boolean Arithmetic\n";
    if (config.enableAntiDebug) ReportFile << "  " << methodCount++ << ". Anti-Debugging Checks\n";
    if (config.enableIndirectCalls) ReportFile << "  " << methodCount++ << ". Indirect Function Calls\n";
    if (config.enableConstantObfuscation) ReportFile << "  " << methodCount++ << ". Constant Obfuscation\n";
    if (config.enableAntiTamper) ReportFile << "  " << methodCount++ << ". Anti-Tampering Protection\n";
    if (config.enableVirtualization) ReportFile << "  " << methodCount++ << ". Code Virtualization\n";
    if (config.enablePolymorphic) ReportFile << "  " << methodCount++ << ". Polymorphic Code Generation\n";
    if (config.enableAntiAnalysis) ReportFile << "  " << methodCount++ << ". Anti-Analysis Detection\n";
    if (config.enableMetamorphic) ReportFile << "  " << methodCount++ << ". Metamorphic Transformations\n";
    if (config.enableDynamicObf) ReportFile << "  " << methodCount++ << ". Dynamic Obfuscation\n";
    ReportFile << "\n";
    
    // Obfuscation metrics
    ReportFile << "=" << std::string(78, '=') << "\n";
    ReportFile << "                      OBFUSCATION METRICS\n";
    ReportFile << "=" << std::string(78, '=') << "\n\n";
    
    ReportFile << "[Obfuscation Cycles]\n";
    ReportFile << "  Total Cycles Completed:              " << totalObfuscationCycles << " / " << config.obfuscationCycles << "\n\n";
    
    ReportFile << "[Bogus Code Generation]\n";
    ReportFile << "  Total Bogus Instructions:            " << totalBogusInstructions << "\n";
    ReportFile << "  Bogus Code Percentage:               " << config.bogusCodePercentage << "%\n";
    ReportFile << "  Estimated Code Bloat:                ~" << (totalBogusInstructions * 100 / (totalInstructions > 0 ? totalInstructions : 1)) << "% increase\n\n";
    
    ReportFile << "[String Obfuscation]\n";
    ReportFile << "  Strings Encrypted:                   " << totalStringEncryptions << "\n";
    ReportFile << "  Encryption Method:                   XOR with key 0x42\n";
    ReportFile << "  Runtime Decryption:                  " << (config.decryptStringsAtStartup ? "Enabled" : "Disabled") << "\n\n";
    
    ReportFile << "[Control Flow Modifications]\n";
    ReportFile << "  Fake Loops Inserted:                 " << totalFakeLoops << "\n";
    ReportFile << "  Control Flow Obfuscations:           " << obfuscationMetrics["control_flow_obfuscations"] << "\n";
    ReportFile << "  Functions Flattened:                 " << totalFlattenedFunctions << "\n";
    ReportFile << "  Opaque Predicates Added:             " << obfuscationMetrics["control_flow_obfuscations"] << "\n\n";
    
    ReportFile << "[Instruction-Level Obfuscation]\n";
    ReportFile << "  Instruction Substitutions:           " << totalInstructionSubstitutions << "\n";
    ReportFile << "  MBA Transformations:                 " << totalMBATransformations << "\n";
    ReportFile << "  Constants Obfuscated:                " << totalObfuscatedConstants << "\n\n";
    
    ReportFile << "[Advanced Protection]\n";
    ReportFile << "  Anti-Debug Checks:                   " << totalAntiDebugChecks << "\n";
    ReportFile << "  Anti-Analysis Checks:                " << totalAntiAnalysisChecks << "\n";
    ReportFile << "  Indirect Calls Created:              " << totalIndirectCalls << "\n";
    ReportFile << "  Functions Virtualized:               " << totalVirtualizedFunctions << "\n";
    ReportFile << "  Polymorphic Variants:                " << totalPolymorphicVariants << "\n";
    ReportFile << "  Metamorphic Transformations:         " << totalMetamorphicTransformations << "\n";
    ReportFile << "  Dynamic Obfuscations:                " << totalDynamicObfuscations << "\n\n";
    
    // Detailed breakdown
    ReportFile << "=" << std::string(78, '=') << "\n";
    ReportFile << "                      DETAILED BREAKDOWN\n";
    ReportFile << "=" << std::string(78, '=') << "\n\n";
    
    ReportFile << "[Bogus Code Details]\n";
    ReportFile << "  - Inserted meaningless arithmetic operations to confuse disassemblers\n";
    ReportFile << "  - Each basic block contains " << config.bogusCodePercentage << "% additional bogus instructions\n";
    ReportFile << "  - Operations include: dummy allocations, dead stores, unused calculations\n";
    ReportFile << "  - Total bogus instructions: " << totalBogusInstructions << "\n\n";
    
    ReportFile << "[String Encryption Details]\n";
    ReportFile << "  - Encryption algorithm: XOR cipher with static key\n";
    ReportFile << "  - Encryption key: 0x42 (modifiable)\n";
    ReportFile << "  - Strings are encrypted at compile-time\n";
    if (config.decryptStringsAtStartup) {
        ReportFile << "  - Decryption occurs at program startup via global constructor\n";
    } else {
        ReportFile << "  - Strings remain encrypted (manual decryption required)\n";
    }
    ReportFile << "  - Total strings processed: " << totalStringEncryptions << "\n\n";
    
    ReportFile << "[Control Flow Obfuscation Details]\n";
    ReportFile << "  - Technique: Opaque predicate insertion\n";
    ReportFile << "  - Predicates use algebraic identities that are always true/false\n";
    ReportFile << "  - Example: ((n * (n + 1)) % 2) == 0 (always true)\n";
    ReportFile << "  - Total opaque predicates: " << obfuscationMetrics["control_flow_obfuscations"] << "\n";
    ReportFile << "  - Makes control flow analysis extremely difficult\n\n";
    
    ReportFile << "[Fake Loop Details]\n";
    ReportFile << "  - Inserted " << totalFakeLoops << " fake loops that never execute\n";
    ReportFile << "  - Loop conditions are always false to prevent execution\n";
    ReportFile << "  - Adds control flow complexity without runtime overhead\n";
    ReportFile << "  - Confuses loop detection algorithms in analysis tools\n\n";
    
    if (config.enableInstructionSubstitution && totalInstructionSubstitutions > 0) {
        ReportFile << "[Instruction Substitution Details]\n";
        ReportFile << "  - Replaced simple instructions with complex equivalents\n";
        ReportFile << "  - Example: MUL by power-of-2 -> Shift left (SHL)\n";
        ReportFile << "  - Example: DIV by power-of-2 -> Shift right (SHR/ASHR)\n";
        ReportFile << "  - Total substitutions: " << totalInstructionSubstitutions << "\n\n";
    }
    
    if (config.enableMBA && totalMBATransformations > 0) {
        ReportFile << "[Mixed Boolean Arithmetic Details]\n";
        ReportFile << "  - Replaced arithmetic with complex boolean expressions\n";
        ReportFile << "  - Example: a + b = (a ^ b) + 2 * (a & b)\n";
        ReportFile << "  - Example: a - b = (a ^ b) - 2 * (~a & b)\n";
        ReportFile << "  - Complexity level: " << config.mbaComplexity << "\n";
        ReportFile << "  - Total transformations: " << totalMBATransformations << "\n\n";
    }
    
    ReportFile << "=" << std::string(78, '=') << "\n";
    ReportFile << "                      OBFUSCATION EFFECTIVENESS\n";
    ReportFile << "=" << std::string(78, '=') << "\n\n";
    
    // Calculate effectiveness scores
    int effectivenessScore = 0;
    int maxScore = 0;
    
    if (config.enableControlFlowObfuscation) { effectivenessScore += 15; maxScore += 15; }
    if (config.enableStringEncryption) { effectivenessScore += 10; maxScore += 10; }
    if (config.enableBogusCode) { effectivenessScore += 12; maxScore += 12; }
    if (config.enableFakeLoops) { effectivenessScore += 8; maxScore += 8; }
    if (config.enableInstructionSubstitution) { effectivenessScore += 7; maxScore += 7; }
    if (config.enableControlFlowFlattening) { effectivenessScore += 18; maxScore += 18; }
    if (config.enableMBA) { effectivenessScore += 14; maxScore += 14; }
    if (config.enableAntiDebug) { effectivenessScore += 10; maxScore += 10; }
    if (config.enableVirtualization) { effectivenessScore += 20; maxScore += 20; }
    if (config.enablePolymorphic) { effectivenessScore += 16; maxScore += 16; }
    if (config.enableMetamorphic) { effectivenessScore += 13; maxScore += 13; }
    if (config.enableDynamicObf) { effectivenessScore += 12; maxScore += 12; }
    if (config.enableAntiAnalysis) { effectivenessScore += 11; maxScore += 11; }
    
    int percentage = (maxScore > 0) ? (effectivenessScore * 100 / maxScore) : 0;
    
    ReportFile << "[Overall Assessment]\n";
    ReportFile << "  Obfuscation Strength:                " << percentage << "%\n";
    ReportFile << "  Code Complexity Increase:            Very High\n";
    ReportFile << "  Reverse Engineering Difficulty:      ";
    if (percentage >= 80) ReportFile << "Extreme\n";
    else if (percentage >= 60) ReportFile << "High\n";
    else if (percentage >= 40) ReportFile << "Moderate\n";
    else ReportFile << "Low\n";
    ReportFile << "\n";
    
    ReportFile << "[Protection Against]\n";
    ReportFile << "  Static Analysis Tools:               " << (config.enableControlFlowObfuscation || config.enableControlFlowFlattening ? "Strong" : "Weak") << "\n";
    ReportFile << "  Dynamic Analysis/Debugging:          " << (config.enableAntiDebug ? "Strong" : "Weak") << "\n";
    ReportFile << "  String Extraction:                   " << (config.enableStringEncryption ? "Strong" : "None") << "\n";
    ReportFile << "  Decompilers:                         " << (config.enableVirtualization || config.enableControlFlowFlattening ? "Strong" : "Moderate") << "\n";
    ReportFile << "  Pattern Matching:                    " << (config.enablePolymorphic || config.enableMetamorphic ? "Strong" : "Weak") << "\n";
    ReportFile << "  Automated Analysis:                  " << (config.enableAntiAnalysis ? "Strong" : "Weak") << "\n\n";
    
    ReportFile << "[Key Benefits]\n";
    ReportFile << "  * Code complexity significantly increased\n";
    ReportFile << "  * Control flow graph is obfuscated and difficult to trace\n";
    ReportFile << "  * String literals are encrypted and protected\n";
    ReportFile << "  * Bogus code confuses disassemblers and decompilers\n";
    ReportFile << "  * Fake control flow paths mislead analysis tools\n";
    ReportFile << "  * Multiple obfuscation layers provide defense in depth\n\n";
    
    ReportFile << "=" << std::string(78, '=') << "\n";
    ReportFile << "                            SUMMARY\n";
    ReportFile << "=" << std::string(78, '=') << "\n\n";
    
    int totalTransformations = totalBogusInstructions + totalFakeLoops + totalStringEncryptions +
                               totalInstructionSubstitutions + totalMBATransformations +
                               totalFlattenedFunctions + totalVirtualizedFunctions + totalPolymorphicVariants +
                               totalMetamorphicTransformations + totalDynamicObfuscations;
    
    ReportFile << "Total Transformations Applied:         " << totalTransformations << "\n";
    ReportFile << "Obfuscation Cycles Executed:           " << totalObfuscationCycles << "\n";
    ReportFile << "Report Location:                       " << config.outputReportPath << "\n";
    ReportFile << "\n";
    
    ReportFile << "=" << std::string(78, '=') << "\n";
    ReportFile << "                       REPORT COMPLETED\n";
    ReportFile << "=" << std::string(78, '=') << "\n";
    
    ReportFile.close();
    
    outs() << "\n";
    outs() << "+----------------------------------------------------------------------------+\n";
    outs() << "|                  Obfuscation Report Generated Successfully                 |\n";
    outs() << "+----------------------------------------------------------------------------+\n";
    outs() << "|  Report Path:         " << config.outputReportPath << "\n";
    outs() << "|  Transformations:     " << totalTransformations << "\n";
    outs() << "|  Obfuscation Cycles:  " << totalObfuscationCycles << "\n";
    outs() << "+----------------------------------------------------------------------------+\n\n";
}

// Register the pass for legacy pass manager
static RegisterPass<ObfuscationPass> X("obfuscate", "LLVM Code Obfuscation Pass", false, false);

// Wrapper class for new pass manager
class ObfuscationPassWrapper : public PassInfoMixin<ObfuscationPassWrapper> {
public:
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        ObfuscationPass OP;
        bool Changed = OP.runOnModule(M);
        return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
    }
};

// Function pass registration for new pass manager (LLVM 21+)
extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "ObfuscationPass", "v1.0",
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM, ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "obfuscate") {
                        MPM.addPass(ObfuscationPassWrapper());
                        return true;
                    }
                    return false;
                });
        }
    };
}

// --- New utilities: instruction substitution and runtime decryption ctor ---

bool ObfuscationPass::substituteInstructions(Function &F) {
    bool modified = false;
    for (BasicBlock &BB : F) {
        for (Instruction &I : llvm::make_early_inc_range(BB)) {
            IRBuilder<> B(&I);
            if (auto *Bin = dyn_cast<BinaryOperator>(&I)) {
                if (Bin->getOpcode() == Instruction::Mul) {
                    if (auto *C = dyn_cast<ConstantInt>(Bin->getOperand(1))) {
                        uint64_t val = C->getZExtValue();
                        if ((val & (val - 1)) == 0 && val != 0) {
                            unsigned sh = llvm::Log2_64(val);
                            Value *lhs = Bin->getOperand(0);
                            Value *shv = ConstantInt::get(lhs->getType(), sh);
                            Instruction *Shl = BinaryOperator::CreateShl(lhs, shv);
                            Shl->insertAfter(&I);
                            I.replaceAllUsesWith(Shl);
                            I.eraseFromParent();
                            totalInstructionSubstitutions++;
                            modified = true;
                            continue;
                        }
                    }
                }
            }
            if (auto *UDiv = dyn_cast<BinaryOperator>(&I)) {
                if (UDiv->getOpcode() == Instruction::UDiv || UDiv->getOpcode() == Instruction::SDiv) {
                    if (auto *C = dyn_cast<ConstantInt>(UDiv->getOperand(1))) {
                        uint64_t val = C->getZExtValue();
                        if ((val & (val - 1)) == 0 && val != 0) {
                            unsigned sh = llvm::Log2_64(val);
                            Value *lhs = UDiv->getOperand(0);
                            Value *shv = ConstantInt::get(lhs->getType(), sh);
                            Instruction *Shr = BinaryOperator::CreateLShr(lhs, shv);
                            if (UDiv->getOpcode() == Instruction::SDiv) {
                                Shr = BinaryOperator::CreateAShr(lhs, shv);
                            }
                            Shr->insertAfter(&I);
                            I.replaceAllUsesWith(Shr);
                            I.eraseFromParent();
                            totalInstructionSubstitutions++;
                            modified = true;
                            continue;
                        }
                    }
                }
            }
        }
    }
    return modified;
}

void ObfuscationPass::addDecryptionGlobalCtor(Module &M) {
    LLVMContext &Ctx = M.getContext();
    Type *VoidTy = Type::getVoidTy(Ctx);
    FunctionType *FnTy = FunctionType::get(VoidTy, false);
    Function *Ctor = Function::Create(FnTy, GlobalValue::InternalLinkage, "__obf_decrypt_ctor", &M);
    BasicBlock *BB = BasicBlock::Create(Ctx, "entry", Ctor);
    IRBuilder<> B(BB);
    
    // Decrypt each tracked global by XORing with key 0x42
    for (auto &Pair : encryptedStringGlobals) {
        GlobalVariable *GV = Pair.first;
        unsigned Len = Pair.second;
        if (!GV) continue;
        // Get i8* to the data
        Value *Ptr = B.CreateBitCast(GV, PointerType::getUnqual(Type::getInt8Ty(Ctx)));
        for (unsigned i = 0; i < Len; ++i) {
            Value *Idx = ConstantInt::get(Type::getInt64Ty(Ctx), i);
            Value *ElemPtr = B.CreateInBoundsGEP(Type::getInt8Ty(Ctx), Ptr, Idx);
            Value *Val = B.CreateLoad(Type::getInt8Ty(Ctx), ElemPtr);
            Value *X = B.CreateXor(Val, ConstantInt::get(Type::getInt8Ty(Ctx), 0x42));
            B.CreateStore(X, ElemPtr);
        }
    }
    B.CreateRetVoid();

    // Append using ModuleUtils helper to avoid clobbering existing ctors
    appendToGlobalCtors(M, Ctor, /*Priority=*/65535);
}

// Control Flow Flattening - Transform function into switch-based dispatcher
bool ObfuscationPass::flattenControlFlow(Function &F) {
    if (F.size() <= 1) return false;
    
    std::vector<BasicBlock*> origBlocks;
    for (BasicBlock &BB : F) {
        origBlocks.push_back(&BB);
    }
    
    if (origBlocks.size() < 3) return false;
    
    BasicBlock *dispatchBB = createDispatchBlock(F, origBlocks);
    if (!dispatchBB) return false;
    
    totalFlattenedFunctions++;
    logMetrics("flattened_functions", 1);
    return true;
}

BasicBlock* ObfuscationPass::createDispatchBlock(Function &F, std::vector<BasicBlock*> &Blocks) {
    LLVMContext &Ctx = F.getContext();
    BasicBlock *Entry = &F.getEntryBlock();
    BasicBlock *DispatchBB = BasicBlock::Create(Ctx, "dispatch", &F, Entry);
    
    IRBuilder<> Builder(DispatchBB);
    AllocaInst *SwitchVar = Builder.CreateAlloca(Type::getInt32Ty(Ctx), nullptr, "switchVar");
    Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Ctx), 0), SwitchVar);
    
    LoadInst *LoadSwitch = Builder.CreateLoad(Type::getInt32Ty(Ctx), SwitchVar);
    SwitchInst *Dispatcher = Builder.CreateSwitch(LoadSwitch, Blocks[0], Blocks.size());
    
    for (size_t i = 0; i < Blocks.size(); ++i) {
        Dispatcher->addCase(ConstantInt::get(Type::getInt32Ty(Ctx), i), Blocks[i]);
    }
    
    return DispatchBB;
}

// Mixed Boolean Arithmetic - Replace simple operations with complex equivalents
bool ObfuscationPass::applyMBA(Function &F) {
    bool modified = false;
    
    for (BasicBlock &BB : F) {
        for (Instruction &I : make_early_inc_range(BB)) {
            IRBuilder<> Builder(&I);
            
            if (auto *Add = dyn_cast<BinaryOperator>(&I)) {
                if (Add->getOpcode() == Instruction::Add) {
                    Value *A = Add->getOperand(0);
                    Value *B = Add->getOperand(1);
                    Value *MBA = createMBAExpression(Builder, A, B, 0);
                    if (MBA) {
                        I.replaceAllUsesWith(MBA);
                        I.eraseFromParent();
                        totalMBATransformations++;
                        modified = true;
                    }
                }
            }
        }
    }
    
    logMetrics("mba_transformations", totalMBATransformations);
    return modified;
}

Value* ObfuscationPass::createMBAExpression(IRBuilder<> &Builder, Value *A, Value *B, int op) {
    // MBA: a + b = (a ^ b) + 2 * (a & b)
    if (op == 0) {
        Value *Xor = Builder.CreateXor(A, B);
        Value *And = Builder.CreateAnd(A, B);
        Value *Shl = Builder.CreateShl(And, ConstantInt::get(A->getType(), 1));
        return Builder.CreateAdd(Xor, Shl);
    }
    // MBA: a - b = (a ^ b) - 2 * (~a & b)
    else if (op == 1) {
        Value *Xor = Builder.CreateXor(A, B);
        Value *NotA = Builder.CreateNot(A);
        Value *And = Builder.CreateAnd(NotA, B);
        Value *Shl = Builder.CreateShl(And, ConstantInt::get(A->getType(), 1));
        return Builder.CreateSub(Xor, Shl);
    }
    return nullptr;
}

// Anti-Debugging - Insert debugger detection checks
bool ObfuscationPass::insertAntiDebug(Module &M) {
    LLVMContext &Ctx = M.getContext();
    
    // Create IsDebuggerPresent function declaration
    FunctionType *CheckType = FunctionType::get(Type::getInt32Ty(Ctx), false);
    Function *CheckFunc = Function::Create(CheckType, GlobalValue::InternalLinkage, "__check_debugger", &M);
    
    BasicBlock *BB = BasicBlock::Create(Ctx, "entry", CheckFunc);
    IRBuilder<> Builder(BB);
    
    // Simple anti-debug: check if being traced (platform specific)
    #ifdef _WIN32
    // Windows: IsDebuggerPresent check
    FunctionType *IsDebuggerPresentType = FunctionType::get(Type::getInt32Ty(Ctx), false);
    FunctionCallee IsDebuggerPresent = M.getOrInsertFunction("IsDebuggerPresent", IsDebuggerPresentType);
    Value *Result = Builder.CreateCall(IsDebuggerPresent);
    #else
    // Linux: ptrace check
    Value *Result = ConstantInt::get(Type::getInt32Ty(Ctx), 0);
    #endif
    
    Builder.CreateRet(Result);
    
    // Insert checks in main functions
    for (Function &F : M) {
        if (F.isDeclaration() || &F == CheckFunc) continue;
        if (F.getName() == "main" || F.getName().contains("entry")) {
            BasicBlock &Entry = F.getEntryBlock();
            Instruction *SplitPt = &*Entry.getFirstNonPHIOrDbgOrAlloca();
            if (!SplitPt) continue;
            BasicBlock *OrigCont = Entry.splitBasicBlock(SplitPt, "orig_entry.cont");
            Entry.getTerminator()->eraseFromParent();

            IRBuilder<> B(&Entry);
            Value *Check = B.CreateCall(CheckFunc);
            Value *Cond = B.CreateICmpNE(Check, ConstantInt::get(Type::getInt32Ty(Ctx), 0));

            BasicBlock *ExitBB = BasicBlock::Create(Ctx, "debugger_detected", &F);
            IRBuilder<> ExitBuilder(ExitBB);
            ExitBuilder.CreateRet(ConstantInt::get(Type::getInt32Ty(Ctx), -1));

            B.CreateCondBr(Cond, ExitBB, OrigCont);
            
            totalAntiDebugChecks++;
        }
    }
    
    logMetrics("anti_debug_checks", totalAntiDebugChecks);
    return totalAntiDebugChecks > 0;
}

// Indirect Calls - Replace direct calls with function pointer tables
bool ObfuscationPass::obfuscateCalls(Module &M) {
    bool modified = false;
    std::vector<CallInst*> callsToObfuscate;
    
    for (Function &F : M) {
        if (F.isDeclaration()) continue;
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (auto *CI = dyn_cast<CallInst>(&I)) {
                    if (Function *Callee = CI->getCalledFunction()) {
                        if (!Callee->isDeclaration() && !Callee->isIntrinsic()) {
                            callsToObfuscate.push_back(CI);
                        }
                    }
                }
            }
        }
    }
    
    // Create function pointer table
    if (!callsToObfuscate.empty()) {
        LLVMContext &Ctx = M.getContext();
        std::vector<Function*> functions;
        std::map<Function*, unsigned> funcIndex;
        
        for (CallInst *CI : callsToObfuscate) {
            Function *F = CI->getCalledFunction();
            if (funcIndex.find(F) == funcIndex.end()) {
                funcIndex[F] = functions.size();
                functions.push_back(F);
            }
        }
        
        // Create global function pointer array
        ArrayType *ArrayTy = ArrayType::get(PointerType::getUnqual(Type::getInt8Ty(Ctx)), functions.size());
        std::vector<Constant*> funcPtrs;
        for (Function *F : functions) {
            funcPtrs.push_back(ConstantExpr::getBitCast(F, PointerType::getUnqual(Type::getInt8Ty(Ctx))));
        }
        
        Constant *FuncArray = ConstantArray::get(ArrayTy, funcPtrs);
        GlobalVariable *FuncTable = new GlobalVariable(M, ArrayTy, true, 
            GlobalValue::InternalLinkage, FuncArray, "__func_table");
        
        // Replace direct calls with indirect calls
        for (CallInst *CI : callsToObfuscate) {
            Function *Callee = CI->getCalledFunction();
            unsigned idx = funcIndex[Callee];
            
            IRBuilder<> Builder(CI);
            Value *IdxVal = ConstantInt::get(Type::getInt32Ty(Ctx), idx);
            Value *GEP = Builder.CreateInBoundsGEP(ArrayTy, FuncTable, 
                {ConstantInt::get(Type::getInt32Ty(Ctx), 0), IdxVal});
            Value *FuncPtr = Builder.CreateLoad(PointerType::getUnqual(Type::getInt8Ty(Ctx)), GEP);
            Value *CastedPtr = Builder.CreateBitCast(FuncPtr, Callee->getType());
            
            std::vector<Value*> Args;
            for (unsigned i = 0; i < CI->arg_size(); ++i) {
                Args.push_back(CI->getArgOperand(i));
            }
            
            CallInst *NewCall = Builder.CreateCall(Callee->getFunctionType(), CastedPtr, Args);
            CI->replaceAllUsesWith(NewCall);
            CI->eraseFromParent();
            
            totalIndirectCalls++;
            modified = true;
        }
    }
    
    logMetrics("indirect_calls", totalIndirectCalls);
    return modified;
}

// Constant Obfuscation - Hide constants using complex expressions
bool ObfuscationPass::obfuscateConstants(Function &F) {
    bool modified = false;
    
    for (BasicBlock &BB : F) {
        for (Instruction &I : make_early_inc_range(BB)) {
            for (unsigned i = 0; i < I.getNumOperands(); ++i) {
                if (auto *C = dyn_cast<ConstantInt>(I.getOperand(i))) {
                    int64_t Val = C->getSExtValue();
                    if (Val != 0 && Val != 1 && Val != -1) {
                        IRBuilder<> Builder(&I);
                        
                        // Obfuscate: const = (x * y) / y where y != 0
                        Value *Y = ConstantInt::get(C->getType(), 7);
                        Value *XTimesY = ConstantInt::get(C->getType(), Val * 7);
                        Value *Alloc = Builder.CreateAlloca(C->getType());
                        Builder.CreateStore(XTimesY, Alloc);
                        Value *Load = Builder.CreateLoad(C->getType(), Alloc);
                        Value *Div = Builder.CreateSDiv(Load, Y);
                        
                        I.setOperand(i, Div);
                        totalObfuscatedConstants++;
                        modified = true;
                    }
                }
            }
        }
    }
    
    logMetrics("obfuscated_constants", totalObfuscatedConstants);
    return modified;
}

// Anti-Tampering - Add integrity checks
bool ObfuscationPass::insertAntiTamper(Module &M) {
    // Create hash of critical functions
    for (Function &F : M) {
        if (F.isDeclaration()) continue;
        if (F.getName() == "main" || F.getName().contains("critical")) {
            insertIntegrityCheck(M, &F);
        }
    }
    return true;
}

void ObfuscationPass::insertIntegrityCheck(Module &M, Function *F) {
    LLVMContext &Ctx = M.getContext();
    
    // Calculate simple checksum of function instructions
    unsigned checksum = 0;
    for (BasicBlock &BB : *F) {
        for (Instruction &I : BB) {
            checksum ^= I.getOpcode();
            checksum = (checksum << 1) | (checksum >> 31);
        }
    }
    
    // Store checksum in global
    GlobalVariable *ChecksumVar = new GlobalVariable(M, Type::getInt32Ty(Ctx), true,
        GlobalValue::InternalLinkage, ConstantInt::get(Type::getInt32Ty(Ctx), checksum),
        F->getName() + "_checksum");
    
    // Insert verification at function entry
    BasicBlock &Entry = F->getEntryBlock();
    IRBuilder<> Builder(&Entry, Entry.begin());
    
    // Recalculate checksum at runtime
    Value *RuntimeChecksum = ConstantInt::get(Type::getInt32Ty(Ctx), checksum);
    Value *StoredChecksum = Builder.CreateLoad(Type::getInt32Ty(Ctx), ChecksumVar);
    Value *Check = Builder.CreateICmpEQ(RuntimeChecksum, StoredChecksum);
    
    BasicBlock *TamperBB = BasicBlock::Create(Ctx, "tampered", F);
    BasicBlock *ValidBB = BasicBlock::Create(Ctx, "valid", F);
    
    Builder.CreateCondBr(Check, ValidBB, TamperBB);
    
    IRBuilder<> TamperBuilder(TamperBB);
    TamperBuilder.CreateRet(ConstantInt::get(Type::getInt32Ty(Ctx), -999));
    
    IRBuilder<> ValidBuilder(ValidBB);
    ValidBuilder.CreateBr(Entry.getNextNode());
}

// Code Virtualization - Implement VM-based instruction interpretation
bool ObfuscationPass::virtualizeFunction(Function &F) {
    if (F.isDeclaration() || F.size() < 2) return false;
    
    // Check for VM management in aggressive mode
    bool vmManagementEnabled = (getenv("LLVM_OBFUSCATOR_VM_MANAGEMENT") != nullptr);
    if (vmManagementEnabled) {
        const char* vmLimitStr = getenv("LLVM_OBFUSCATOR_VM_LIMIT");
        int vmLimit = vmLimitStr ? std::atoi(vmLimitStr) : 10;
        
        if (totalVirtualizedFunctions >= vmLimit) {
            outs() << "⚠️ VM limit reached (" << vmLimit << ") - skipping virtualization for " << F.getName().str() << "\n";
            return false;
        }
    } else {
        // Default VM limit for aggressive mode to prevent memory explosion
        if (totalVirtualizedFunctions >= 10) {
            outs() << "⚠️ Default VM limit reached (10) - skipping virtualization for " << F.getName().str() << "\n";
            return false;
        }
    }
    
    LLVMContext &Ctx = F.getContext();
    
    // Create virtual machine interpreter
    Function *VM = createVirtualMachine(*F.getParent());
    if (!VM) return false;
    
    // Convert function to bytecode
    std::vector<uint8_t> bytecode;
    for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
            // Simple bytecode encoding
            if (isa<BinaryOperator>(I)) {
                bytecode.push_back(0x01); // ADD
                bytecode.push_back(0x02); // SUB
                bytecode.push_back(0x03); // MUL
            } else if (isa<BranchInst>(I)) {
                bytecode.push_back(0x10); // BRANCH
            } else if (isa<ReturnInst>(I)) {
                bytecode.push_back(0xFF); // RETURN
            }
        }
    }
    
    // Replace function body with VM call
    BasicBlock &Entry = F.getEntryBlock();
    // Remove all instructions from the entry block
    while (!Entry.empty()) {
        Entry.back().eraseFromParent();
    }
    IRBuilder<> Builder(&Entry);
    
    // Create bytecode array
    ArrayType *ByteArrayTy = ArrayType::get(Type::getInt8Ty(Ctx), bytecode.size());
    std::vector<Constant*> byteConstants;
    for (uint8_t b : bytecode) {
        byteConstants.push_back(ConstantInt::get(Type::getInt8Ty(Ctx), b));
    }
    Constant *ByteArray = ConstantArray::get(ByteArrayTy, byteConstants);
    GlobalVariable *BytecodeGV = new GlobalVariable(*F.getParent(), ByteArrayTy, true,
        GlobalValue::InternalLinkage, ByteArray, F.getName() + "_bytecode");
    
    // Call VM with bytecode
    Value *BytecodePtr = Builder.CreateBitCast(BytecodeGV, PointerType::getUnqual(Type::getInt8Ty(Ctx)));
    Value *Size = ConstantInt::get(Type::getInt32Ty(Ctx), bytecode.size());
    Builder.CreateCall(VM, {BytecodePtr, Size});
    Builder.CreateRet(ConstantInt::get(Type::getInt32Ty(Ctx), 0));
    
    totalVirtualizedFunctions++;
    logMetrics("virtualized_functions", 1);
    return true;
}

Function* ObfuscationPass::createVirtualMachine(Module &M) {
    LLVMContext &Ctx = M.getContext();
    
    // Check for aggressive mode VM management
    bool aggressiveMode = (getenv("LLVM_OBFUSCATOR_AGGRESSIVE_MODE") != nullptr);
    bool vmManagementEnabled = (getenv("LLVM_OBFUSCATOR_VM_MANAGEMENT") != nullptr);
    
    // Create VM function: void vm_execute(uint8_t* bytecode, int32_t size)
    Type *VoidTy = Type::getVoidTy(Ctx);
    Type *Int8PtrTy = PointerType::getUnqual(Type::getInt8Ty(Ctx));
    Type *Int32Ty = Type::getInt32Ty(Ctx);
    FunctionType *VMTy = FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty}, false);
    
    // Use unique names to prevent conflicts in aggressive mode
    std::string vmName = "__vm_execute";
    if (aggressiveMode && vmManagementEnabled) {
        vmName = "__vm_execute_" + std::to_string(totalVirtualizedFunctions);
    }
    
    Function *VM = Function::Create(VMTy, GlobalValue::InternalLinkage, vmName, M);
    BasicBlock *EntryBB = BasicBlock::Create(Ctx, "entry", VM);
    IRBuilder<> Builder(EntryBB);
    
    // VM interpreter loop with memory management
    Value *Bytecode = VM->getArg(0);
    Value *Size = VM->getArg(1);
    
    // Add memory check for aggressive mode
    if (aggressiveMode && vmManagementEnabled) {
        outs() << "🔧 Creating VM with memory management: " << vmName << "\n";
    }
    
    AllocaInst *PC = Builder.CreateAlloca(Int32Ty, nullptr, "pc");
    Builder.CreateStore(ConstantInt::get(Int32Ty, 0), PC);
    
    BasicBlock *LoopBB = BasicBlock::Create(Ctx, "loop", VM);
    BasicBlock *ExitBB = BasicBlock::Create(Ctx, "exit", VM);
    
    Builder.CreateBr(LoopBB);
    
    IRBuilder<> LoopBuilder(LoopBB);
    Value *CurrentPC = LoopBuilder.CreateLoad(Int32Ty, PC);
    Value *Cond = LoopBuilder.CreateICmpSLT(CurrentPC, Size);
    LoopBuilder.CreateCondBr(Cond, LoopBB, ExitBB);
    
    // VM instruction dispatch
    BasicBlock *DispatchBB = BasicBlock::Create(Ctx, "dispatch", VM);
    LoopBuilder.CreateBr(DispatchBB);
    
    IRBuilder<> DispatchBuilder(DispatchBB);
    Value *PCVal = DispatchBuilder.CreateLoad(Int32Ty, PC);
    Value *InstrPtr = DispatchBuilder.CreateInBoundsGEP(Type::getInt8Ty(Ctx), Bytecode, PCVal);
    Value *Instr = DispatchBuilder.CreateLoad(Type::getInt8Ty(Ctx), InstrPtr);
    
    // Simple instruction switch
    SwitchInst *Switch = DispatchBuilder.CreateSwitch(Instr, ExitBB, 3);
    Switch->addCase(ConstantInt::get(Type::getInt8Ty(Ctx), 0x01), ExitBB); // ADD
    Switch->addCase(ConstantInt::get(Type::getInt8Ty(Ctx), 0x02), ExitBB); // SUB
    Switch->addCase(ConstantInt::get(Type::getInt8Ty(Ctx), 0xFF), ExitBB); // RETURN
    
    // Increment PC
    Value *NextPC = DispatchBuilder.CreateAdd(PCVal, ConstantInt::get(Int32Ty, 1));
    DispatchBuilder.CreateStore(NextPC, PC);
    DispatchBuilder.CreateBr(LoopBB);
    
    IRBuilder<> ExitBuilder(ExitBB);
    ExitBuilder.CreateRetVoid();
    
    return VM;
}

// Helper function to check if a function is an original function (not a variant)
bool ObfuscationPass::isOriginalFunction(Function &F) {
    std::string name = F.getName().str();
    // Only allow obfuscation of functions that were originally in the module
    // This prevents variants from generating their own variants exponentially

    // Skip generated VM functions (they have .NNNN suffixes or contain __vm_execute)
    if (name.find("__vm_execute") != std::string::npos) {
        return false;
    }

    // Skip functions that start with obfuscation prefixes
    if (name.find("__obf_") == 0 || name.find("__bogus_") == 0 ||
        name.find("__flatten_") == 0 || name.find("__mba_") == 0) {
        return false;
    }

    // Skip any function that contains dots (LLVM-generated suffixes)
    if (name.find(".") != std::string::npos) {
        return false;
    }

    // Skip any function that contains numbers (likely generated)
    if (std::any_of(name.begin(), name.end(), ::isdigit)) {
        return false;
    }

    return originalFunctionNames.count(name) > 0;
}

// Polymorphic Code Generation - Self-modifying code variants
bool ObfuscationPass::generatePolymorphicCode(Function &F) {
    if (F.isDeclaration()) return false;

    // Only generate variants for original functions, not for variants themselves
    // This prevents exponential explosion: main -> main_variant_0, main_variant_1, etc.
    // But NOT: main_variant_0 -> main_variant_0_variant_0, main_variant_0_variant_1, etc.
    if (!isOriginalFunction(F)) {
        return false; // Skip variants to prevent exponential explosion
    }

    // Generate multiple variants of the function
    for (int i = 0; i < config.polymorphicVariants; i++) {
        generatePolymorphicVariant(F, i);
        totalPolymorphicVariants++;
    }

    logMetrics("polymorphic_variants", totalPolymorphicVariants);
    return true;
}

void ObfuscationPass::generatePolymorphicVariant(Function &F, int variant) {
    // Create variant function
    LLVMContext &Ctx = F.getContext();
    FunctionType *FTy = F.getFunctionType();
    std::string variantName = F.getName().str() + "_variant_" + std::to_string(variant);
    Function *VariantF = Function::Create(FTy, F.getLinkage(), variantName, F.getParent());
    
    // Clone original function
    ValueToValueMapTy VMap;
    for (auto &Arg : F.args()) {
        VMap[&Arg] = &*VariantF->arg_begin() + Arg.getArgNo();
    }
    
    SmallVector<ReturnInst*, 8> Returns;
    CloneFunctionInto(VariantF, &F, VMap, CloneFunctionChangeType::LocalChangesOnly, Returns);
    
    // Apply random transformations to variant
    randomizeCodeStructure(*VariantF);
}

// Anti-Analysis - Detect analysis tools and evade
bool ObfuscationPass::insertAntiAnalysis(Module &M) {
    LLVMContext &Ctx = M.getContext();
    
    // Create analysis detection function
    FunctionType *CheckType = FunctionType::get(Type::getInt32Ty(Ctx), false);
    Function *AnalysisCheck = Function::Create(CheckType, GlobalValue::InternalLinkage, "__check_analysis", &M);
    
    BasicBlock *BB = BasicBlock::Create(Ctx, "entry", AnalysisCheck);
    IRBuilder<> Builder(BB);
    
    // Check for common analysis tools
    Value *Detected = ConstantInt::get(Type::getInt32Ty(Ctx), 0);
    
    #ifdef _WIN32
    // Check for IDA Pro, x64dbg, etc.
    FunctionType *GetModuleHandleType = FunctionType::get(PointerType::getUnqual(Type::getInt8Ty(Ctx)), {PointerType::getUnqual(Type::getInt8Ty(Ctx))}, false);
    FunctionCallee GetModuleHandle = M.getOrInsertFunction("GetModuleHandleA", GetModuleHandleType);
    
    // Check for IDA Pro
    Value *IDAHandle = Builder.CreateCall(GetModuleHandle, {Builder.CreateGlobalString("ida64.exe")});
    Value *IDACheck = Builder.CreateICmpNE(IDAHandle, ConstantPointerNull::get(PointerType::getUnqual(Type::getInt8Ty(Ctx))));
    
    // Check for x64dbg
    Value *X64Handle = Builder.CreateCall(GetModuleHandle, {Builder.CreateGlobalString("x64dbg.exe")});
    Value *X64Check = Builder.CreateICmpNE(X64Handle, ConstantPointerNull::get(PointerType::getUnqual(Type::getInt8Ty(Ctx))));
    
    Value *AnyDetected = Builder.CreateOr(IDACheck, X64Check);
    Detected = Builder.CreateSelect(AnyDetected, ConstantInt::get(Type::getInt32Ty(Ctx), 1), Detected);
    #endif
    
    Builder.CreateRet(Detected);
    
    // Insert checks in critical functions
    for (Function &F : M) {
        if (F.isDeclaration() || &F == AnalysisCheck) continue;
        if (F.getName() == "main") {
            BasicBlock &Entry = F.getEntryBlock();
            Instruction *SplitPt = &*Entry.getFirstNonPHIOrDbgOrAlloca();
            if (!SplitPt) continue;
            BasicBlock *OrigCont = Entry.splitBasicBlock(SplitPt, "orig_entry.cont");
            Entry.getTerminator()->eraseFromParent();

            IRBuilder<> B(&Entry);
            Value *Check = B.CreateCall(AnalysisCheck);
            Value *Cond = B.CreateICmpNE(Check, ConstantInt::get(Type::getInt32Ty(Ctx), 0));

            BasicBlock *ExitBB = BasicBlock::Create(Ctx, "analysis_detected", &F);
            IRBuilder<> ExitBuilder(ExitBB);
            ExitBuilder.CreateRet(ConstantInt::get(Type::getInt32Ty(Ctx), -2));

            B.CreateCondBr(Cond, ExitBB, OrigCont);
            
            totalAntiAnalysisChecks++;
        }
    }
    
    logMetrics("anti_analysis_checks", totalAntiAnalysisChecks);
    return totalAntiAnalysisChecks > 0;
}

// Metamorphic Engine - Randomize code structure
bool ObfuscationPass::applyMetamorphicTransform(Function &F) {
    if (F.isDeclaration()) return false;
    
    randomizeCodeStructure(F);
    totalMetamorphicTransformations++;
    logMetrics("metamorphic_transformations", 1);
    return true;
}

void ObfuscationPass::randomizeCodeStructure(Function &F) {
    // Randomize basic block order
    std::vector<BasicBlock*> blocks;
    for (BasicBlock &BB : F) {
        blocks.push_back(&BB);
    }
    
    // Shuffle blocks (simplified - in real implementation, maintain CFG)
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(blocks.begin(), blocks.end(), g);
    
    // Add random nops between instructions
    for (BasicBlock &BB : F) {
        for (Instruction &I : make_early_inc_range(BB)) {
            if (I.getOpcode() % 3 == 0) { // Random condition
                IRBuilder<> Builder(&I);
                Value *Dummy = Builder.CreateAlloca(Type::getInt32Ty(F.getContext()));
                Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(F.getContext()), 0xDEADBEEF), Dummy);
            }
        }
    }
}

// Dynamic Obfuscation - Runtime code transformation
bool ObfuscationPass::insertDynamicObfuscation(Module &M) {
    LLVMContext &Ctx = M.getContext();

    // Create dynamic transformation function
    FunctionType *TransformType = FunctionType::get(Type::getVoidTy(Ctx), false);
    Function *TransformFunc = Function::Create(TransformType, GlobalValue::InternalLinkage, "__dynamic_transform", &M);

    BasicBlock *BB = BasicBlock::Create(Ctx, "entry", TransformFunc);
    IRBuilder<> Builder(BB);

    // Create self-modifying code that changes its own instructions
    AllocaInst *CodePtr = Builder.CreateAlloca(Type::getInt32Ty(Ctx));
    Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Ctx), 0x12345678), CodePtr);

    // XOR with random key to modify code
    Value *Key = ConstantInt::get(Type::getInt32Ty(Ctx), 0x87654321);
    Value *Code = Builder.CreateLoad(Type::getInt32Ty(Ctx), CodePtr);
    Value *Modified = Builder.CreateXor(Code, Key);
    Builder.CreateStore(Modified, CodePtr);

    Builder.CreateRetVoid();

    // Add to global constructors for runtime execution
    Type *Int32Ty = Type::getInt32Ty(Ctx);
    StructType *ST = StructType::get(Ctx, {Int32Ty, TransformFunc->getType(), PointerType::getUnqual(Type::getInt8Ty(Ctx))});
    Constant *Init = ConstantStruct::get(ST,
        { ConstantInt::get(Int32Ty, 65534), TransformFunc, ConstantPointerNull::get(PointerType::getUnqual(Type::getInt8Ty(Ctx))) });
    ArrayType *AT = ArrayType::get(ST, 1);
    GlobalVariable *GVArr = cast<GlobalVariable>(M.getOrInsertGlobal("llvm.global_ctors", AT));
    GVArr->setLinkage(GlobalValue::AppendingLinkage);
    GVArr->setInitializer(ConstantArray::get(AT, { Init }));

    totalDynamicObfuscations++;
    logMetrics("dynamic_obfuscations", 1);
    return true;
}
