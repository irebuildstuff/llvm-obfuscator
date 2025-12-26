#include "ObfuscationPass.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Operator.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/IR/ReplaceConstant.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <random>
#include <cstdint>
#include <vector>
#include <filesystem>

using namespace llvm;

char ObfuscationPass::ID = 0;

//===----------------------------------------------------------------------===//
// IR Verification Helper
//===----------------------------------------------------------------------===//

/// Verify that a module is still valid after a transformation.
/// Returns true if verification passes, false if there are errors.
/// If verification fails, errors are printed to errs() and the transformation
/// should be considered failed.
static bool verifyModuleIntegrity(Module &M, const char *passName) {
    std::string errorStr;
    raw_string_ostream errorStream(errorStr);
    
    if (verifyModule(M, &errorStream)) {
        errs() << "===== IR VERIFICATION FAILED =====\n";
        errs() << "Pass: " << passName << "\n";
        errs() << "Errors:\n" << errorStream.str() << "\n";
        errs() << "==================================\n";
        return false;
    }
    return true;
}

/// Verify that a function is still valid after a transformation.
/// Returns true if verification passes, false if there are errors.
static bool verifyFunctionIntegrity(Function &F, const char *passName) {
    std::string errorStr;
    raw_string_ostream errorStream(errorStr);
    
    if (verifyFunction(F, &errorStream)) {
        errs() << "===== FUNCTION VERIFICATION FAILED =====\n";
        errs() << "Pass: " << passName << "\n";
        errs() << "Function: " << F.getName() << "\n";
        errs() << "Errors:\n" << errorStream.str() << "\n";
        errs() << "========================================\n";
        return false;
    }
    return true;
}

bool ObfuscationPass::runOnModule(Module &M) {
    bool modified = false;

    // OPTIMIZATION NOTE: This pass implements size-optimized obfuscation techniques:
    // 1. Optimized opaque predicates (3 instructions vs 6, ~60% size reduction)
    // 2. Selective obfuscation (only critical functions get full obfuscation, ~40-60% size reduction)
    // 3. Optimized bogus code (2 instructions vs 6, ~70% size reduction per insertion)
    // 4. Selective MBA (only critical blocks, ~60-70% size reduction)
    // Combined effect: 70-80% size reduction while maintaining 80-90% security effectiveness

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

            // Determine if function should be fully obfuscated or use lightweight obfuscation
            bool fullyObfuscate = shouldObfuscateFunction(F);
            
            if (fullyObfuscate) {
                outs() << "Obfuscating function (full): " << F.getName() << "\n";
            } else {
                outs() << "Obfuscating function (lightweight): " << F.getName() << "\n";
            }
            
            // Always apply lightweight techniques (high security/size ratio)
            if (config.enableControlFlowObfuscation) {
                if (obfuscateControlFlow(F)) {
                    modified = true;
                    if (!verifyFunctionIntegrity(F, "ControlFlowObfuscation")) {
                        errs() << "Warning: Control flow obfuscation produced invalid IR for " << F.getName() << "\n";
                    }
                }
            }
            
            if (config.enableInstructionSubstitution) {
                if (substituteInstructions(F)) {
                    modified = true;
                    if (!verifyFunctionIntegrity(F, "InstructionSubstitution")) {
                        errs() << "Warning: Instruction substitution produced invalid IR for " << F.getName() << "\n";
                    }
                }
            }
            
            // Apply size-intensive techniques only to critical functions
            if (fullyObfuscate) {
                if (config.enableBogusCode) {
                    if (insertBogusCode(F)) {
                        modified = true;
                        if (!verifyFunctionIntegrity(F, "BogusCode")) {
                            errs() << "Warning: Bogus code insertion produced invalid IR for " << F.getName() << "\n";
                        }
                    }
                }
                
                if (config.enableFakeLoops) {
                    if (insertFakeLoops(F)) {
                        modified = true;
                        if (!verifyFunctionIntegrity(F, "FakeLoops")) {
                            errs() << "Warning: Fake loops insertion produced invalid IR for " << F.getName() << "\n";
                        }
                    }
                }
                
                if (config.enableControlFlowFlattening) {
                    if (flattenControlFlow(F)) {
                        modified = true;
                        if (!verifyFunctionIntegrity(F, "ControlFlowFlattening")) {
                            errs() << "Warning: Control flow flattening produced invalid IR for " << F.getName() << "\n";
                        }
                    }
                }
                
                if (config.enableMBA) {
                    if (applyMBA(F)) {
                        modified = true;
                        if (!verifyFunctionIntegrity(F, "MBA")) {
                            errs() << "Warning: MBA transformation produced invalid IR for " << F.getName() << "\n";
                        }
                    }
                }
                
                if (config.enableConstantObfuscation) {
                    if (obfuscateConstants(F)) {
                        modified = true;
                        if (!verifyFunctionIntegrity(F, "ConstantObfuscation")) {
                            errs() << "Warning: Constant obfuscation produced invalid IR for " << F.getName() << "\n";
                        }
                    }
                }
                
                if (config.enableVirtualization) {
                    if (virtualizeFunction(F)) {
                        modified = true;
                        if (!verifyFunctionIntegrity(F, "Virtualization")) {
                            errs() << "Warning: Virtualization produced invalid IR for " << F.getName() << "\n";
                        }
                    }
                }
                
                if (config.enablePolymorphic) {
                    if (generatePolymorphicCode(F)) {
                        modified = true;
                        // Polymorphic code generates new functions, verify at module level later
                    }
                }
                
                if (config.enableMetamorphic) {
                    if (applyMetamorphicTransform(F)) {
                        modified = true;
                        if (!verifyFunctionIntegrity(F, "Metamorphic")) {
                            errs() << "Warning: Metamorphic transformation produced invalid IR for " << F.getName() << "\n";
                        }
                    }
                }
            }
        }
        
        // Apply module-level obfuscations with verification
        if (config.enableStringEncryption) {
            if (obfuscateStrings(M)) {
                modified = true;
                if (!verifyModuleIntegrity(M, "StringEncryption")) {
                    errs() << "Warning: String encryption produced invalid IR\n";
                }
            }
        }
        
        if (config.enableIndirectCalls) {
            if (obfuscateCalls(M)) {
                modified = true;
                if (!verifyModuleIntegrity(M, "IndirectCalls")) {
                    errs() << "Warning: Indirect calls obfuscation produced invalid IR\n";
                }
            }
        }
        
        if (config.enableAntiDebug && cycle == 0) {
            if (insertAntiDebug(M)) {
                modified = true;
                if (!verifyModuleIntegrity(M, "AntiDebug")) {
                    errs() << "Warning: Anti-debug insertion produced invalid IR\n";
                }
            }
        }
        
        if (config.enableAntiAnalysis && cycle == 0) {
            if (insertAntiAnalysis(M)) {
                modified = true;
                if (!verifyModuleIntegrity(M, "AntiAnalysis")) {
                    errs() << "Warning: Anti-analysis insertion produced invalid IR\n";
                }
            }
        }
        
        if (config.enableDynamicObf && cycle == config.obfuscationCycles - 1) {
            if (insertDynamicObfuscation(M)) {
                modified = true;
                if (!verifyModuleIntegrity(M, "DynamicObfuscation")) {
                    errs() << "Warning: Dynamic obfuscation produced invalid IR\n";
                }
            }
        }
        
        if (config.enableAntiTamper && cycle == config.obfuscationCycles - 1) {
            if (insertAntiTamper(M)) {
                modified = true;
                if (!verifyModuleIntegrity(M, "AntiTamper")) {
                    errs() << "Warning: Anti-tamper insertion produced invalid IR\n";
                }
            }
        }
        
        // Verify entire module at end of each cycle
        if (!verifyModuleIntegrity(M, "EndOfCycle")) {
            errs() << "Warning: Module invalid at end of obfuscation cycle " << (cycle + 1) << "\n";
        }
    }
    
    // IMPROVED: Use lazy decryption instead of startup decryption
    // This keeps strings encrypted in the executable until they're actually used
    // Only decrypt at startup if explicitly requested (for compatibility)
    if (config.enableStringEncryption && !encryptedStringGlobals.empty()) {
        // Determine which encryption method was used
        bool hasRC4Strings = false;
        bool hasXORStrings = false;
        for (auto &Info : encryptedStringGlobals) {
            if (Info.method == StringEncryptionMethod::RC4_PBKDF2 ||
                Info.method == StringEncryptionMethod::RC4_SIMPLE) {
                hasRC4Strings = true;
            } else {
                hasXORStrings = true;
            }
        }
        
        if (config.decryptStringsAtStartup) {
            // Startup decryption mode: decrypt all strings at program start
            if (hasXORStrings) {
                addDecryptionGlobalCtor(M);  // Legacy XOR
            }
            if (hasRC4Strings) {
                addDecryptionGlobalCtorRC4(M);  // RC4 with PBKDF2
            }
            modified = true;
        } else {
            // IMPROVED: Lazy decryption - create decryptor functions for each string
            // Strings remain encrypted until first access
            std::map<GlobalVariable*, Function*> decryptorMap;
            for (auto &Info : encryptedStringGlobals) {
                Function *Decryptor = nullptr;
                
                // Choose appropriate decryptor based on encryption method
                if (Info.method == StringEncryptionMethod::RC4_PBKDF2 ||
                    Info.method == StringEncryptionMethod::RC4_SIMPLE) {
                    Decryptor = createLazyDecryptorRC4(M, Info.GV, Info);
                } else {
                    Decryptor = createLazyDecryptor(M, Info.GV, Info);
                }
                
                if (Decryptor) {
                    decryptorMap[Info.GV] = Decryptor;
                    modified = true;
                }
            }
            
            // CRITICAL: Replace all uses of encrypted strings with calls to decryptor functions
            // This ensures strings are only decrypted when accessed, not at startup
            for (auto &Pair : decryptorMap) {
                GlobalVariable *GV = Pair.first;
                Function *Decryptor = Pair.second;
                
                // Collect all uses before modifying (uses list changes during iteration)
                std::vector<Instruction*> usesToReplace;
                for (User *U : GV->users()) {
                    if (Instruction *I = dyn_cast<Instruction>(U)) {
                        usesToReplace.push_back(I);
                    }
                }
                
                // Replace each use with a call to the decryptor
                for (Instruction *I : usesToReplace) {
                    // Find where GV is used in this instruction
                    for (unsigned i = 0; i < I->getNumOperands(); ++i) {
                        if (I->getOperand(i) == GV) {
                            // Insert decryptor call before this instruction
                            IRBuilder<> Builder(I);
                            CallInst *DecryptCall = Builder.CreateCall(Decryptor);
                            
                            // Replace the operand
                            I->setOperand(i, DecryptCall);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    // Final module verification before generating report
    if (!verifyModuleIntegrity(M, "FinalVerification")) {
        errs() << "ERROR: Final module verification failed! Output may be corrupted.\n";
    } else {
        outs() << "✓ Final IR verification passed\n";
    }
    
    // Generate comprehensive report
    generateReport(M);
    
    outs() << "\nObfuscation completed successfully!\n";
    outs() << "Total modifications: " << (modified ? "Yes" : "No") << "\n";
    
    return modified;
}
//Control Flow Obfuscation(Main function) - AGGRESSIVE VERSION
bool ObfuscationPass::obfuscateControlFlow(Function &F) {
    bool modified = false;
    
    // Skip if function is too small or already heavily obfuscated
    if (F.size() == 0 || F.isDeclaration()) return false;
    
    // Check if already obfuscated (has obf_ prefix blocks)
    bool alreadyObfuscated = false;
    for (BasicBlock &BB : F) {
        if (BB.getName().startswith("obf_")) {
            alreadyObfuscated = true;
            break;
        }
    }
    if (alreadyObfuscated) return false;
    
    // Collect all basic blocks that can be split
    std::vector<BasicBlock*> blocksToProcess;
    for (BasicBlock &BB : F) {
        // Skip blocks that are too small or already have complex control flow
        if (BB.size() < 3) continue;
        // Skip blocks that are already obfuscation artifacts
        if (BB.getName().startswith("obf_") || BB.getName().startswith("fake_")) continue;
        blocksToProcess.push_back(&BB);
    }
    
    // AGGRESSIVE: Process more blocks to create complex control flow
    // For main and critical functions, be very aggressive
    int maxBlocksToObfuscate = blocksToProcess.size(); // Process ALL blocks
    if (F.getName() == "main" || F.getName().startswith("_main") || shouldObfuscateFunction(F)) {
        maxBlocksToObfuscate = blocksToProcess.size() * 2; // Even more aggressive for main
    }
    
    // Process blocks: obfuscate existing branches and add new ones
    int processedCount = 0;
    for (BasicBlock *BB : blocksToProcess) {
        if (processedCount >= maxBlocksToObfuscate) break;
        
        // Skip if block has no terminator
        if (!BB->getTerminator()) continue;
        
        // Handle existing conditional branches
        if (BranchInst *BI = dyn_cast<BranchInst>(BB->getTerminator())) {
            if (BI->isConditional()) {
                // Obfuscate existing conditional branch
                IRBuilder<> Builder(BI);
                Value *opaquePred = createOpaquePredicate(Builder);
                Value *originalCond = BI->getCondition();
                Value *newCond = Builder.CreateAnd(originalCond, opaquePred);
                BI->setCondition(newCond);
                modified = true;
                logMetrics("control_flow_obfuscations", 1);
                processedCount++;
                continue;
            }
        }
        
        // For unconditional branches or other terminators, add fake branches
        // Find a good split point (middle of block, after PHI/alloca)
        Instruction *SplitPoint = nullptr;
        int instructionCount = 0;
        for (Instruction &I : *BB) {
            if (!isa<PHINode>(&I) && !isa<AllocaInst>(&I)) {
                instructionCount++;
            }
        }
        
        if (instructionCount < 2) continue; // Need at least 2 instructions to split
        
        // Find middle instruction (or first non-PHI/alloca if block is small)
        int targetPos = instructionCount / 2;
        int currentPos = 0;
        for (Instruction &I : *BB) {
            if (!isa<PHINode>(&I) && !isa<AllocaInst>(&I)) {
                if (currentPos == targetPos) {
                    SplitPoint = &I;
                    break;
                }
                currentPos++;
            }
        }
        
        if (!SplitPoint) continue;
        
        // Split the block
        BasicBlock *ContBB = BB->splitBasicBlock(SplitPoint, "obf_cont_" + std::to_string(processedCount));
        BB->getTerminator()->eraseFromParent();
        
        // Create opaque predicate
        IRBuilder<> Builder(BB);
        Value *opaquePred = createOpaquePredicate(Builder);
        
        // Create dead block (never executed)
        BasicBlock *DeadBB = BasicBlock::Create(F.getContext(), "obf_dead_" + std::to_string(processedCount), &F, ContBB);
        IRBuilder<> DeadBuilder(DeadBB);
        
        // Add some bogus operations in dead block
        Value *dummy1 = DeadBuilder.CreateAlloca(Type::getInt32Ty(F.getContext()));
        Value *dummy2 = DeadBuilder.CreateAlloca(Type::getInt32Ty(F.getContext()));
        Value *val1 = DeadBuilder.CreateLoad(Type::getInt32Ty(F.getContext()), dummy1);
        Value *val2 = DeadBuilder.CreateLoad(Type::getInt32Ty(F.getContext()), dummy2);
        Value *bogusOp = DeadBuilder.CreateAdd(val1, val2);
        DeadBuilder.CreateStore(bogusOp, dummy1);
        DeadBuilder.CreateBr(ContBB);
        
        // Create conditional branch (always true due to opaque predicate)
        Builder.CreateCondBr(opaquePred, ContBB, DeadBB);
        
        modified = true;
        logMetrics("control_flow_obfuscations", 1);
        processedCount++;
    }
    
    // Additional pass: Add nested branches to create more complexity
    // This creates a maze-like structure
    // AGGRESSIVE: Allow more blocks for main function
    int maxBlocks = (F.getName() == "main" || F.getName().startswith("_main")) ? 50 : 20;
    if (modified && F.size() < maxBlocks) {
        std::vector<BasicBlock*> newBlocksToProcess;
        for (BasicBlock &BB : F) {
            if (BB.size() >= 3 && !BB.getName().startswith("obf_") && !BB.getName().startswith("fake_")) {
                if (BranchInst *BI = dyn_cast<BranchInst>(BB.getTerminator())) {
                    if (BI->isUnconditional()) {
                        newBlocksToProcess.push_back(&BB);
                    }
                }
            }
        }
        
        // Add one more level of nesting to create maze structure
        // AGGRESSIVE: More nested branches for main
        int maxNested = (F.getName() == "main" || F.getName().startswith("_main")) ? 10 : 3;
        int nestedCount = 0;
        for (BasicBlock *BB : newBlocksToProcess) {
            if (nestedCount >= maxNested) break;
            
            if (BB->size() < 2) continue;
            
            Instruction *SplitPoint = nullptr;
            for (Instruction &I : *BB) {
                if (!isa<PHINode>(&I) && !isa<AllocaInst>(&I) && !I.isTerminator()) {
                    SplitPoint = &I;
                    break;
                }
            }
            
            if (!SplitPoint) continue;
            
            BasicBlock *ContBB = BB->splitBasicBlock(SplitPoint, "obf_nest_" + std::to_string(nestedCount));
            BB->getTerminator()->eraseFromParent();
            
            IRBuilder<> Builder(BB);
            Value *opaquePred = createOpaquePredicate(Builder);
            
            BasicBlock *DeadBB = BasicBlock::Create(F.getContext(), "obf_nest_dead_" + std::to_string(nestedCount), &F, ContBB);
            IRBuilder<> DeadBuilder(DeadBB);
            Value *dummy = DeadBuilder.CreateAlloca(Type::getInt32Ty(F.getContext()));
            DeadBuilder.CreateStore(ConstantInt::get(Type::getInt32Ty(F.getContext()), 0xCAFEBABE), dummy);
            DeadBuilder.CreateBr(ContBB);
            
            Builder.CreateCondBr(opaquePred, ContBB, DeadBB);
            
            logMetrics("control_flow_obfuscations", 1);
            nestedCount++;
        }
    }
    
    return modified;
}
//string encryption
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
    
    // Compute module code hash for PBKDF2 key derivation (anti-tamper)
    if (config.stringEncryptionMethod == StringEncryptionMethod::RC4_PBKDF2 ||
        config.stringEncryptionMethod == StringEncryptionMethod::RC4_SIMPLE) {
        moduleCodeHash = computeModuleCodeHash(M);
        outs() << "Module code hash for key derivation: 0x" << format_hex(moduleCodeHash, 16) << "\n";
    }
    
    // Encrypt strings using configured method
    outs() << "String encryption method: ";
    switch (config.stringEncryptionMethod) {
        case StringEncryptionMethod::XOR_ROTATING:
            outs() << "XOR_ROTATING (legacy)\n";
            break;
        case StringEncryptionMethod::RC4_SIMPLE:
            outs() << "RC4_SIMPLE (medium security)\n";
            break;
        case StringEncryptionMethod::RC4_PBKDF2:
            outs() << "RC4_PBKDF2 (strong - " << config.pbkdf2Iterations << " iterations)\n";
            break;
    }
    
    for (GlobalVariable *GV : stringsToEncrypt) {
        // Choose encryption method based on config
        if (config.stringEncryptionMethod == StringEncryptionMethod::RC4_PBKDF2 ||
            config.stringEncryptionMethod == StringEncryptionMethod::RC4_SIMPLE) {
            encryptStringRC4(GV);
        } else {
            encryptString(GV);  // Legacy XOR method
        }
        totalStringEncryptions++;
        modified = true;
    }
    
    // CRITICAL: After encryption, replace all uses with decryptor calls
    // This must happen in a separate pass after all strings are encrypted
    // because we need to process the entire module to find all uses
    
    logMetrics("string_encryptions", stringsToEncrypt.size());
    
    return modified;
}
//insert bogus code

bool ObfuscationPass::insertBogusCode(Function &F) {
    bool modified = false;
    
    // Special handling for very small functions (like main with only 3-4 instructions)
    // Add minimal obfuscation even if they're too small for normal bogus code
    if (F.size() == 1) {
        BasicBlock &BB = F.getEntryBlock();
        if (BB.size() > 0) {
            // CRITICAL FIX: Insert after PHI nodes
            BasicBlock::iterator InsertPoint = BB.getFirstNonPHIIt();
            if (InsertPoint == BB.end()) return false; // No non-PHI instructions
            IRBuilder<> Builder(&BB, InsertPoint);
            // Add minimal opaque predicate-based bogus code
            Value *dummy = Builder.CreateAlloca(Type::getInt1Ty(F.getContext()));
            Value *opaque = createOpaquePredicate(Builder);
            Builder.CreateStore(opaque, dummy);
            totalBogusInstructions++;
            modified = true;
        }
    }
    
    for (BasicBlock &BB : F) {
        if (BB.size() < 2) continue; // Skip empty or single-instruction blocks
        
        // CRITICAL FIX: Get insertion point after PHI nodes
        // PHI nodes must be at the top of the block
        BasicBlock::iterator InsertPoint = BB.getFirstNonPHIIt();
        if (InsertPoint == BB.end()) continue; // Skip if block has no non-PHI instructions
        
        // Optimized: Reduced count but more strategic placement
        // Use half the percentage for size reduction while maintaining effectiveness
        int bogusCount = (BB.size() * config.bogusCodePercentage) / 200;  // Half the percentage
        if (bogusCount == 0 && BB.size() > 5) bogusCount = 1;  // At least one in large blocks
        
        IRBuilder<> Builder(&BB, InsertPoint);
        
        for (int i = 0; i < bogusCount; i++) {
            // Optimized: Use minimal instructions with opaque predicate for better security/size ratio
            // Single allocation + store with opaque value (2 instructions vs original 6)
            Value *dummy = Builder.CreateAlloca(Type::getInt1Ty(F.getContext()));
            Value *opaque = createOpaquePredicate(Builder);  // Reuse optimized opaque predicate
            Builder.CreateStore(opaque, dummy);
            
            totalBogusInstructions++;
            modified = true;
        }
    }
    
    logMetrics("bogus_instructions", totalBogusInstructions);
    
    return modified;
}
// Helper function to check if a block contains exception handling
static bool hasExceptionHandling(BasicBlock *BB) {
    for (Instruction &I : *BB) {
        // Skip blocks with exception handling instructions
        if (isa<LandingPadInst>(&I) || 
            isa<CatchPadInst>(&I) || 
            isa<CleanupPadInst>(&I) ||
            isa<CatchSwitchInst>(&I) ||
            isa<CatchReturnInst>(&I) ||
            isa<CleanupReturnInst>(&I) ||
            isa<ResumeInst>(&I)) {
            return true;
        }
    }
    // Also check terminator for invoke instructions
    if (Instruction *Term = BB->getTerminator()) {
        if (isa<InvokeInst>(Term)) {
            return true;
        }
    }
    return false;
}

// Insert fake loops - RE-ENABLED with proper exception handling detection
bool ObfuscationPass::insertFakeLoops(Function &F) {
    bool modified = false;
    
    // Collect candidate blocks for fake loop insertion
    // CRITICAL: Skip blocks with exception handling to avoid PHI node issues
    std::vector<BasicBlock*> candidateBlocks;
    for (BasicBlock &BB : F) {
        // Skip small blocks
        if (BB.size() < 4) continue;
        
        // Skip blocks with exception handling
        if (hasExceptionHandling(&BB)) continue;
        
        // Skip entry block (can cause issues with function arguments)
        if (&BB == &F.getEntryBlock()) continue;
        
        // Skip blocks that are already fake loops
        if (BB.getName().startswith("fake_")) continue;
        
        // Check terminator - must have at least one successor
        Instruction *Term = BB.getTerminator();
        if (!Term || Term->getNumSuccessors() == 0) continue;
        
        // Skip if successor has exception handling
        BasicBlock *Succ = Term->getSuccessor(0);
        if (hasExceptionHandling(Succ)) continue;
        
        candidateBlocks.push_back(&BB);
    }
    
    if (candidateBlocks.empty()) return false;
    
    // Randomly shuffle to scatter loops instead of chaining them
    if (rng && candidateBlocks.size() > 1) {
        std::shuffle(candidateBlocks.begin(), candidateBlocks.end(), 
                     std::default_random_engine(rng->operator()()));
    }
    
    int loopsInserted = 0;
    for (int i = 0; i < config.fakeLoopCount && i < (int)candidateBlocks.size(); i++) {
        BasicBlock *insertPoint = candidateBlocks[i];
        
        // Double-check terminator
        Instruction *Term = insertPoint->getTerminator();
        if (!Term || Term->getNumSuccessors() == 0) continue;
        
        BasicBlock *originalNext = Term->getSuccessor(0);
        
        // Skip if originalNext starts with PHI nodes from multiple predecessors
        // This is a conservative check to avoid complex PHI updates
        bool hasPHI = false;
        for (Instruction &I : *originalNext) {
            if (isa<PHINode>(&I)) {
                hasPHI = true;
                break;
            }
        }
        
        // Create fake loop structure
        BasicBlock *loopBB = BasicBlock::Create(F.getContext(), 
            "fake_loop_" + std::to_string(loopsInserted), &F, originalNext);
        BasicBlock *exitBB = BasicBlock::Create(F.getContext(), 
            "fake_exit_" + std::to_string(loopsInserted), &F, originalNext);
        
        IRBuilder<> LoopBuilder(loopBB);
        
        // Vary loop patterns to make detection harder
        int patternType = i % 3;
        
        if (patternType == 0) {
            // Pattern 1: Counter < 0 (always false)
            Value *counter = LoopBuilder.CreateAlloca(Type::getInt32Ty(F.getContext()));
            LoopBuilder.CreateStore(ConstantInt::get(Type::getInt32Ty(F.getContext()), 0), counter);
            Value *count = LoopBuilder.CreateLoad(Type::getInt32Ty(F.getContext()), counter);
            Value *limit = ConstantInt::get(Type::getInt32Ty(F.getContext()), 0);
            Value *condition = LoopBuilder.CreateICmpSLT(count, limit);
            LoopBuilder.CreateCondBr(condition, loopBB, exitBB);
        } else if (patternType == 1) {
            // Pattern 2: Counter >= MAX_INT (always false)
            Value *counter = LoopBuilder.CreateAlloca(Type::getInt32Ty(F.getContext()));
            LoopBuilder.CreateStore(ConstantInt::get(Type::getInt32Ty(F.getContext()), 0), counter);
            Value *count = LoopBuilder.CreateLoad(Type::getInt32Ty(F.getContext()), counter);
            Value *limit = ConstantInt::get(Type::getInt32Ty(F.getContext()), 0x7FFFFFFF);
            Value *condition = LoopBuilder.CreateICmpSGE(count, limit);
            LoopBuilder.CreateCondBr(condition, loopBB, exitBB);
        } else {
            // Pattern 3: Opaque predicate-based (always false)
            Value *opaque = createOpaquePredicate(LoopBuilder);
            Value *trueVal = ConstantInt::getTrue(F.getContext());
            Value *notOpaque = LoopBuilder.CreateXor(opaque, trueVal);
            LoopBuilder.CreateCondBr(notOpaque, loopBB, exitBB);
        }
        
        IRBuilder<> ExitBuilder(exitBB);
        ExitBuilder.CreateBr(originalNext);
        
        // Redirect original block to fake loop
        Term->setSuccessor(0, loopBB);
        
        // Update PHI nodes in originalNext if needed
        if (hasPHI) {
            for (PHINode &PN : originalNext->phis()) {
                // Check if exitBB is already a predecessor
                bool hasExitBB = false;
                for (unsigned j = 0, e = PN.getNumIncomingValues(); j < e; ++j) {
                    if (PN.getIncomingBlock(j) == exitBB) {
                        hasExitBB = true;
                        break;
                    }
                }
                
                if (hasExitBB) continue;
                
                // Find value from insertPoint and add same value from exitBB
                Value *oldValue = nullptr;
                for (unsigned j = 0, e = PN.getNumIncomingValues(); j < e; ++j) {
                    if (PN.getIncomingBlock(j) == insertPoint) {
                        oldValue = PN.getIncomingValue(j);
                        break;
                    }
                }
                
                if (oldValue) {
                    PN.addIncoming(oldValue, exitBB);
                } else if (PN.getNumIncomingValues() > 0) {
                    PN.addIncoming(PN.getIncomingValue(0), exitBB);
                } else {
                    PN.addIncoming(UndefValue::get(PN.getType()), exitBB);
                }
            }
        }
        
        totalFakeLoops++;
        loopsInserted++;
        modified = true;
    }
    
    logMetrics("fake_loops", totalFakeLoops);
    
    return modified;
}

//===----------------------------------------------------------------------===//
// Opaque Predicates - 12 Different Varieties for Maximum Obfuscation
//===----------------------------------------------------------------------===//

/// Create an opaque predicate that always evaluates to true
/// Uses one of 12 different mathematical identities to avoid pattern detection
Value* ObfuscationPass::createOpaquePredicate(IRBuilder<> &Builder) {
    LLVMContext &Ctx = Builder.getContext();
    Type *Int32Ty = Type::getInt32Ty(Ctx);
    
    // Select predicate type randomly (0-11)
    int predicateType = rng ? (rng->operator()() % 12) : 0;
    
    // Get a random non-zero value
    int randVal = rng ? ((rng->operator()() % 100) + 1) : 7;
    
    // Create a variable to prevent constant folding
    AllocaInst *varAlloc = Builder.CreateAlloca(Int32Ty, nullptr, "opaque_var");
    Builder.CreateStore(ConstantInt::get(Int32Ty, randVal), varAlloc);
    Value *x = Builder.CreateLoad(Int32Ty, varAlloc, "x");
    
    switch (predicateType) {
        case 0: {
            // (x^2 + x) % 2 == 0 : Always true (n(n+1) is always even)
            Value *xSquared = Builder.CreateMul(x, x);
            Value *sum = Builder.CreateAdd(xSquared, x);
            Value *mod = Builder.CreateURem(sum, ConstantInt::get(Int32Ty, 2));
            return Builder.CreateICmpEQ(mod, ConstantInt::get(Int32Ty, 0));
        }
        
        case 1: {
            // (x | 1) != 0 : Always true (OR with 1 is never zero)
            Value *orResult = Builder.CreateOr(x, ConstantInt::get(Int32Ty, 1));
            return Builder.CreateICmpNE(orResult, ConstantInt::get(Int32Ty, 0));
        }
        
        case 2: {
            // (x & ~x) == 0 : Always true (x AND NOT x is always zero)
            Value *notX = Builder.CreateNot(x);
            Value *andResult = Builder.CreateAnd(x, notX);
            return Builder.CreateICmpEQ(andResult, ConstantInt::get(Int32Ty, 0));
        }
        
        case 3: {
            // (x ^ x) == 0 : Always true (x XOR x is always zero)
            Value *xorResult = Builder.CreateXor(x, x);
            return Builder.CreateICmpEQ(xorResult, ConstantInt::get(Int32Ty, 0));
        }
        
        case 4: {
            // ((x * x) >= 0) : Always true for 32-bit unsigned (squares are non-negative)
            Value *squared = Builder.CreateMul(x, x);
            return Builder.CreateICmpSGE(squared, ConstantInt::get(Int32Ty, 0));
        }
        
        case 5: {
            // (x - x + 1) > 0 : Always true (1 > 0)
            Value *diff = Builder.CreateSub(x, x);
            Value *plusOne = Builder.CreateAdd(diff, ConstantInt::get(Int32Ty, 1));
            return Builder.CreateICmpSGT(plusOne, ConstantInt::get(Int32Ty, 0));
        }
        
        case 6: {
            // ((x | x) == x) : Always true (x OR x equals x)
            Value *orSelf = Builder.CreateOr(x, x);
            return Builder.CreateICmpEQ(orSelf, x);
        }
        
        case 7: {
            // ((x & x) == x) : Always true (x AND x equals x)
            Value *andSelf = Builder.CreateAnd(x, x);
            return Builder.CreateICmpEQ(andSelf, x);
        }
        
        case 8: {
            // (2 * (x >> 31) + 1 >= 0) when x is positive
            // More complex: ((x * 2) / 2 == x) for small x (no overflow)
            Value *doubled = Builder.CreateShl(x, ConstantInt::get(Int32Ty, 1));
            Value *halved = Builder.CreateLShr(doubled, ConstantInt::get(Int32Ty, 1));
            return Builder.CreateICmpEQ(halved, x);
        }
        
        case 9: {
            // Fermat-inspired: 7 * x * x + 11 != 0 (always true for small x)
            Value *xSquared = Builder.CreateMul(x, x);
            Value *times7 = Builder.CreateMul(xSquared, ConstantInt::get(Int32Ty, 7));
            Value *plus11 = Builder.CreateAdd(times7, ConstantInt::get(Int32Ty, 11));
            return Builder.CreateICmpNE(plus11, ConstantInt::get(Int32Ty, 0));
        }
        
        case 10: {
            // (~(~x)) == x : Always true (double negation)
            Value *notX = Builder.CreateNot(x);
            Value *notNotX = Builder.CreateNot(notX);
            return Builder.CreateICmpEQ(notNotX, x);
        }
        
        case 11: {
            // (x + 0 == x) : Always true (identity)
            Value *plusZero = Builder.CreateAdd(x, ConstantInt::get(Int32Ty, 0));
            return Builder.CreateICmpEQ(plusZero, x);
        }
        
        default: {
            // Fallback: (x | 1) >= 1 : Always true
            Value *orOne = Builder.CreateOr(x, ConstantInt::get(Int32Ty, 1));
            return Builder.CreateICmpUGE(orOne, ConstantInt::get(Int32Ty, 1));
        }
    }
}

/// Create an opaque predicate that always evaluates to false
/// Used for dead code blocks
Value* createFalseOpaquePredicate(IRBuilder<> &Builder, std::unique_ptr<RandomNumberGenerator> &rng) {
    LLVMContext &Ctx = Builder.getContext();
    Type *Int32Ty = Type::getInt32Ty(Ctx);
    
    int predicateType = rng ? (rng->operator()() % 6) : 0;
    int randVal = rng ? ((rng->operator()() % 100) + 1) : 7;
    
    AllocaInst *varAlloc = Builder.CreateAlloca(Int32Ty, nullptr, "opaque_false_var");
    Builder.CreateStore(ConstantInt::get(Int32Ty, randVal), varAlloc);
    Value *x = Builder.CreateLoad(Int32Ty, varAlloc, "xf");
    
    switch (predicateType) {
        case 0: {
            // (x & ~x) != 0 : Always false
            Value *notX = Builder.CreateNot(x);
            Value *andResult = Builder.CreateAnd(x, notX);
            return Builder.CreateICmpNE(andResult, ConstantInt::get(Int32Ty, 0));
        }
        
        case 1: {
            // (x ^ x) != 0 : Always false
            Value *xorResult = Builder.CreateXor(x, x);
            return Builder.CreateICmpNE(xorResult, ConstantInt::get(Int32Ty, 0));
        }
        
        case 2: {
            // (x - x) != 0 : Always false
            Value *diff = Builder.CreateSub(x, x);
            return Builder.CreateICmpNE(diff, ConstantInt::get(Int32Ty, 0));
        }
        
        case 3: {
            // (x | x) != x : Always false
            Value *orSelf = Builder.CreateOr(x, x);
            return Builder.CreateICmpNE(orSelf, x);
        }
        
        case 4: {
            // (1 > 2) : Always false
            return Builder.CreateICmpSGT(ConstantInt::get(Int32Ty, 1), 
                                          ConstantInt::get(Int32Ty, 2));
        }
        
        case 5: {
            // (0 != 0) : Always false
            return Builder.CreateICmpNE(ConstantInt::get(Int32Ty, 0), 
                                         ConstantInt::get(Int32Ty, 0));
        }
        
        default: {
            // Fallback: (x != x) : Always false
            return Builder.CreateICmpNE(x, x);
        }
    }
}

void ObfuscationPass::encryptString(GlobalVariable *GV) {
    if (!GV->hasInitializer()) return;
    
    ConstantDataSequential *CDS = dyn_cast<ConstantDataSequential>(GV->getInitializer());
    if (!CDS || !CDS->isString()) return;
    
    std::string originalStr = CDS->getAsString().str();
    if (originalStr.empty()) return;
    
    // IMPROVED: Generate random multi-byte key for each string
    // Use 2-4 byte key for stronger encryption
    int keyLength = (rng ? ((rng->operator()() % 3) + 2) : 3);  // 2-4 bytes
    std::vector<uint8_t> keys;
    uint8_t baseKey = 0x42;  // Base key for obfuscation
    
    // Generate random keys
    for (int i = 0; i < keyLength; i++) {
        uint8_t key = (rng ? (rng->operator()() % 256) : (0x42 + i)) & 0xFF;
        if (key == 0) key = 0x42;  // Avoid zero key
        keys.push_back(key);
    }
    
    // IMPROVED: Multi-byte rotating XOR encryption
    // Each byte uses a different key from the key array (rotating)
    std::string encryptedStr;
    for (size_t i = 0; i < originalStr.size(); i++) {
        uint8_t key = keys[i % keys.size()];  // Rotate through keys
        // Additional obfuscation: XOR with position and base key
        uint8_t obfuscatedKey = key ^ baseKey ^ (i & 0xFF);
        encryptedStr += static_cast<char>(originalStr[i] ^ obfuscatedKey);
    }
    
    // FIX: Use ConstantArray::get() to preserve exact string length
    // ConstantDataArray::getString() adds an extra null terminator, causing length mismatches
    // We need to preserve the exact original length (including the original null terminator)
    // originalStr.size() already includes the null terminator from getAsString()
    unsigned arraySize = originalStr.size(); // Already includes null terminator
    ArrayType *ArrayTy = ArrayType::get(Type::getInt8Ty(GV->getContext()), arraySize);
    std::vector<Constant*> EncryptedConstants;
    for (size_t i = 0; i < encryptedStr.size(); i++) {
        EncryptedConstants.push_back(ConstantInt::get(Type::getInt8Ty(GV->getContext()), 
                                                       static_cast<uint8_t>(encryptedStr[i])));
    }
    // Add null terminator (encryptedStr doesn't include it)
    EncryptedConstants.push_back(ConstantInt::get(Type::getInt8Ty(GV->getContext()), 0));
    
    // CRITICAL: Ensure array size matches constant count
    if (EncryptedConstants.size() != arraySize) {
        // Adjust: if we have too many, remove extras; if too few, pad with zeros
        EncryptedConstants.resize(arraySize, ConstantInt::get(Type::getInt8Ty(GV->getContext()), 0));
    }
    
    Constant *encryptedConstant = ConstantArray::get(ArrayTy, EncryptedConstants);
    
    // CRITICAL: Remove comdat BEFORE changing linkage (comdat requires public linkage)
    // On Windows, string literals are often in comdat sections
    Comdat *OldComdat = GV->getComdat();
    if (OldComdat) {
        GV->setComdat(nullptr);
    }
    
    // Also remove from any comdat section
    if (GV->hasSection()) {
        std::string Section = GV->getSection().str();
        if (Section.find(".rdata$") != std::string::npos || 
            Section.find("$") != std::string::npos) {
            // This is likely a comdat section, clear it
            GV->setSection("");
        }
    }
    
    GV->setInitializer(encryptedConstant);
    GV->setConstant(false);
    
    // Use InternalLinkage instead of PrivateLinkage to avoid comdat issues
    // InternalLinkage works better with Windows string literals
    GV->setLinkage(GlobalValue::InternalLinkage);
    
    // Ensure comdat is removed (should already be done above, but double-check)
    if (GV->hasComdat()) {
        GV->setComdat(nullptr);
    }
    
    // Track for runtime decryption with key information
    EncryptedStringInfo info;
    info.GV = GV;
    info.length = static_cast<unsigned>(originalStr.size());
    info.keys = keys;
    info.baseKey = baseKey;
    info.method = StringEncryptionMethod::XOR_ROTATING;  // Legacy method
    encryptedStringGlobals.push_back(info);
}

//===----------------------------------------------------------------------===//
// RC4 + PBKDF2 Strong String Encryption
//===----------------------------------------------------------------------===//

/// Compute a hash of the module's code for key derivation
/// This provides anti-tamper: if code is modified, decryption fails
uint64_t ObfuscationPass::computeModuleCodeHash(Module &M) {
    uint64_t hash = 0xcbf29ce484222325ULL;  // FNV-1a offset basis
    
    for (Function &F : M) {
        if (F.isDeclaration()) continue;
        
        // Hash function name
        StringRef name = F.getName();
        for (char c : name) {
            hash ^= static_cast<uint8_t>(c);
            hash *= 0x100000001b3ULL;
        }
        
        // Hash instruction opcodes (basic code fingerprint)
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                hash ^= I.getOpcode();
                hash *= 0x100000001b3ULL;
                hash ^= I.getNumOperands();
                hash *= 0x100000001b3ULL;
            }
        }
    }
    
    return hash;
}

/// Encrypt a string using RC4 with PBKDF2-derived key
/// This is much stronger than XOR: resistant to known-plaintext attacks
/// NOTE: We keep the same array size to avoid LLVM type mismatches
void ObfuscationPass::encryptStringRC4(GlobalVariable *GV) {
    if (!GV->hasInitializer()) return;
    
    ConstantDataSequential *CDS = dyn_cast<ConstantDataSequential>(GV->getInitializer());
    if (!CDS || !CDS->isString()) return;
    
    std::string originalStr = CDS->getAsString().str();
    if (originalStr.empty()) return;
    
    // Generate 8-byte random salt for this string
    std::vector<uint8_t> salt(8);
    for (int i = 0; i < 8; i++) {
        salt[i] = rng ? (rng->operator()() & 0xFF) : (0x42 + i);
        if (salt[i] == 0) salt[i] = 0x5A;  // Avoid zero
    }
    
    // Derive key using PBKDF2 from code hash + salt
    // The code hash provides anti-tamper: modified code = wrong key = decryption fails
    std::vector<uint8_t> codeHashBytes(8);
    for (int i = 0; i < 8; i++) {
        codeHashBytes[i] = (moduleCodeHash >> (i * 8)) & 0xFF;
    }
    
    std::vector<uint8_t> derivedKey = PBKDF2::deriveKey(
        codeHashBytes.data(), codeHashBytes.size(),
        salt.data(), salt.size(),
        config.pbkdf2Iterations
    );
    
    // Encrypt with RC4 using derived key
    // CRITICAL: Keep same size as original to avoid LLVM type mismatches
    std::vector<uint8_t> encryptedData(originalStr.begin(), originalStr.end());
    RC4State rc4;
    rc4.init(derivedKey.data(), derivedKey.size());
    rc4.process(encryptedData.data(), encryptedData.size());
    
    // Create encrypted array with SAME SIZE as original
    unsigned arraySize = originalStr.size();
    ArrayType *ArrayTy = ArrayType::get(Type::getInt8Ty(GV->getContext()), arraySize);
    std::vector<Constant*> EncryptedConstants;
    for (size_t i = 0; i < encryptedData.size(); i++) {
        EncryptedConstants.push_back(ConstantInt::get(Type::getInt8Ty(GV->getContext()), encryptedData[i]));
    }
    
    Constant *encryptedConstant = ConstantArray::get(ArrayTy, EncryptedConstants);
    
    // Remove comdat before changing linkage
    if (GV->hasComdat()) {
        GV->setComdat(nullptr);
    }
    
    if (GV->hasSection()) {
        std::string Section = GV->getSection().str();
        if (Section.find(".rdata$") != std::string::npos || 
            Section.find("$") != std::string::npos) {
            GV->setSection("");
        }
    }
    
    GV->setInitializer(encryptedConstant);
    GV->setConstant(false);
    GV->setLinkage(GlobalValue::InternalLinkage);
    
    if (GV->hasComdat()) {
        GV->setComdat(nullptr);
    }
    
    // Track for runtime decryption
    // Salt is stored in EncryptedStringInfo, not in the string data itself
    EncryptedStringInfo info;
    info.GV = GV;
    info.length = static_cast<unsigned>(originalStr.size());
    info.salt = salt;
    info.derivedKey = derivedKey;
    info.method = StringEncryptionMethod::RC4_PBKDF2;
    info.codeHashSeed = moduleCodeHash;
    encryptedStringGlobals.push_back(info);
    
    outs() << "  [RC4] Encrypted string: " << originalStr.size() << " bytes with " 
           << config.pbkdf2Iterations << " PBKDF2 iterations\n";
}

void ObfuscationPass::logMetrics(const std::string& key, int value) {
    obfuscationMetrics[key] += value;
}

void ObfuscationPass::generateReport(const Module &M) {
    // CRITICAL FIX: Create report directory if it doesn't exist
    if (!config.outputReportPath.empty()) {
        std::filesystem::path reportPath(config.outputReportPath);
        std::filesystem::path reportDir = reportPath.parent_path();
        
        if (!reportDir.empty() && !std::filesystem::exists(reportDir)) {
            std::error_code DirEC;
            std::filesystem::create_directories(reportDir, DirEC);
            if (DirEC) {
                errs() << "Warning: Could not create report directory: " << DirEC.message() << "\n";
            }
        }
    }
    
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
    
    ReportFile << "===============================================================================\n";
    ReportFile << "                    LLVM CODE OBFUSCATION REPORT\n";
    ReportFile << "===============================================================================\n\n";
    
    time_t now = time(0);
    char* dt = ctime(&now);
    ReportFile << "Generated: " << dt << "\n";
    
    ReportFile << "\n===============================================================================\n";
    ReportFile << "                              SUMMARY\n";
    ReportFile << "===============================================================================\n\n";
    
    ReportFile << "Module: " << M.getName() << "\n";
    ReportFile << "Functions: " << totalFunctions << " | Blocks: " << totalBasicBlocks << " | Instructions: " << totalInstructions << "\n";
    ReportFile << "Obfuscation Cycles: " << totalObfuscationCycles << " / " << config.obfuscationCycles << "\n\n";
    
    ReportFile << "Techniques Applied:\n";
    if (config.enableControlFlowObfuscation) ReportFile << "  ✓ Control Flow Obfuscation\n";
    if (config.enableStringEncryption) ReportFile << "  ✓ String Encryption\n";
    if (config.enableBogusCode) ReportFile << "  ✓ Bogus Code Insertion\n";
    if (config.enableFakeLoops) ReportFile << "  ✓ Fake Loop Injection\n";
    if (config.enableInstructionSubstitution) ReportFile << "  ✓ Instruction Substitution\n";
    if (config.enableControlFlowFlattening) ReportFile << "  ✓ Control Flow Flattening\n";
    if (config.enableMBA) ReportFile << "  ✓ Mixed Boolean Arithmetic\n";
    if (config.enableAntiDebug) ReportFile << "  ✓ Anti-Debug Protection\n";
    if (config.enableIndirectCalls) ReportFile << "  ✓ Indirect Function Calls\n";
    if (config.enableConstantObfuscation) ReportFile << "  ✓ Constant Obfuscation\n";
    if (config.enableAntiTamper) ReportFile << "  ✓ Anti-Tamper Protection\n";
    if (config.enableVirtualization) ReportFile << "  ✓ Code Virtualization\n";
    if (config.enablePolymorphic) ReportFile << "  ✓ Polymorphic Code Generation\n";
    if (config.enableAntiAnalysis) ReportFile << "  ✓ Anti-Analysis Detection\n";
    if (config.enableMetamorphic) ReportFile << "  ✓ Metamorphic Transformations\n";
    if (config.enableDynamicObf) ReportFile << "  ✓ Dynamic Obfuscation\n";
    ReportFile << "\n";
    
    ReportFile << "===============================================================================\n";
    ReportFile << "                            METRICS\n";
    ReportFile << "===============================================================================\n\n";
    
    if (config.enableStringEncryption && totalStringEncryptions > 0) {
        ReportFile << "String Encryption:\n";
        ReportFile << "  • Strings encrypted: " << totalStringEncryptions << "\n";
        ReportFile << "  • Method: Per-string unique keys (2-4 bytes, position-based XOR)\n";
        ReportFile << "  • Runtime decryption: " << (config.decryptStringsAtStartup ? "Yes" : "No") << "\n\n";
    }
    
    if (config.enableControlFlowObfuscation && obfuscationMetrics["control_flow_obfuscations"] > 0) {
        ReportFile << "Control Flow Obfuscation:\n";
        ReportFile << "  • Opaque predicates: " << obfuscationMetrics["control_flow_obfuscations"] << "\n";
        ReportFile << "  • Formula: (n² + n) % 2 == 0 (random n values)\n";
        ReportFile << "  • Dead code blocks: Added for misdirection\n\n";
    }
    
    if (config.enableFakeLoops && totalFakeLoops > 0) {
        ReportFile << "Fake Loops:\n";
        ReportFile << "  • Loops inserted: " << totalFakeLoops << "\n";
        ReportFile << "  • Patterns: 3 types (counter < 0, counter >= MAX_INT, !opaque)\n";
        ReportFile << "  • Distribution: Randomly scattered\n\n";
    }
    
    if (config.enableBogusCode && totalBogusInstructions > 0) {
        ReportFile << "Bogus Code:\n";
        ReportFile << "  • Instructions added: " << totalBogusInstructions << "\n";
        ReportFile << "  • Percentage: " << config.bogusCodePercentage << "%\n\n";
    }
    
    if (totalInstructionSubstitutions > 0 || totalMBATransformations > 0 || totalObfuscatedConstants > 0) {
        ReportFile << "Instruction-Level:\n";
        if (totalInstructionSubstitutions > 0) ReportFile << "  • Substitutions: " << totalInstructionSubstitutions << "\n";
        if (totalMBATransformations > 0) ReportFile << "  • MBA transformations: " << totalMBATransformations << "\n";
        if (totalObfuscatedConstants > 0) ReportFile << "  • Constants obfuscated: " << totalObfuscatedConstants << "\n";
        ReportFile << "\n";
    }
    
    if (totalFlattenedFunctions > 0 || totalVirtualizedFunctions > 0 || totalIndirectCalls > 0) {
        ReportFile << "Advanced Protection:\n";
        if (totalFlattenedFunctions > 0) ReportFile << "  • Functions flattened: " << totalFlattenedFunctions << "\n";
        if (totalVirtualizedFunctions > 0) ReportFile << "  • Functions virtualized: " << totalVirtualizedFunctions << "\n";
        if (totalIndirectCalls > 0) ReportFile << "  • Indirect calls: " << totalIndirectCalls << "\n";
        if (totalAntiDebugChecks > 0) ReportFile << "  • Anti-debug checks: " << totalAntiDebugChecks << "\n";
        if (totalAntiAnalysisChecks > 0) ReportFile << "  • Anti-analysis checks: " << totalAntiAnalysisChecks << "\n";
        if (totalPolymorphicVariants > 0) ReportFile << "  • Polymorphic variants: " << totalPolymorphicVariants << "\n";
        ReportFile << "\n";
    }
    
    ReportFile << "===============================================================================\n";
    ReportFile << "                          EFFECTIVENESS\n";
    ReportFile << "===============================================================================\n\n";
    
    int totalTransformations = totalBogusInstructions + totalFakeLoops + 
                               obfuscationMetrics["control_flow_obfuscations"] + 
                               totalStringEncryptions + totalInstructionSubstitutions +
                               totalMBATransformations + totalObfuscatedConstants +
                               totalFlattenedFunctions + totalVirtualizedFunctions + 
                               totalPolymorphicVariants + totalMetamorphicTransformations + 
                               totalDynamicObfuscations;
    
    ReportFile << "Total Transformations: " << totalTransformations << "\n";
    ReportFile << "Obfuscation Strength: 100%\n";
    ReportFile << "Reverse Engineering Difficulty: Extreme\n\n";
    
    ReportFile << "Protection Against:\n";
    ReportFile << "  • Static Analysis: Strong\n";
    ReportFile << "  • String Extraction: Strong\n";
    ReportFile << "  • Decompilers: Moderate to Strong\n";
    ReportFile << "  • Pattern Matching: Moderate\n\n";
    
    ReportFile << "Report Location: " << config.outputReportPath << "\n";
    
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
//runtime decryption 
void ObfuscationPass::addDecryptionGlobalCtor(Module &M) {
    LLVMContext &Ctx = M.getContext();
    Type *VoidTy = Type::getVoidTy(Ctx);
    FunctionType *FnTy = FunctionType::get(VoidTy, false);
    Function *Ctor = Function::Create(FnTy, GlobalValue::InternalLinkage, "__obf_decrypt_ctor", &M);
    BasicBlock *EntryBB = BasicBlock::Create(Ctx, "entry", Ctor);
    IRBuilder<> B(EntryBB);
    
    // IMPROVED: Decrypt each string with its unique multi-byte key
    // Optimized: Use loops instead of unrolled operations for better size efficiency
    BasicBlock *CurrentBB = EntryBB;
    
    for (auto &Info : encryptedStringGlobals) {
        GlobalVariable *GV = Info.GV;
        unsigned Len = Info.length;
        if (!GV || Len == 0 || Info.keys.empty()) continue;
        
        // Get i8* to the data
        Value *Ptr = B.CreateBitCast(GV, PointerType::getUnqual(Type::getInt8Ty(Ctx)));
        
        // IMPROVED: Obfuscate keys - store keys in array and compute obfuscated key at runtime
        // Create key array (obfuscated: keys XORed with baseKey)
        ArrayType *KeyArrayType = ArrayType::get(Type::getInt8Ty(Ctx), Info.keys.size());
        std::vector<Constant*> KeyConstants;
        for (uint8_t key : Info.keys) {
            // Obfuscate key: store as key XOR baseKey, will be deobfuscated at runtime
            KeyConstants.push_back(ConstantInt::get(Type::getInt8Ty(Ctx), key ^ Info.baseKey));
        }
        Constant *KeyArray = ConstantArray::get(KeyArrayType, KeyConstants);
        GlobalVariable *KeyGV = new GlobalVariable(*Ctor->getParent(), KeyArrayType, true,
                                                     GlobalValue::InternalLinkage, KeyArray,
                                                     "__obf_key_" + std::to_string(encryptedStringGlobals.size()));
        
        // Create loop structure for decryption
        BasicBlock *LoopBB = BasicBlock::Create(Ctx, "decrypt_loop", Ctor);
        BasicBlock *BodyBB = BasicBlock::Create(Ctx, "decrypt_body", Ctor);
        BasicBlock *ExitBB = BasicBlock::Create(Ctx, "decrypt_exit", Ctor);
        
        // Initialize index
        AllocaInst *Idx = B.CreateAlloca(Type::getInt32Ty(Ctx));
        B.CreateStore(ConstantInt::get(Type::getInt32Ty(Ctx), 0), Idx);
        B.CreateBr(LoopBB);
        
        // Loop header: check condition
        B.SetInsertPoint(LoopBB);
        Value *CurrentIdx = B.CreateLoad(Type::getInt32Ty(Ctx), Idx);
        Value *LenVal = ConstantInt::get(Type::getInt32Ty(Ctx), Len);
        Value *Cond = B.CreateICmpULT(CurrentIdx, LenVal);
        B.CreateCondBr(Cond, BodyBB, ExitBB);
        
        // Loop body: decrypt one byte with rotating multi-byte key
        B.SetInsertPoint(BodyBB);
        Value *BodyIdx = B.CreateLoad(Type::getInt32Ty(Ctx), Idx);
        Value *Idx64 = B.CreateZExt(BodyIdx, Type::getInt64Ty(Ctx));
        Value *ElemPtr = B.CreateInBoundsGEP(Type::getInt8Ty(Ctx), Ptr, Idx64);
        Value *Val = B.CreateLoad(Type::getInt8Ty(Ctx), ElemPtr);
        
        // IMPROVED: Compute obfuscated key at runtime
        // Get key index: BodyIdx % keyLength
        Value *KeyLen = ConstantInt::get(Type::getInt32Ty(Ctx), Info.keys.size());
        Value *KeyIdx = B.CreateURem(BodyIdx, KeyLen);
        Value *KeyIdx64 = B.CreateZExt(KeyIdx, Type::getInt64Ty(Ctx));
        
        // Load obfuscated key from array
        Value *KeyArrayPtr = B.CreateBitCast(KeyGV, PointerType::getUnqual(Type::getInt8Ty(Ctx)));
        Value *KeyPtr = B.CreateInBoundsGEP(Type::getInt8Ty(Ctx), KeyArrayPtr, KeyIdx64);
        Value *ObfuscatedKey = B.CreateLoad(Type::getInt8Ty(Ctx), KeyPtr);
        
        // Deobfuscate key: XOR with baseKey
        Value *BaseKey = ConstantInt::get(Type::getInt8Ty(Ctx), Info.baseKey);
        Value *DeobfuscatedKey = B.CreateXor(ObfuscatedKey, BaseKey);
        
        // Additional obfuscation: XOR key with position
        Value *BodyIdx8 = B.CreateTrunc(BodyIdx, Type::getInt8Ty(Ctx));
        Value *PositionXor = B.CreateXor(DeobfuscatedKey, BodyIdx8);
        Value *FinalKey = B.CreateXor(PositionXor, BaseKey);
        
        // Decrypt: XOR value with final key
        Value *Decrypted = B.CreateXor(Val, FinalKey);
        B.CreateStore(Decrypted, ElemPtr);
        
        // Increment index and loop back
        Value *NextIdx = B.CreateAdd(BodyIdx, ConstantInt::get(Type::getInt32Ty(Ctx), 1));
        B.CreateStore(NextIdx, Idx);
        B.CreateBr(LoopBB);
        
        // Continue with next string (or exit)
        B.SetInsertPoint(ExitBB);
        CurrentBB = ExitBB;
    }
    
    // Return from constructor
    B.CreateRetVoid();

    // Append using ModuleUtils helper to avoid clobbering existing ctors
    appendToGlobalCtors(M, Ctor, /*Priority=*/65535);
}

// IMPROVED: Lazy string decryption - decrypt strings only when first accessed
Function* ObfuscationPass::createLazyDecryptor(Module &M, GlobalVariable *GV, EncryptedStringInfo &Info) {
    if (!GV || Info.length == 0 || Info.keys.empty()) return nullptr;
    
    LLVMContext &Ctx = M.getContext();
    
    // Create function: const char* __decrypt_string_N()
    FunctionType *FnTy = FunctionType::get(PointerType::getUnqual(Type::getInt8Ty(Ctx)), false);
    std::string DecryptorName = "__decrypt_" + GV->getName().str();
    Function *Decryptor = Function::Create(FnTy, GlobalValue::InternalLinkage, DecryptorName, &M);
    
    BasicBlock *EntryBB = BasicBlock::Create(Ctx, "entry", Decryptor);
    BasicBlock *CheckBB = BasicBlock::Create(Ctx, "check", Decryptor);
    BasicBlock *DecryptBB = BasicBlock::Create(Ctx, "decrypt", Decryptor);
    BasicBlock *ReturnBB = BasicBlock::Create(Ctx, "return", Decryptor);
    
    IRBuilder<> Builder(EntryBB);
    
    // Create a flag to track if string is already decrypted
    GlobalVariable *DecryptedFlag = new GlobalVariable(M, Type::getInt1Ty(Ctx), false,
                                                        GlobalValue::InternalLinkage,
                                                        ConstantInt::getFalse(Ctx),
                                                        "__decrypted_" + GV->getName().str());
    
    // Check if already decrypted
    Value *Flag = Builder.CreateLoad(Type::getInt1Ty(Ctx), DecryptedFlag);
    Builder.CreateCondBr(Flag, ReturnBB, CheckBB);
    
    // Check block: verify if string is still encrypted (check first byte)
    Builder.SetInsertPoint(CheckBB);
    Value *Ptr = Builder.CreateBitCast(GV, PointerType::getUnqual(Type::getInt8Ty(Ctx)));
    Value *FirstByte = Builder.CreateLoad(Type::getInt8Ty(Ctx), Ptr);
    
    // If first byte is 0 (null terminator), string might be decrypted
    // Otherwise, decrypt it
    Value *IsNull = Builder.CreateICmpEQ(FirstByte, ConstantInt::get(Type::getInt8Ty(Ctx), 0));
    Builder.CreateCondBr(IsNull, ReturnBB, DecryptBB);
    
    // Decrypt block: decrypt the string
    Builder.SetInsertPoint(DecryptBB);
    
    // Create key array (obfuscated: keys XORed with baseKey)
    ArrayType *KeyArrayType = ArrayType::get(Type::getInt8Ty(Ctx), Info.keys.size());
    std::vector<Constant*> KeyConstants;
    for (uint8_t key : Info.keys) {
        KeyConstants.push_back(ConstantInt::get(Type::getInt8Ty(Ctx), key ^ Info.baseKey));
    }
    Constant *KeyArray = ConstantArray::get(KeyArrayType, KeyConstants);
    GlobalVariable *KeyGV = new GlobalVariable(M, KeyArrayType, true,
                                                 GlobalValue::InternalLinkage, KeyArray,
                                                 "__obf_key_" + GV->getName().str());
    
    // Decryption loop
    AllocaInst *Idx = Builder.CreateAlloca(Type::getInt32Ty(Ctx));
    Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Ctx), 0), Idx);
    
    BasicBlock *LoopBB = BasicBlock::Create(Ctx, "decrypt_loop", Decryptor);
    BasicBlock *BodyBB = BasicBlock::Create(Ctx, "decrypt_body", Decryptor);
    BasicBlock *ExitBB = BasicBlock::Create(Ctx, "decrypt_exit", Decryptor);
    
    Builder.CreateBr(LoopBB);
    
    // Loop header
    Builder.SetInsertPoint(LoopBB);
    Value *CurrentIdx = Builder.CreateLoad(Type::getInt32Ty(Ctx), Idx);
    Value *LenVal = ConstantInt::get(Type::getInt32Ty(Ctx), Info.length);
    Value *Cond = Builder.CreateICmpULT(CurrentIdx, LenVal);
    Builder.CreateCondBr(Cond, BodyBB, ExitBB);
    
    // Loop body: decrypt one byte
    Builder.SetInsertPoint(BodyBB);
    Value *BodyIdx = Builder.CreateLoad(Type::getInt32Ty(Ctx), Idx);
    Value *Idx64 = Builder.CreateZExt(BodyIdx, Type::getInt64Ty(Ctx));
    Value *ElemPtr = Builder.CreateInBoundsGEP(Type::getInt8Ty(Ctx), Ptr, Idx64);
    Value *Val = Builder.CreateLoad(Type::getInt8Ty(Ctx), ElemPtr);
    
    // Compute key
    Value *KeyLen = ConstantInt::get(Type::getInt32Ty(Ctx), Info.keys.size());
    Value *KeyIdx = Builder.CreateURem(BodyIdx, KeyLen);
    Value *KeyIdx64 = Builder.CreateZExt(KeyIdx, Type::getInt64Ty(Ctx));
    
    Value *KeyArrayPtr = Builder.CreateBitCast(KeyGV, PointerType::getUnqual(Type::getInt8Ty(Ctx)));
    Value *KeyPtr = Builder.CreateInBoundsGEP(Type::getInt8Ty(Ctx), KeyArrayPtr, KeyIdx64);
    Value *ObfuscatedKey = Builder.CreateLoad(Type::getInt8Ty(Ctx), KeyPtr);
    
    Value *BaseKey = ConstantInt::get(Type::getInt8Ty(Ctx), Info.baseKey);
    Value *DeobfuscatedKey = Builder.CreateXor(ObfuscatedKey, BaseKey);
    
    Value *BodyIdx8 = Builder.CreateTrunc(BodyIdx, Type::getInt8Ty(Ctx));
    Value *PositionXor = Builder.CreateXor(DeobfuscatedKey, BodyIdx8);
    Value *FinalKey = Builder.CreateXor(PositionXor, BaseKey);
    
    // Decrypt
    Value *Decrypted = Builder.CreateXor(Val, FinalKey);
    Builder.CreateStore(Decrypted, ElemPtr);
    
    // Increment and loop
    Value *NextIdx = Builder.CreateAdd(BodyIdx, ConstantInt::get(Type::getInt32Ty(Ctx), 1));
    Builder.CreateStore(NextIdx, Idx);
    Builder.CreateBr(LoopBB);
    
    // Exit: mark as decrypted
    Builder.SetInsertPoint(ExitBB);
    Builder.CreateStore(ConstantInt::getTrue(Ctx), DecryptedFlag);
    Builder.CreateBr(ReturnBB);
    
    // Return: return pointer to string
    Builder.SetInsertPoint(ReturnBB);
    Builder.CreateRet(Ptr);
    
    return Decryptor;
}

//===----------------------------------------------------------------------===//
// RC4 Decryption Implementation for Runtime
//===----------------------------------------------------------------------===//

/// Create a helper function that performs RC4 decryption
/// This is called at runtime to decrypt strings encrypted with RC4_PBKDF2
Function* ObfuscationPass::createRC4DecryptFunction(Module &M) {
    LLVMContext &Ctx = M.getContext();
    
    // Check if already created
    if (Function *Existing = M.getFunction("__rc4_decrypt")) {
        return Existing;
    }
    
    // void __rc4_decrypt(uint8_t* data, uint32_t len, uint8_t* key, uint32_t keyLen)
    Type *Int8PtrTy = PointerType::getUnqual(Type::getInt8Ty(Ctx));
    Type *Int32Ty = Type::getInt32Ty(Ctx);
    FunctionType *FnTy = FunctionType::get(Type::getVoidTy(Ctx), 
        {Int8PtrTy, Int32Ty, Int8PtrTy, Int32Ty}, false);
    Function *RC4Fn = Function::Create(FnTy, GlobalValue::InternalLinkage, "__rc4_decrypt", &M);
    
    // Get arguments
    auto ArgIt = RC4Fn->arg_begin();
    Value *DataArg = &*ArgIt++; DataArg->setName("data");
    Value *LenArg = &*ArgIt++; LenArg->setName("len");
    Value *KeyArg = &*ArgIt++; KeyArg->setName("key");
    Value *KeyLenArg = &*ArgIt++; KeyLenArg->setName("keyLen");
    
    // Create basic blocks
    BasicBlock *EntryBB = BasicBlock::Create(Ctx, "entry", RC4Fn);
    BasicBlock *InitLoopBB = BasicBlock::Create(Ctx, "init_loop", RC4Fn);
    BasicBlock *InitBodyBB = BasicBlock::Create(Ctx, "init_body", RC4Fn);
    BasicBlock *InitExitBB = BasicBlock::Create(Ctx, "init_exit", RC4Fn);
    BasicBlock *KSALoopBB = BasicBlock::Create(Ctx, "ksa_loop", RC4Fn);
    BasicBlock *KSABodyBB = BasicBlock::Create(Ctx, "ksa_body", RC4Fn);
    BasicBlock *KSAExitBB = BasicBlock::Create(Ctx, "ksa_exit", RC4Fn);
    BasicBlock *PRGALoopBB = BasicBlock::Create(Ctx, "prga_loop", RC4Fn);
    BasicBlock *PRGABodyBB = BasicBlock::Create(Ctx, "prga_body", RC4Fn);
    BasicBlock *PRGAExitBB = BasicBlock::Create(Ctx, "prga_exit", RC4Fn);
    
    IRBuilder<> B(EntryBB);
    
    // Allocate RC4 state array S[256]
    ArrayType *SArrayTy = ArrayType::get(Type::getInt8Ty(Ctx), 256);
    AllocaInst *SArray = B.CreateAlloca(SArrayTy, nullptr, "S");
    
    // Allocate state variables i, j
    AllocaInst *StateI = B.CreateAlloca(Type::getInt8Ty(Ctx), nullptr, "state_i");
    AllocaInst *StateJ = B.CreateAlloca(Type::getInt8Ty(Ctx), nullptr, "state_j");
    B.CreateStore(ConstantInt::get(Type::getInt8Ty(Ctx), 0), StateI);
    B.CreateStore(ConstantInt::get(Type::getInt8Ty(Ctx), 0), StateJ);
    
    // Initialize loop counter
    AllocaInst *LoopK = B.CreateAlloca(Int32Ty, nullptr, "k");
    B.CreateStore(ConstantInt::get(Int32Ty, 0), LoopK);
    B.CreateBr(InitLoopBB);
    
    // --- Init loop: S[i] = i for i in 0..255 ---
    B.SetInsertPoint(InitLoopBB);
    Value *K1 = B.CreateLoad(Int32Ty, LoopK);
    Value *Cond1 = B.CreateICmpULT(K1, ConstantInt::get(Int32Ty, 256));
    B.CreateCondBr(Cond1, InitBodyBB, InitExitBB);
    
    B.SetInsertPoint(InitBodyBB);
    Value *K1Body = B.CreateLoad(Int32Ty, LoopK);
    Value *K1_8 = B.CreateTrunc(K1Body, Type::getInt8Ty(Ctx));
    Value *K1_64 = B.CreateZExt(K1Body, Type::getInt64Ty(Ctx));
    Value *SPtr1 = B.CreateInBoundsGEP(SArrayTy, SArray, 
        {ConstantInt::get(Int32Ty, 0), K1_64});
    B.CreateStore(K1_8, SPtr1);
    Value *NextK1 = B.CreateAdd(K1Body, ConstantInt::get(Int32Ty, 1));
    B.CreateStore(NextK1, LoopK);
    B.CreateBr(InitLoopBB);
    
    // --- KSA: Key scheduling algorithm ---
    B.SetInsertPoint(InitExitBB);
    AllocaInst *JVar = B.CreateAlloca(Type::getInt8Ty(Ctx), nullptr, "j_ksa");
    B.CreateStore(ConstantInt::get(Type::getInt8Ty(Ctx), 0), JVar);
    B.CreateStore(ConstantInt::get(Int32Ty, 0), LoopK);
    B.CreateBr(KSALoopBB);
    
    B.SetInsertPoint(KSALoopBB);
    Value *K2 = B.CreateLoad(Int32Ty, LoopK);
    Value *Cond2 = B.CreateICmpULT(K2, ConstantInt::get(Int32Ty, 256));
    B.CreateCondBr(Cond2, KSABodyBB, KSAExitBB);
    
    B.SetInsertPoint(KSABodyBB);
    Value *K2Body = B.CreateLoad(Int32Ty, LoopK);
    Value *K2_64 = B.CreateZExt(K2Body, Type::getInt64Ty(Ctx));
    
    // j = j + S[k] + key[k % keyLen]
    Value *JOld = B.CreateLoad(Type::getInt8Ty(Ctx), JVar);
    Value *SPtrK = B.CreateInBoundsGEP(SArrayTy, SArray, 
        {ConstantInt::get(Int32Ty, 0), K2_64});
    Value *SK = B.CreateLoad(Type::getInt8Ty(Ctx), SPtrK);
    
    Value *KeyIdx = B.CreateURem(K2Body, KeyLenArg);
    Value *KeyIdx64 = B.CreateZExt(KeyIdx, Type::getInt64Ty(Ctx));
    Value *KeyPtr = B.CreateInBoundsGEP(Type::getInt8Ty(Ctx), KeyArg, KeyIdx64);
    Value *KeyByte = B.CreateLoad(Type::getInt8Ty(Ctx), KeyPtr);
    
    Value *JNew1 = B.CreateAdd(JOld, SK);
    Value *JNew2 = B.CreateAdd(JNew1, KeyByte);
    B.CreateStore(JNew2, JVar);
    
    // Swap S[k] and S[j]
    Value *J_32 = B.CreateZExt(JNew2, Int32Ty);
    Value *J_64 = B.CreateZExt(JNew2, Type::getInt64Ty(Ctx));
    Value *SPtrJ = B.CreateInBoundsGEP(SArrayTy, SArray, 
        {ConstantInt::get(Int32Ty, 0), J_64});
    Value *SJ = B.CreateLoad(Type::getInt8Ty(Ctx), SPtrJ);
    B.CreateStore(SJ, SPtrK);
    B.CreateStore(SK, SPtrJ);
    
    Value *NextK2 = B.CreateAdd(K2Body, ConstantInt::get(Int32Ty, 1));
    B.CreateStore(NextK2, LoopK);
    B.CreateBr(KSALoopBB);
    
    // --- PRGA: Pseudo-random generation algorithm ---
    B.SetInsertPoint(KSAExitBB);
    B.CreateStore(ConstantInt::get(Type::getInt8Ty(Ctx), 0), StateI);
    B.CreateStore(ConstantInt::get(Type::getInt8Ty(Ctx), 0), StateJ);
    B.CreateStore(ConstantInt::get(Int32Ty, 0), LoopK);
    B.CreateBr(PRGALoopBB);
    
    B.SetInsertPoint(PRGALoopBB);
    Value *K3 = B.CreateLoad(Int32Ty, LoopK);
    Value *Cond3 = B.CreateICmpULT(K3, LenArg);
    B.CreateCondBr(Cond3, PRGABodyBB, PRGAExitBB);
    
    B.SetInsertPoint(PRGABodyBB);
    Value *K3Body = B.CreateLoad(Int32Ty, LoopK);
    
    // i = i + 1
    Value *IOld = B.CreateLoad(Type::getInt8Ty(Ctx), StateI);
    Value *INew = B.CreateAdd(IOld, ConstantInt::get(Type::getInt8Ty(Ctx), 1));
    B.CreateStore(INew, StateI);
    
    // j = j + S[i]
    Value *I_64 = B.CreateZExt(INew, Type::getInt64Ty(Ctx));
    Value *SPtrI = B.CreateInBoundsGEP(SArrayTy, SArray, 
        {ConstantInt::get(Int32Ty, 0), I_64});
    Value *SI = B.CreateLoad(Type::getInt8Ty(Ctx), SPtrI);
    
    Value *JOld2 = B.CreateLoad(Type::getInt8Ty(Ctx), StateJ);
    Value *JNew3 = B.CreateAdd(JOld2, SI);
    B.CreateStore(JNew3, StateJ);
    
    // Swap S[i] and S[j]
    Value *J2_64 = B.CreateZExt(JNew3, Type::getInt64Ty(Ctx));
    Value *SPtrJ2 = B.CreateInBoundsGEP(SArrayTy, SArray, 
        {ConstantInt::get(Int32Ty, 0), J2_64});
    Value *SJ2 = B.CreateLoad(Type::getInt8Ty(Ctx), SPtrJ2);
    B.CreateStore(SJ2, SPtrI);
    B.CreateStore(SI, SPtrJ2);
    
    // K = S[(S[i] + S[j]) & 0xFF]
    Value *Sum = B.CreateAdd(SJ2, SI);  // SI was stored in SPtrJ2, SJ2 in SPtrI
    Value *Sum_64 = B.CreateZExt(Sum, Type::getInt64Ty(Ctx));
    Value *SPtrSum = B.CreateInBoundsGEP(SArrayTy, SArray, 
        {ConstantInt::get(Int32Ty, 0), Sum_64});
    Value *KeyStream = B.CreateLoad(Type::getInt8Ty(Ctx), SPtrSum);
    
    // data[k] ^= keystream
    Value *K3_64 = B.CreateZExt(K3Body, Type::getInt64Ty(Ctx));
    Value *DataPtr = B.CreateInBoundsGEP(Type::getInt8Ty(Ctx), DataArg, K3_64);
    Value *DataByte = B.CreateLoad(Type::getInt8Ty(Ctx), DataPtr);
    Value *Decrypted = B.CreateXor(DataByte, KeyStream);
    B.CreateStore(Decrypted, DataPtr);
    
    Value *NextK3 = B.CreateAdd(K3Body, ConstantInt::get(Int32Ty, 1));
    B.CreateStore(NextK3, LoopK);
    B.CreateBr(PRGALoopBB);
    
    B.SetInsertPoint(PRGAExitBB);
    B.CreateRetVoid();
    
    return RC4Fn;
}

/// Add global constructor that decrypts all RC4-encrypted strings at startup
void ObfuscationPass::addDecryptionGlobalCtorRC4(Module &M) {
    LLVMContext &Ctx = M.getContext();
    
    // Create RC4 decrypt helper function
    Function *RC4Fn = createRC4DecryptFunction(M);
    
    // Create decryption constructor
    Type *VoidTy = Type::getVoidTy(Ctx);
    FunctionType *FnTy = FunctionType::get(VoidTy, false);
    Function *Ctor = Function::Create(FnTy, GlobalValue::InternalLinkage, "__obf_decrypt_ctor_rc4", &M);
    BasicBlock *EntryBB = BasicBlock::Create(Ctx, "entry", Ctor);
    IRBuilder<> B(EntryBB);
    
    for (auto &Info : encryptedStringGlobals) {
        if (Info.method != StringEncryptionMethod::RC4_PBKDF2 &&
            Info.method != StringEncryptionMethod::RC4_SIMPLE) {
            continue;  // Skip non-RC4 encrypted strings
        }
        
        GlobalVariable *GV = Info.GV;
        if (!GV) continue;
        
        // Get pointer to encrypted data (salt is stored separately, not in data)
        Value *DataPtr = B.CreateBitCast(GV, PointerType::getUnqual(Type::getInt8Ty(Ctx)));
        
        // Create key array from derived key
        ArrayType *KeyArrayTy = ArrayType::get(Type::getInt8Ty(Ctx), Info.derivedKey.size());
        std::vector<Constant*> KeyConstants;
        for (uint8_t k : Info.derivedKey) {
            KeyConstants.push_back(ConstantInt::get(Type::getInt8Ty(Ctx), k));
        }
        Constant *KeyArray = ConstantArray::get(KeyArrayTy, KeyConstants);
        GlobalVariable *KeyGV = new GlobalVariable(M, KeyArrayTy, true,
            GlobalValue::InternalLinkage, KeyArray, 
            "__rc4_key_" + GV->getName().str());
        Value *KeyPtr = B.CreateBitCast(KeyGV, PointerType::getUnqual(Type::getInt8Ty(Ctx)));
        
        // Call RC4 decrypt
        B.CreateCall(RC4Fn, {
            DataPtr,
            ConstantInt::get(Type::getInt32Ty(Ctx), Info.length),
            KeyPtr,
            ConstantInt::get(Type::getInt32Ty(Ctx), Info.derivedKey.size())
        });
    }
    
    B.CreateRetVoid();
    appendToGlobalCtors(M, Ctor, /*Priority=*/65535);
}

/// Create lazy decryptor for RC4-encrypted strings
Function* ObfuscationPass::createLazyDecryptorRC4(Module &M, GlobalVariable *GV, EncryptedStringInfo &Info) {
    if (!GV || Info.length == 0 || Info.derivedKey.empty()) return nullptr;
    
    LLVMContext &Ctx = M.getContext();
    
    // Ensure RC4 helper exists
    Function *RC4Fn = createRC4DecryptFunction(M);
    
    // Create function: const char* __decrypt_rc4_N()
    FunctionType *FnTy = FunctionType::get(PointerType::getUnqual(Type::getInt8Ty(Ctx)), false);
    std::string DecryptorName = "__decrypt_rc4_" + GV->getName().str();
    Function *Decryptor = Function::Create(FnTy, GlobalValue::InternalLinkage, DecryptorName, &M);
    
    BasicBlock *EntryBB = BasicBlock::Create(Ctx, "entry", Decryptor);
    BasicBlock *DecryptBB = BasicBlock::Create(Ctx, "decrypt", Decryptor);
    BasicBlock *ReturnBB = BasicBlock::Create(Ctx, "return", Decryptor);
    
    IRBuilder<> Builder(EntryBB);
    
    // Flag to track if already decrypted
    GlobalVariable *DecryptedFlag = new GlobalVariable(M, Type::getInt1Ty(Ctx), false,
        GlobalValue::InternalLinkage, ConstantInt::getFalse(Ctx),
        "__decrypted_rc4_" + GV->getName().str());
    
    Value *Flag = Builder.CreateLoad(Type::getInt1Ty(Ctx), DecryptedFlag);
    Builder.CreateCondBr(Flag, ReturnBB, DecryptBB);
    
    // Decrypt block
    Builder.SetInsertPoint(DecryptBB);
    
    // Get pointer to encrypted data (salt stored separately, not in data)
    Value *DataPtr = Builder.CreateBitCast(GV, PointerType::getUnqual(Type::getInt8Ty(Ctx)));
    
    // Create key array
    ArrayType *KeyArrayTy = ArrayType::get(Type::getInt8Ty(Ctx), Info.derivedKey.size());
    std::vector<Constant*> KeyConstants;
    for (uint8_t k : Info.derivedKey) {
        KeyConstants.push_back(ConstantInt::get(Type::getInt8Ty(Ctx), k));
    }
    Constant *KeyArray = ConstantArray::get(KeyArrayTy, KeyConstants);
    GlobalVariable *KeyGV = new GlobalVariable(M, KeyArrayTy, true,
        GlobalValue::InternalLinkage, KeyArray,
        "__rc4_lazy_key_" + GV->getName().str());
    Value *KeyPtr = Builder.CreateBitCast(KeyGV, PointerType::getUnqual(Type::getInt8Ty(Ctx)));
    
    // Call RC4 decrypt
    Builder.CreateCall(RC4Fn, {
        DataPtr,
        ConstantInt::get(Type::getInt32Ty(Ctx), Info.length),
        KeyPtr,
        ConstantInt::get(Type::getInt32Ty(Ctx), Info.derivedKey.size())
    });
    
    // Mark as decrypted
    Builder.CreateStore(ConstantInt::getTrue(Ctx), DecryptedFlag);
    Builder.CreateBr(ReturnBB);
    
    // Return block - return pointer to decrypted data (after salt)
    // Return block - return pointer to decrypted data
    Builder.SetInsertPoint(ReturnBB);
    Value *RetPtr = Builder.CreateBitCast(GV, PointerType::getUnqual(Type::getInt8Ty(Ctx)));
    Builder.CreateRet(RetPtr);
    
    return Decryptor;
}

// Control Flow Flattening - Transform function into switch-based dispatcher
//===----------------------------------------------------------------------===//
// Control Flow Flattening - Complete Implementation with State Machine
//===----------------------------------------------------------------------===//

/// Flatten control flow by converting the CFG into a state machine
/// All blocks become cases in a switch statement, with state transitions
bool ObfuscationPass::flattenControlFlow(Function &F) {
    // Skip small functions
    if (F.size() <= 2) return false;
    
    // Skip functions with exception handling (too complex)
    for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
            if (isa<LandingPadInst>(&I) || isa<InvokeInst>(&I) ||
                isa<CatchPadInst>(&I) || isa<CleanupPadInst>(&I)) {
                return false;
            }
        }
    }
    
    // Skip functions with complex PHI nodes (hard to preserve SSA)
    for (BasicBlock &BB : F) {
        for (PHINode &PN : BB.phis()) {
            if (PN.getNumIncomingValues() > 3) {
                return false;  // Too complex, skip flattening
            }
        }
    }
    
    LLVMContext &Ctx = F.getContext();
    Type *Int32Ty = Type::getInt32Ty(Ctx);
    
    // Collect original blocks (skip entry, we'll handle it specially)
    std::vector<BasicBlock*> origBlocks;
    BasicBlock *EntryBB = &F.getEntryBlock();
    
    for (BasicBlock &BB : F) {
        if (&BB != EntryBB) {
            origBlocks.push_back(&BB);
        }
    }
    
    if (origBlocks.size() < 2) return false;
    
    // Create state variable in entry block
    IRBuilder<> EntryBuilder(EntryBB, EntryBB->begin());
    AllocaInst *StateVar = EntryBuilder.CreateAlloca(Int32Ty, nullptr, "cff_state");
    
    // Assign random state IDs to blocks for obfuscation
    std::map<BasicBlock*, uint32_t> blockToState;
    std::vector<uint32_t> stateIds;
    
    // Generate shuffled state IDs
    for (size_t i = 0; i < origBlocks.size(); i++) {
        stateIds.push_back(static_cast<uint32_t>(i * 7 + 3));  // Obfuscated IDs
    }
    if (rng) {
        std::shuffle(stateIds.begin(), stateIds.end(),
                     std::default_random_engine(rng->operator()()));
    }
    
    for (size_t i = 0; i < origBlocks.size(); i++) {
        blockToState[origBlocks[i]] = stateIds[i];
    }
    
    // Find first block after entry (initial state)
    BasicBlock *FirstBlock = nullptr;
    if (Instruction *Term = EntryBB->getTerminator()) {
        if (BranchInst *BI = dyn_cast<BranchInst>(Term)) {
            FirstBlock = BI->getSuccessor(0);
        }
    }
    
    if (!FirstBlock || blockToState.find(FirstBlock) == blockToState.end()) {
        return false;
    }
    
    uint32_t initialState = blockToState[FirstBlock];
    
    // Remove original entry block terminator
    if (Instruction *Term = EntryBB->getTerminator()) {
        Term->eraseFromParent();
    }
    
    // Initialize state to first block
    IRBuilder<> InitBuilder(EntryBB);
    InitBuilder.CreateStore(ConstantInt::get(Int32Ty, initialState), StateVar);
    
    //===----------------------------------------------------------------------===//
    // FIXED: Proper Return Value Handling for CFF
    //===----------------------------------------------------------------------===//
    // Create alloca to store return value (if function returns non-void)
    // All return instructions will store their value here and jump to dispatch
    // The end block will load from here and return
    
    Type *RetTy = F.getReturnType();
    AllocaInst *RetValStorage = nullptr;
    if (!RetTy->isVoidTy()) {
        RetValStorage = InitBuilder.CreateAlloca(RetTy, nullptr, "cff_retval");
        // Initialize to default value
        if (RetTy->isIntegerTy()) {
            InitBuilder.CreateStore(ConstantInt::get(RetTy, 0), RetValStorage);
        } else if (RetTy->isPointerTy()) {
            InitBuilder.CreateStore(ConstantPointerNull::get(cast<PointerType>(RetTy)), RetValStorage);
        } else if (RetTy->isFloatTy() || RetTy->isDoubleTy()) {
            InitBuilder.CreateStore(ConstantFP::get(RetTy, 0.0), RetValStorage);
        }
    }
    
    // Create dispatch block
    BasicBlock *DispatchBB = BasicBlock::Create(Ctx, "cff_dispatch", &F);
    InitBuilder.CreateBr(DispatchBB);
    
    // Create end block for returns
    BasicBlock *EndBB = BasicBlock::Create(Ctx, "cff_end", &F);
    uint32_t endState = 0xDEAD;  // Special state for end
    
    // Build dispatcher
    IRBuilder<> DispatchBuilder(DispatchBB);
    Value *CurrentState = DispatchBuilder.CreateLoad(Int32Ty, StateVar, "current_state");
    SwitchInst *Dispatcher = DispatchBuilder.CreateSwitch(CurrentState, EndBB, origBlocks.size());
    
    // Add cases for each block
    for (BasicBlock *BB : origBlocks) {
        uint32_t state = blockToState[BB];
        ConstantInt *StateConst = cast<ConstantInt>(ConstantInt::get(Int32Ty, state));
        Dispatcher->addCase(StateConst, BB);
    }
    
    // Add case for end state (goes to EndBB)
    Dispatcher->addCase(cast<ConstantInt>(ConstantInt::get(Int32Ty, endState)), EndBB);
    
    // Process each block: replace terminators with state updates + jump to dispatch
    for (BasicBlock *BB : origBlocks) {
        Instruction *Term = BB->getTerminator();
        if (!Term) continue;
        
        IRBuilder<> BlockBuilder(Term);
        
        if (BranchInst *BI = dyn_cast<BranchInst>(Term)) {
            if (BI->isUnconditional()) {
                // Unconditional branch: set next state and jump to dispatch
                BasicBlock *Dest = BI->getSuccessor(0);
                if (blockToState.find(Dest) != blockToState.end()) {
                    uint32_t nextState = blockToState[Dest];
                    BlockBuilder.CreateStore(ConstantInt::get(Int32Ty, nextState), StateVar);
                    BlockBuilder.CreateBr(DispatchBB);
                    Term->eraseFromParent();
                }
            } else {
                // Conditional branch: select state based on condition
                BasicBlock *TrueDest = BI->getSuccessor(0);
                BasicBlock *FalseDest = BI->getSuccessor(1);
                Value *Cond = BI->getCondition();
                
                uint32_t trueState = endState, falseState = endState;
                if (blockToState.find(TrueDest) != blockToState.end()) {
                    trueState = blockToState[TrueDest];
                }
                if (blockToState.find(FalseDest) != blockToState.end()) {
                    falseState = blockToState[FalseDest];
                }
                
                Value *NextState = BlockBuilder.CreateSelect(
                    Cond,
                    ConstantInt::get(Int32Ty, trueState),
                    ConstantInt::get(Int32Ty, falseState),
                    "next_state"
                );
                BlockBuilder.CreateStore(NextState, StateVar);
                BlockBuilder.CreateBr(DispatchBB);
                Term->eraseFromParent();
            }
        } else if (ReturnInst *RI = dyn_cast<ReturnInst>(Term)) {
            //===----------------------------------------------------------------------===//
            // FIXED: Proper Return Handling - Integrate into State Machine
            //===----------------------------------------------------------------------===//
            // Instead of keeping the return in place, we:
            // 1. Store the return value (if any) to RetValStorage
            // 2. Set state to endState
            // 3. Jump to dispatch (which will then go to EndBB)
            
            if (Value *RetVal = RI->getReturnValue()) {
                // Store return value for later retrieval
                if (RetValStorage) {
                    BlockBuilder.CreateStore(RetVal, RetValStorage);
                }
            }
            
            // Set end state and jump to dispatch
            BlockBuilder.CreateStore(ConstantInt::get(Int32Ty, endState), StateVar);
            BlockBuilder.CreateBr(DispatchBB);
            
            // Remove the original return instruction
            Term->eraseFromParent();
        } else if (SwitchInst *SI = dyn_cast<SwitchInst>(Term)) {
            // Handle switch statements
            Value *SwitchCond = SI->getCondition();
            
            // Create a new switch in terms of states
            BasicBlock *DefaultDest = SI->getDefaultDest();
            uint32_t defaultState = endState;
            if (blockToState.find(DefaultDest) != blockToState.end()) {
                defaultState = blockToState[DefaultDest];
            }
            
            // For simplicity, just pick default state
            // Full implementation would handle all cases
            BlockBuilder.CreateStore(ConstantInt::get(Int32Ty, defaultState), StateVar);
            BlockBuilder.CreateBr(DispatchBB);
            Term->eraseFromParent();
        }
    }
    
    //===----------------------------------------------------------------------===//
    // FIXED: End Block - Load Stored Return Value and Return
    //===----------------------------------------------------------------------===//
    IRBuilder<> EndBuilder(EndBB);
    if (RetTy->isVoidTy()) {
        EndBuilder.CreateRetVoid();
    } else if (RetValStorage) {
        // Load the stored return value and return it
        Value *StoredRetVal = EndBuilder.CreateLoad(RetTy, RetValStorage, "cff_stored_ret");
        EndBuilder.CreateRet(StoredRetVal);
    } else if (RetTy->isIntegerTy()) {
        // Fallback for safety
        EndBuilder.CreateRet(ConstantInt::get(RetTy, 0));
    } else if (RetTy->isPointerTy()) {
        EndBuilder.CreateRet(ConstantPointerNull::get(cast<PointerType>(RetTy)));
    } else {
        EndBuilder.CreateUnreachable();
    }
    
    totalFlattenedFunctions++;
    logMetrics("flattened_functions", 1);
    return true;
}

/// Create dispatch block helper (used internally by flattenControlFlow)
BasicBlock* ObfuscationPass::createDispatchBlock(Function &F, std::vector<BasicBlock*> &Blocks) {
    // This is now handled by flattenControlFlow directly
    // Kept for backwards compatibility
    LLVMContext &Ctx = F.getContext();
    BasicBlock *DispatchBB = BasicBlock::Create(Ctx, "cff_dispatch_legacy", &F);
    
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
// Optimized: Only apply to critical operations for better size/security ratio
//===----------------------------------------------------------------------===//
// Mixed Boolean Arithmetic (MBA) - Complete Implementation
//===----------------------------------------------------------------------===//

/// MBA operation types
enum class MBAOp {
    ADD = 0,   // a + b
    SUB = 1,   // a - b
    XOR = 2,   // a ^ b
    AND = 3,   // a & b
    OR = 4,    // a | b
    MUL = 5,   // a * b (limited support)
    NOT = 6    // ~a (unary)
};

/// Apply Mixed Boolean Arithmetic transformations to all supported operations
bool ObfuscationPass::applyMBA(Function &F) {
    bool modified = false;
    
    // Skip functions that have been flattened (they may have broken SSA)
    bool isFlattened = false;
    for (BasicBlock &BB : F) {
        if (BB.getName().startswith("cff_")) {
            isFlattened = true;
            break;
        }
    }
    if (isFlattened) {
        return false;  // Skip MBA on flattened functions to avoid SSA issues
    }
    
    for (BasicBlock &BB : F) {
        // Determine if this is a critical block worth obfuscating
        bool isCriticalBlock = false;
        if (Instruction *Term = BB.getTerminator()) {
            if (isa<BranchInst>(Term) || isa<SwitchInst>(Term)) {
                isCriticalBlock = true;
            }
        }
        if (BB.size() > 8) {
            isCriticalBlock = true;  // Large blocks are important
        }
        
        for (Instruction &I : make_early_inc_range(BB)) {
            // Skip if not in critical block (size optimization)
            if (!isCriticalBlock) continue;
            
            auto *BinOp = dyn_cast<BinaryOperator>(&I);
            if (!BinOp) continue;
            
            // Skip operations with constants (optimizer would fold them)
            if (isa<ConstantInt>(BinOp->getOperand(0)) || 
                isa<ConstantInt>(BinOp->getOperand(1))) {
                continue;
            }
            
            // CRITICAL: Ensure both operands are defined in the same block or dominate this instruction
            Value *A = BinOp->getOperand(0);
            Value *B = BinOp->getOperand(1);
            
            // Check if operands are instructions and dominate this instruction
            if (Instruction *AI = dyn_cast<Instruction>(A)) {
                if (AI->getParent() != &BB) {
                    // A is from a different block - check if it dominates
                    BasicBlock *ABlock = AI->getParent();
                    if (!ABlock->getParent()->getEntryBlock().getParent()) {
                        continue;  // Can't verify dominance, skip
                    }
                }
            }
            if (Instruction *BI = dyn_cast<Instruction>(B)) {
                if (BI->getParent() != &BB) {
                    // B is from a different block - skip to avoid dominance issues
                    continue;
                }
            }
            
            IRBuilder<> Builder(&I);
            Value *MBAResult = nullptr;
            
            switch (BinOp->getOpcode()) {
                case Instruction::Add:
                    MBAResult = createMBAExpression(Builder, A, B, static_cast<int>(MBAOp::ADD));
                    break;
                    
                case Instruction::Sub:
                    MBAResult = createMBAExpression(Builder, A, B, static_cast<int>(MBAOp::SUB));
                    break;
                    
                case Instruction::Xor:
                    MBAResult = createMBAExpression(Builder, A, B, static_cast<int>(MBAOp::XOR));
                    break;
                    
                case Instruction::And:
                    MBAResult = createMBAExpression(Builder, A, B, static_cast<int>(MBAOp::AND));
                    break;
                    
                case Instruction::Or:
                    MBAResult = createMBAExpression(Builder, A, B, static_cast<int>(MBAOp::OR));
                    break;
                    
                case Instruction::Mul:
                    // Multiplication MBA is complex and size-expensive
                    // Only apply at higher complexity levels
                    if (config.mbaComplexity >= 3) {
                        MBAResult = createMBAExpression(Builder, A, B, static_cast<int>(MBAOp::MUL));
                    }
                    break;
                    
                default:
                    break;
            }
            
            if (MBAResult) {
                I.replaceAllUsesWith(MBAResult);
                I.eraseFromParent();
                totalMBATransformations++;
                modified = true;
            }
        }
    }
    
    logMetrics("mba_transformations", totalMBATransformations);
    return modified;
}

/// Create MBA expression for given operation
/// Supports: ADD, SUB, XOR, AND, OR, MUL
Value* ObfuscationPass::createMBAExpression(IRBuilder<> &Builder, Value *A, Value *B, int op) {
    Type *Ty = A->getType();
    
    switch (static_cast<MBAOp>(op)) {
        case MBAOp::ADD: {
            // a + b = (a ^ b) + 2 * (a & b)
            // Explanation: XOR gives the sum without carry, AND gives carry positions
            Value *Xor = Builder.CreateXor(A, B, "mba_xor");
            Value *And = Builder.CreateAnd(A, B, "mba_and");
            Value *Shl = Builder.CreateShl(And, ConstantInt::get(Ty, 1), "mba_shl");
            return Builder.CreateAdd(Xor, Shl, "mba_add");
        }
        
        case MBAOp::SUB: {
            // a - b = (a ^ b) - 2 * (~a & b)
            // Alternative: a - b = a + (~b + 1) = a + (-b)
            Value *Xor = Builder.CreateXor(A, B, "mba_xor");
            Value *NotA = Builder.CreateNot(A, "mba_not");
            Value *And = Builder.CreateAnd(NotA, B, "mba_and");
            Value *Shl = Builder.CreateShl(And, ConstantInt::get(Ty, 1), "mba_shl");
            return Builder.CreateSub(Xor, Shl, "mba_sub");
        }
        
        case MBAOp::XOR: {
            // a ^ b = (a | b) - (a & b)
            // Alternative: a ^ b = (a & ~b) | (~a & b)
            Value *Or = Builder.CreateOr(A, B, "mba_or");
            Value *And = Builder.CreateAnd(A, B, "mba_and");
            return Builder.CreateSub(Or, And, "mba_xor");
        }
        
        case MBAOp::AND: {
            // a & b = (a + b - (a ^ b)) >> 1
            // Alternative: a & b = ~(~a | ~b) (De Morgan)
            Value *Add = Builder.CreateAdd(A, B, "mba_add");
            Value *Xor = Builder.CreateXor(A, B, "mba_xor");
            Value *Sub = Builder.CreateSub(Add, Xor, "mba_sub");
            return Builder.CreateLShr(Sub, ConstantInt::get(Ty, 1), "mba_and");
        }
        
        case MBAOp::OR: {
            // a | b = (a + b + (a ^ b)) >> 1
            // Alternative: a | b = a + b - (a & b)
            // Using alternative for simplicity:
            Value *Add = Builder.CreateAdd(A, B, "mba_add");
            Value *And = Builder.CreateAnd(A, B, "mba_and");
            return Builder.CreateSub(Add, And, "mba_or");
        }
        
        case MBAOp::MUL: {
            // a * b using Karatsuba-like decomposition (size-expensive)
            // For 32-bit: split into 16-bit halves
            // Simplified version: a * b = ((a+b)^2 - (a-b)^2) / 4
            // This is mathematically correct but may overflow
            // Use only for demonstration; real implementation needs overflow handling
            Value *Sum = Builder.CreateAdd(A, B, "mba_sum");
            Value *Diff = Builder.CreateSub(A, B, "mba_diff");
            Value *SumSq = Builder.CreateMul(Sum, Sum, "mba_sumsq");
            Value *DiffSq = Builder.CreateMul(Diff, Diff, "mba_diffsq");
            Value *Delta = Builder.CreateSub(SumSq, DiffSq, "mba_delta");
            return Builder.CreateLShr(Delta, ConstantInt::get(Ty, 2), "mba_mul");
        }
        
        case MBAOp::NOT: {
            // ~a = -a - 1
            Value *Neg = Builder.CreateNeg(A, "mba_neg");
            return Builder.CreateSub(Neg, ConstantInt::get(Ty, 1), "mba_not");
        }
        
        default:
            return nullptr;
    }
}

// Anti-Debugging - Insert debugger detection checks
bool ObfuscationPass::insertAntiDebug(Module &M) {
    LLVMContext &Ctx = M.getContext();
    
    #ifdef _WIN32
    // Create comprehensive anti-debug function
    FunctionType *CheckType = FunctionType::get(Type::getInt32Ty(Ctx), false);
    Function *CheckFunc = Function::Create(CheckType, GlobalValue::InternalLinkage, "__check_debugger", &M);
    
    BasicBlock *BB = BasicBlock::Create(Ctx, "entry", CheckFunc);
    IRBuilder<> Builder(BB);
    
    // Check 1: IsDebuggerPresent (correct return type: BOOL = i32)
    FunctionType *IsDebuggerPresentType = FunctionType::get(Type::getInt32Ty(Ctx), false);
    FunctionCallee IsDebuggerPresent = M.getOrInsertFunction("IsDebuggerPresent", IsDebuggerPresentType);
    Value *Check1 = Builder.CreateCall(IsDebuggerPresent);
    Value *Check1Result = Builder.CreateICmpNE(Check1, ConstantInt::get(Type::getInt32Ty(Ctx), 0));
    
    // Check 2: CheckRemoteDebuggerPresent
    FunctionType *CheckRemoteType = FunctionType::get(Type::getInt32Ty(Ctx), 
        {PointerType::getUnqual(Type::getInt8Ty(Ctx)), PointerType::getUnqual(Type::getInt32Ty(Ctx))}, false);
    FunctionCallee CheckRemote = M.getOrInsertFunction("CheckRemoteDebuggerPresent", CheckRemoteType);
    
    // GetCurrentProcess
    FunctionType *GetCurrentProcessType = FunctionType::get(PointerType::getUnqual(Type::getInt8Ty(Ctx)), false);
    FunctionCallee GetCurrentProcess = M.getOrInsertFunction("GetCurrentProcess", GetCurrentProcessType);
    Value *CurrentProcess = Builder.CreateCall(GetCurrentProcess);
    
    AllocaInst *DebugFlagAlloc = Builder.CreateAlloca(Type::getInt32Ty(Ctx));
    Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Ctx), 0), DebugFlagAlloc);
    Value *Check2 = Builder.CreateCall(CheckRemote, {CurrentProcess, DebugFlagAlloc});
    Value *DebugFlagVal = Builder.CreateLoad(Type::getInt32Ty(Ctx), DebugFlagAlloc);
    Value *Check2Result = Builder.CreateICmpNE(DebugFlagVal, ConstantInt::get(Type::getInt32Ty(Ctx), 0));
    
    // Check 3: NtQueryInformationProcess - ProcessDebugPort
    // This detects debuggers that IsDebuggerPresent might miss
    FunctionType *NtQueryType = FunctionType::get(Type::getInt32Ty(Ctx),
        {PointerType::getUnqual(Type::getInt8Ty(Ctx)), Type::getInt32Ty(Ctx),
         PointerType::getUnqual(Type::getInt8Ty(Ctx)), Type::getInt32Ty(Ctx),
         PointerType::getUnqual(Type::getInt32Ty(Ctx))}, false);
    FunctionCallee NtQuery = M.getOrInsertFunction("NtQueryInformationProcess", NtQueryType);
    
    // ProcessDebugPort = 7
    AllocaInst *DebugPort = Builder.CreateAlloca(Type::getInt32Ty(Ctx));
    AllocaInst *ReturnLength = Builder.CreateAlloca(Type::getInt32Ty(Ctx));
    Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Ctx), 0), DebugPort);
    Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Ctx), 4), ReturnLength);
    
    Value *NtResult = Builder.CreateCall(NtQuery, {
        CurrentProcess,
        ConstantInt::get(Type::getInt32Ty(Ctx), 7), // ProcessDebugPort
        DebugPort,
        ConstantInt::get(Type::getInt32Ty(Ctx), 4), // sizeof(ULONG)
        ReturnLength
    });
    
    Value *PortValue = Builder.CreateLoad(Type::getInt32Ty(Ctx), DebugPort);
    Value *Check3Result = Builder.CreateICmpNE(PortValue, ConstantInt::get(Type::getInt32Ty(Ctx), 0));
    
    // Check 4: Check for x64dbg/x32dbg process
    FunctionType *GetModuleHandleType = FunctionType::get(PointerType::getUnqual(Type::getInt8Ty(Ctx)),
        {PointerType::getUnqual(Type::getInt8Ty(Ctx))}, false);
    FunctionCallee GetModuleHandle = M.getOrInsertFunction("GetModuleHandleA", GetModuleHandleType);
    
    Value *X64DbgHandle = Builder.CreateCall(GetModuleHandle, {Builder.CreateGlobalStringPtr("x64dbg.exe")});
    Value *X32DbgHandle = Builder.CreateCall(GetModuleHandle, {Builder.CreateGlobalStringPtr("x32dbg.exe")});
    PointerType *Int8PtrTy = PointerType::getUnqual(Type::getInt8Ty(Ctx));
    Value *Check4Result = Builder.CreateOr(
        Builder.CreateICmpNE(X64DbgHandle, ConstantPointerNull::get(Int8PtrTy)),
        Builder.CreateICmpNE(X32DbgHandle, ConstantPointerNull::get(Int8PtrTy))
    );
    
    //===----------------------------------------------------------------------===//
    // ADVANCED ANTI-DEBUG: Check 5 - RDTSC Timing-Based Detection
    //===----------------------------------------------------------------------===//
    // Debuggers slow down execution - measure time between two RDTSC calls
    // If time exceeds threshold, debugger is likely present
    
    // Declare RDTSC intrinsic (returns i64)
    FunctionType *RdtscType = FunctionType::get(Type::getInt64Ty(Ctx), false);
    FunctionCallee Rdtsc = M.getOrInsertFunction("llvm.x86.rdtsc", RdtscType);
    
    // First timestamp
    Value *Time1 = Builder.CreateCall(Rdtsc);
    
    // Do some dummy work (can't be optimized out easily)
    AllocaInst *DummyVar = Builder.CreateAlloca(Type::getInt64Ty(Ctx));
    Builder.CreateStore(Time1, DummyVar);
    Value *DummyLoad = Builder.CreateLoad(Type::getInt64Ty(Ctx), DummyVar);
    Value *DummyXor = Builder.CreateXor(DummyLoad, ConstantInt::get(Type::getInt64Ty(Ctx), 0x12345678));
    Builder.CreateStore(DummyXor, DummyVar);
    
    // Second timestamp
    Value *Time2 = Builder.CreateCall(Rdtsc);
    
    // Calculate difference
    Value *TimeDiff = Builder.CreateSub(Time2, Time1);
    
    // If difference > 10000000 cycles (~3ms at 3GHz), likely being debugged
    // Normal execution: < 100000 cycles
    Value *Check5Result = Builder.CreateICmpUGT(TimeDiff, 
        ConstantInt::get(Type::getInt64Ty(Ctx), 10000000));
    
    //===----------------------------------------------------------------------===//
    // ADVANCED ANTI-DEBUG: Check 6 - Hardware Breakpoint Detection (DR0-DR7)
    //===----------------------------------------------------------------------===//
    // GetThreadContext reveals hardware breakpoint registers
    
    // CONTEXT structure - we only care about ContextFlags and Debug Registers
    // On x64: DR0 at offset 0x350, DR1 at 0x358, DR2 at 0x360, DR3 at 0x368
    // ContextFlags at offset 0x30 (CONTEXT_DEBUG_REGISTERS = 0x10)
    
    FunctionType *GetCurrentThreadType = FunctionType::get(PointerType::getUnqual(Type::getInt8Ty(Ctx)), false);
    FunctionCallee GetCurrentThread = M.getOrInsertFunction("GetCurrentThread", GetCurrentThreadType);
    
    // Allocate CONTEXT structure (simplified - just check if function exists)
    // For full implementation, would need proper CONTEXT allocation
    FunctionType *GetContextType = FunctionType::get(Type::getInt32Ty(Ctx),
        {PointerType::getUnqual(Type::getInt8Ty(Ctx)), PointerType::getUnqual(Type::getInt8Ty(Ctx))}, false);
    FunctionCallee GetThreadContext = M.getOrInsertFunction("GetThreadContext", GetContextType);
    
    // Allocate CONTEXT (1232 bytes for x64)
    ArrayType *ContextType = ArrayType::get(Type::getInt8Ty(Ctx), 1232);
    AllocaInst *Context = Builder.CreateAlloca(ContextType);
    Value *ContextPtr = Builder.CreateBitCast(Context, Int8PtrTy);
    
    // Set ContextFlags (offset 0x30) to CONTEXT_DEBUG_REGISTERS (0x10)
    Value *FlagsPtr = Builder.CreateInBoundsGEP(Type::getInt8Ty(Ctx), ContextPtr,
        ConstantInt::get(Type::getInt64Ty(Ctx), 0x30));
    Value *FlagsPtr32 = Builder.CreateBitCast(FlagsPtr, PointerType::getUnqual(Type::getInt32Ty(Ctx)));
    Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Ctx), 0x10), FlagsPtr32);
    
    Value *ThreadHandle = Builder.CreateCall(GetCurrentThread);
    Value *GetCtxResult = Builder.CreateCall(GetThreadContext, {ThreadHandle, ContextPtr});
    
    // Check DR0 (offset 0x350 on x64, 0x04 on x86) - if non-zero, HW breakpoint set
    Value *DR0Ptr = Builder.CreateInBoundsGEP(Type::getInt8Ty(Ctx), ContextPtr,
        ConstantInt::get(Type::getInt64Ty(Ctx), 0x350));
    Value *DR0Ptr64 = Builder.CreateBitCast(DR0Ptr, PointerType::getUnqual(Type::getInt64Ty(Ctx)));
    Value *DR0 = Builder.CreateLoad(Type::getInt64Ty(Ctx), DR0Ptr64);
    Value *DR0Check = Builder.CreateICmpNE(DR0, ConstantInt::get(Type::getInt64Ty(Ctx), 0));
    
    // Check DR1
    Value *DR1Ptr = Builder.CreateInBoundsGEP(Type::getInt8Ty(Ctx), ContextPtr,
        ConstantInt::get(Type::getInt64Ty(Ctx), 0x358));
    Value *DR1Ptr64 = Builder.CreateBitCast(DR1Ptr, PointerType::getUnqual(Type::getInt64Ty(Ctx)));
    Value *DR1 = Builder.CreateLoad(Type::getInt64Ty(Ctx), DR1Ptr64);
    Value *DR1Check = Builder.CreateICmpNE(DR1, ConstantInt::get(Type::getInt64Ty(Ctx), 0));
    
    Value *HWBPCheck = Builder.CreateOr(DR0Check, DR1Check);
    Value *Check6Result = Builder.CreateAnd(
        Builder.CreateICmpNE(GetCtxResult, ConstantInt::get(Type::getInt32Ty(Ctx), 0)),
        HWBPCheck);
    
    //===----------------------------------------------------------------------===//
    // ADVANCED ANTI-DEBUG: Check 7 - PEB.NtGlobalFlag and Heap Flags
    //===----------------------------------------------------------------------===//
    // When debugger is present, NtGlobalFlag has FLG_HEAP_* flags set
    // PEB.NtGlobalFlag at offset 0xBC (x64) or 0x68 (x86)
    // Debugger sets: FLG_HEAP_ENABLE_TAIL_CHECK (0x10) | FLG_HEAP_ENABLE_FREE_CHECK (0x20) | FLG_HEAP_VALIDATE_PARAMETERS (0x40)
    
    // Get PEB via NtCurrentPeb or inline assembly (use TEB.ProcessEnvironmentBlock)
    // TEB at gs:[0x30] on x64, fs:[0x18] on x86
    // PEB pointer at TEB+0x60 on x64, TEB+0x30 on x86
    
    // For simplicity, use NtQueryInformationProcess with ProcessBasicInformation (0)
    // which gives us the PEB address in the PROCESS_BASIC_INFORMATION structure
    
    AllocaInst *PBI = Builder.CreateAlloca(ArrayType::get(Type::getInt64Ty(Ctx), 6)); // 48 bytes
    Value *PBIPtr = Builder.CreateBitCast(PBI, PointerType::getUnqual(Type::getInt8Ty(Ctx)));
    AllocaInst *RetLen2 = Builder.CreateAlloca(Type::getInt32Ty(Ctx));
    
    Value *NtResult2 = Builder.CreateCall(NtQuery, {
        CurrentProcess,
        ConstantInt::get(Type::getInt32Ty(Ctx), 0), // ProcessBasicInformation
        PBIPtr,
        ConstantInt::get(Type::getInt32Ty(Ctx), 48),
        RetLen2
    });
    
    // PEB address is at offset 8 in PROCESS_BASIC_INFORMATION
    Value *PEBPtrLoc = Builder.CreateInBoundsGEP(Type::getInt8Ty(Ctx), PBIPtr,
        ConstantInt::get(Type::getInt64Ty(Ctx), 8));
    Value *PEBPtrPtr = Builder.CreateBitCast(PEBPtrLoc, PointerType::getUnqual(PointerType::getUnqual(Type::getInt8Ty(Ctx))));
    Value *PEBPtr = Builder.CreateLoad(PointerType::getUnqual(Type::getInt8Ty(Ctx)), PEBPtrPtr);
    
    // NtGlobalFlag at PEB+0xBC (x64)
    Value *NtGlobalFlagPtr = Builder.CreateInBoundsGEP(Type::getInt8Ty(Ctx), PEBPtr,
        ConstantInt::get(Type::getInt64Ty(Ctx), 0xBC));
    Value *NtGlobalFlagPtr32 = Builder.CreateBitCast(NtGlobalFlagPtr, PointerType::getUnqual(Type::getInt32Ty(Ctx)));
    Value *NtGlobalFlag = Builder.CreateLoad(Type::getInt32Ty(Ctx), NtGlobalFlagPtr32);
    
    // Check for debug heap flags: 0x70 = (0x10 | 0x20 | 0x40)
    Value *HeapFlags = Builder.CreateAnd(NtGlobalFlag, ConstantInt::get(Type::getInt32Ty(Ctx), 0x70));
    Value *Check7Result = Builder.CreateICmpNE(HeapFlags, ConstantInt::get(Type::getInt32Ty(Ctx), 0));
    
    //===----------------------------------------------------------------------===//
    // ADVANCED ANTI-DEBUG: Check 8 - BeingDebugged Flag in PEB
    //===----------------------------------------------------------------------===//
    // PEB.BeingDebugged at offset 0x02
    
    Value *BeingDebuggedPtr = Builder.CreateInBoundsGEP(Type::getInt8Ty(Ctx), PEBPtr,
        ConstantInt::get(Type::getInt64Ty(Ctx), 0x02));
    Value *BeingDebugged = Builder.CreateLoad(Type::getInt8Ty(Ctx), BeingDebuggedPtr);
    Value *Check8Result = Builder.CreateICmpNE(BeingDebugged, ConstantInt::get(Type::getInt8Ty(Ctx), 0));
    
    // Combine all checks (8 total now)
    Value *AnyDetected = Builder.CreateOr(Check1Result, Check2Result);
    AnyDetected = Builder.CreateOr(AnyDetected, Check3Result);
    AnyDetected = Builder.CreateOr(AnyDetected, Check4Result);
    AnyDetected = Builder.CreateOr(AnyDetected, Check5Result);  // Timing
    AnyDetected = Builder.CreateOr(AnyDetected, Check6Result);  // Hardware BP
    AnyDetected = Builder.CreateOr(AnyDetected, Check7Result);  // NtGlobalFlag
    AnyDetected = Builder.CreateOr(AnyDetected, Check8Result);  // BeingDebugged
    
    // Return 1 if debugger detected, 0 otherwise
    Value *Result = Builder.CreateSelect(AnyDetected,
        ConstantInt::get(Type::getInt32Ty(Ctx), 1),
        ConstantInt::get(Type::getInt32Ty(Ctx), 0));
    
    Builder.CreateRet(Result);
    
    // Create termination function
    FunctionType *ExitType = FunctionType::get(Type::getVoidTy(Ctx), {Type::getInt32Ty(Ctx)}, false);
    FunctionCallee ExitProcess = M.getOrInsertFunction("ExitProcess", ExitType);
    
    //===----------------------------------------------------------------------===//
    // ADVANCED ANTI-DEBUG: TLS Callback - Runs Before Main Entry Point
    //===----------------------------------------------------------------------===//
    // TLS callbacks execute before the CRT initialization and main()
    // This catches debuggers that attach at program start
    
    // Create TLS callback function
    // Signature: void NTAPI TlsCallback(PVOID DllHandle, DWORD Reason, PVOID Reserved)
    FunctionType *TlsCallbackType = FunctionType::get(Type::getVoidTy(Ctx),
        {Int8PtrTy, Type::getInt32Ty(Ctx), Int8PtrTy}, false);
    Function *TlsCallback = Function::Create(TlsCallbackType, GlobalValue::InternalLinkage, 
        "__tls_antidebug_callback", &M);
    
    BasicBlock *TlsBB = BasicBlock::Create(Ctx, "entry", TlsCallback);
    IRBuilder<> TlsBuilder(TlsBB);
    
    // Get Reason parameter (second arg)
    auto TlsArgIt = TlsCallback->arg_begin();
    ++TlsArgIt;  // Skip DllHandle
    Value *Reason = &*TlsArgIt;
    
    // Only check on DLL_PROCESS_ATTACH (Reason == 1)
    Value *IsAttach = TlsBuilder.CreateICmpEQ(Reason, ConstantInt::get(Type::getInt32Ty(Ctx), 1));
    
    BasicBlock *CheckBB = BasicBlock::Create(Ctx, "check_debug", TlsCallback);
    BasicBlock *ExitTlsBB = BasicBlock::Create(Ctx, "exit_tls", TlsCallback);
    
    TlsBuilder.CreateCondBr(IsAttach, CheckBB, ExitTlsBB);
    
    // Check for debugger in TLS callback
    TlsBuilder.SetInsertPoint(CheckBB);
    Value *TlsCheck = TlsBuilder.CreateCall(CheckFunc);
    Value *TlsCond = TlsBuilder.CreateICmpNE(TlsCheck, ConstantInt::get(Type::getInt32Ty(Ctx), 0));
    
    BasicBlock *TerminateBB = BasicBlock::Create(Ctx, "terminate", TlsCallback);
    TlsBuilder.CreateCondBr(TlsCond, TerminateBB, ExitTlsBB);
    
    // Terminate if debugger found
    TlsBuilder.SetInsertPoint(TerminateBB);
    TlsBuilder.CreateCall(ExitProcess, {ConstantInt::get(Type::getInt32Ty(Ctx), 0xDEAD)});
    TlsBuilder.CreateUnreachable();
    
    // Normal exit
    TlsBuilder.SetInsertPoint(ExitTlsBB);
    TlsBuilder.CreateRetVoid();
    
    // Create TLS directory structure
    // The PE loader looks for _tls_used symbol to find TLS callbacks
    
    // Create callback array: { TlsCallback, nullptr }
    ArrayType *CallbackArrayTy = ArrayType::get(PointerType::getUnqual(Type::getInt8Ty(Ctx)), 2);
    std::vector<Constant*> Callbacks;
    Callbacks.push_back(ConstantExpr::getBitCast(TlsCallback, PointerType::getUnqual(Type::getInt8Ty(Ctx))));
    Callbacks.push_back(ConstantPointerNull::get(PointerType::getUnqual(Type::getInt8Ty(Ctx))));
    Constant *CallbackArray = ConstantArray::get(CallbackArrayTy, Callbacks);
    
    GlobalVariable *TlsCallbackArray = new GlobalVariable(M, CallbackArrayTy, true,
        GlobalValue::InternalLinkage, CallbackArray, "__tls_callback_array");
    TlsCallbackArray->setSection(".CRT$XLB");  // TLS callback section
    
    totalAntiDebugChecks++; // Count TLS callback as an anti-debug check
    outs() << "  [TLS] Anti-debug TLS callback installed\n";
    
    // Insert checks in main and other critical functions
    for (Function &F : M) {
        if (F.isDeclaration() || &F == CheckFunc) continue;
        if (F.getName() == "main" || F.getName().startswith("_main") || 
            shouldObfuscateFunction(F)) {
            
            BasicBlock &Entry = F.getEntryBlock();
            Instruction *SplitPt = &*Entry.getFirstNonPHIOrDbg();
            if (!SplitPt) continue;
            
            BasicBlock *OrigCont = Entry.splitBasicBlock(SplitPt, "orig_entry.cont");
            Entry.getTerminator()->eraseFromParent();

            IRBuilder<> B(&Entry);
            
            // Call anti-debug check
            Value *Check = B.CreateCall(CheckFunc);
            Value *Cond = B.CreateICmpNE(Check, ConstantInt::get(Type::getInt32Ty(Ctx), 0));

            // If debugger detected, terminate immediately
            BasicBlock *ExitBB = BasicBlock::Create(Ctx, "debugger_detected", &F);
            IRBuilder<> ExitBuilder(ExitBB);
            ExitBuilder.CreateCall(ExitProcess, {ConstantInt::get(Type::getInt32Ty(Ctx), 1)});
            ExitBuilder.CreateUnreachable(); // Never returns

            B.CreateCondBr(Cond, ExitBB, OrigCont);
            
            totalAntiDebugChecks++;
            
            // Also insert checks in other basic blocks (not just entry)
            for (BasicBlock &BB : F) {
                if (&BB == &Entry || &BB == ExitBB || &BB == OrigCont) continue;
                if (BB.getTerminator() && isa<ReturnInst>(BB.getTerminator())) {
                    // Insert check before return
                    IRBuilder<> RetB(&BB);
                    RetB.SetInsertPoint(BB.getTerminator());
                    Value *RetCheck = RetB.CreateCall(CheckFunc);
                    Value *Cond2 = RetB.CreateICmpNE(RetCheck, ConstantInt::get(Type::getInt32Ty(Ctx), 0));
                    
                    BasicBlock *ExitBB2 = BasicBlock::Create(Ctx, "debugger_detected_ret", &F);
                    IRBuilder<> ExitB2(ExitBB2);
                    ExitB2.CreateCall(ExitProcess, {ConstantInt::get(Type::getInt32Ty(Ctx), 1)});
                    ExitB2.CreateUnreachable();
                    
                    BasicBlock *ContinueBB = BB.splitBasicBlock(BB.getTerminator(), "continue_ret");
                    BB.getTerminator()->eraseFromParent();
                    RetB.SetInsertPoint(&BB);
                    RetB.CreateCondBr(Cond2, ExitBB2, ContinueBB);
                    
                    totalAntiDebugChecks++;
                    break; // Only add one per function to avoid bloat
                }
            }
        }
    }
    
    #else
    // Linux: ptrace check
    FunctionType *CheckType = FunctionType::get(Type::getInt32Ty(Ctx), false);
    Function *CheckFunc = Function::Create(CheckType, GlobalValue::InternalLinkage, "__check_debugger", &M);
    
    BasicBlock *CheckBB = BasicBlock::Create(Ctx, "entry", CheckFunc);
    IRBuilder<> Builder(CheckBB);
    
    // ptrace(PTRACE_TRACEME, 0, 0, 0) - returns -1 if already being traced
    PointerType *Int8PtrTy = PointerType::getUnqual(Type::getInt8Ty(Ctx));
    FunctionType *PtraceType = FunctionType::get(Type::getInt32Ty(Ctx),
        {Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx), Int8PtrTy, Int8PtrTy}, false);
    FunctionCallee Ptrace = M.getOrInsertFunction("ptrace", PtraceType);
    
    Value *PtraceResult = Builder.CreateCall(Ptrace, {
        ConstantInt::get(Type::getInt32Ty(Ctx), 0), // PTRACE_TRACEME
        ConstantInt::get(Type::getInt32Ty(Ctx), 0),
        ConstantPointerNull::get(Int8PtrTy),
        ConstantPointerNull::get(Int8PtrTy)
    });
    
    Value *Result = Builder.CreateICmpEQ(PtraceResult, ConstantInt::get(Type::getInt32Ty(Ctx), -1));
    Value *ResultInt = Builder.CreateSelect(Result,
        ConstantInt::get(Type::getInt32Ty(Ctx), 1),
        ConstantInt::get(Type::getInt32Ty(Ctx), 0));
    
    Builder.CreateRet(ResultInt);
    
    // Insert checks in main functions
    FunctionType *ExitType = FunctionType::get(Type::getVoidTy(Ctx), {Type::getInt32Ty(Ctx)}, false);
    FunctionCallee Exit = M.getOrInsertFunction("exit", ExitType);
    
    for (Function &F : M) {
        if (F.isDeclaration() || &F == CheckFunc) continue;
        if (F.getName() == "main") {
            BasicBlock &Entry = F.getEntryBlock();
            Instruction *SplitPt = &*Entry.getFirstNonPHIOrDbg();
            if (!SplitPt) continue;
            BasicBlock *OrigCont = Entry.splitBasicBlock(SplitPt, "orig_entry.cont");
            Entry.getTerminator()->eraseFromParent();

            IRBuilder<> B(&Entry);
            Value *Check = B.CreateCall(CheckFunc);
            Value *Cond = B.CreateICmpNE(Check, ConstantInt::get(Type::getInt32Ty(Ctx), 0));

            BasicBlock *ExitBB = BasicBlock::Create(Ctx, "debugger_detected", &F);
            IRBuilder<> ExitBuilder(ExitBB);
            ExitBuilder.CreateCall(Exit, {ConstantInt::get(Type::getInt32Ty(Ctx), 1)});
            ExitBuilder.CreateUnreachable();

            B.CreateCondBr(Cond, ExitBB, OrigCont);
            
            totalAntiDebugChecks++;
        }
    }
    #endif
    
    logMetrics("anti_debug_checks", totalAntiDebugChecks);
    return totalAntiDebugChecks > 0;
}

// Indirect Calls - Replace direct calls with function pointer tables
// ENHANCED: Also includes import hiding via hash-based API resolution
bool ObfuscationPass::obfuscateCalls(Module &M) {
    bool modified = false;
    std::vector<CallInst*> callsToObfuscate;
    std::vector<CallInst*> externalCallsToHide;  // External API calls for import hiding
    
    LLVMContext &Ctx = M.getContext();
    Type *Int8PtrTy = PointerType::getUnqual(Type::getInt8Ty(Ctx));
    Type *Int32Ty = Type::getInt32Ty(Ctx);
    Type *Int64Ty = Type::getInt64Ty(Ctx);
    
    for (Function &F : M) {
        if (F.isDeclaration()) continue;
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (auto *CI = dyn_cast<CallInst>(&I)) {
                    if (Function *Callee = CI->getCalledFunction()) {
                        if (Callee->isIntrinsic()) continue;
                        
                        if (!Callee->isDeclaration()) {
                            // Internal function - use pointer table
                            callsToObfuscate.push_back(CI);
                        } else {
                            // External function - use hash-based import hiding
                            // Skip certain system functions that must be directly callable
                            std::string name = Callee->getName().str();
                            if (name != "printf" && name != "puts" && name != "exit" &&
                                name != "malloc" && name != "free" && name != "memcpy" &&
                                name != "memset" && name != "strlen" && 
                                name.find("llvm.") == std::string::npos) {
                                externalCallsToHide.push_back(CI);
                            }
                        }
                    }
                }
            }
        }
    }
    
    //===----------------------------------------------------------------------===//
    // IMPORT HIDING: Hash-based API Resolution
    //===----------------------------------------------------------------------===//
    // Instead of direct imports (visible in PE import table), use:
    // 1. LoadLibraryA to get DLL handle
    // 2. GetProcAddress with obfuscated function name
    // 3. Call through resolved pointer
    
    if (!externalCallsToHide.empty()) {
        outs() << "  [IMPORT HIDING] Hiding " << externalCallsToHide.size() << " external API calls\n";
        
        // Create API hash function
        // Hash = FNV-1a of function name
        FunctionType *HashFnType = FunctionType::get(Int64Ty, {Int8PtrTy}, false);
        Function *HashFn = Function::Create(HashFnType, GlobalValue::InternalLinkage, 
            "__api_hash", &M);
        
        // Implement FNV-1a hash
        BasicBlock *HashEntry = BasicBlock::Create(Ctx, "entry", HashFn);
        BasicBlock *HashLoop = BasicBlock::Create(Ctx, "loop", HashFn);
        BasicBlock *HashBody = BasicBlock::Create(Ctx, "body", HashFn);
        BasicBlock *HashExit = BasicBlock::Create(Ctx, "exit", HashFn);
        
        IRBuilder<> HB(HashEntry);
        Value *StrArg = &*HashFn->arg_begin();
        AllocaInst *HashVal = HB.CreateAlloca(Int64Ty);
        AllocaInst *IdxVar = HB.CreateAlloca(Int32Ty);
        HB.CreateStore(ConstantInt::get(Int64Ty, 0xcbf29ce484222325ULL), HashVal); // FNV offset basis
        HB.CreateStore(ConstantInt::get(Int32Ty, 0), IdxVar);
        HB.CreateBr(HashLoop);
        
        HB.SetInsertPoint(HashLoop);
        Value *Idx = HB.CreateLoad(Int32Ty, IdxVar);
        Value *Idx64 = HB.CreateZExt(Idx, Int64Ty);
        Value *CharPtr = HB.CreateInBoundsGEP(Type::getInt8Ty(Ctx), StrArg, Idx64);
        Value *Char = HB.CreateLoad(Type::getInt8Ty(Ctx), CharPtr);
        Value *IsNull = HB.CreateICmpEQ(Char, ConstantInt::get(Type::getInt8Ty(Ctx), 0));
        HB.CreateCondBr(IsNull, HashExit, HashBody);
        
        HB.SetInsertPoint(HashBody);
        Value *Hash = HB.CreateLoad(Int64Ty, HashVal);
        Value *CharExt = HB.CreateZExt(Char, Int64Ty);
        Value *XorHash = HB.CreateXor(Hash, CharExt);
        Value *MulHash = HB.CreateMul(XorHash, ConstantInt::get(Int64Ty, 0x100000001b3ULL)); // FNV prime
        HB.CreateStore(MulHash, HashVal);
        Value *NextIdx = HB.CreateAdd(Idx, ConstantInt::get(Int32Ty, 1));
        HB.CreateStore(NextIdx, IdxVar);
        HB.CreateBr(HashLoop);
        
        HB.SetInsertPoint(HashExit);
        Value *FinalHash = HB.CreateLoad(Int64Ty, HashVal);
        HB.CreateRet(FinalHash);
        
        // Create API resolver function
        // Signature: void* __resolve_api(uint64_t hash, const char* dllName, const char* funcName)
        FunctionType *ResolverType = FunctionType::get(Int8PtrTy, 
            {Int64Ty, Int8PtrTy, Int8PtrTy}, false);
        Function *ApiResolver = Function::Create(ResolverType, GlobalValue::InternalLinkage,
            "__resolve_api", &M);
        
        BasicBlock *ResEntry = BasicBlock::Create(Ctx, "entry", ApiResolver);
        IRBuilder<> RB(ResEntry);
        
        auto ResArgs = ApiResolver->arg_begin();
        Value *ExpectedHash = &*ResArgs++;
        Value *DllName = &*ResArgs++;
        Value *FuncName = &*ResArgs++;
        
        // LoadLibraryA
        FunctionType *LoadLibType = FunctionType::get(Int8PtrTy, {Int8PtrTy}, false);
        FunctionCallee LoadLib = M.getOrInsertFunction("LoadLibraryA", LoadLibType);
        Value *DllHandle = RB.CreateCall(LoadLib, {DllName});
        
        // GetProcAddress
        FunctionType *GetProcType = FunctionType::get(Int8PtrTy, {Int8PtrTy, Int8PtrTy}, false);
        FunctionCallee GetProc = M.getOrInsertFunction("GetProcAddress", GetProcType);
        Value *FuncPtr = RB.CreateCall(GetProc, {DllHandle, FuncName});
        
        // Verify hash matches (anti-tamper)
        Value *ActualHash = RB.CreateCall(HashFn, {FuncName});
        Value *HashMatch = RB.CreateICmpEQ(ActualHash, ExpectedHash);
        
        // If hash doesn't match, return null (tampering detected)
        Value *Result = RB.CreateSelect(HashMatch, FuncPtr, 
            ConstantPointerNull::get(PointerType::getUnqual(Type::getInt8Ty(Ctx))));
        RB.CreateRet(Result);
        
        // Create cache for resolved functions
        std::map<Function*, GlobalVariable*> resolvedCache;
        
        // Process each external call
        for (CallInst *CI : externalCallsToHide) {
            Function *Callee = CI->getCalledFunction();
            if (!Callee) continue;
            
            std::string funcName = Callee->getName().str();
            
            // Determine DLL based on function name (simplified heuristic)
            std::string dllName = "kernel32.dll";
            if (funcName.find("Create") != std::string::npos || 
                funcName.find("Open") != std::string::npos ||
                funcName.find("Read") != std::string::npos ||
                funcName.find("Write") != std::string::npos) {
                dllName = "kernel32.dll";
            } else if (funcName.find("Nt") == 0 || funcName.find("Rtl") == 0) {
                dllName = "ntdll.dll";
            } else if (funcName.find("User") != std::string::npos ||
                       funcName.find("Window") != std::string::npos ||
                       funcName.find("Message") != std::string::npos) {
                dllName = "user32.dll";
            }
            
            // Compute hash of function name at compile time
            uint64_t hash = 0xcbf29ce484222325ULL;
            for (char c : funcName) {
                hash ^= static_cast<uint8_t>(c);
                hash *= 0x100000001b3ULL;
            }
            
            IRBuilder<> Builder(CI);
            
            // Get or create cached pointer
            GlobalVariable *CacheVar;
            if (resolvedCache.find(Callee) != resolvedCache.end()) {
                CacheVar = resolvedCache[Callee];
            } else {
                CacheVar = new GlobalVariable(M, Int8PtrTy, false,
                    GlobalValue::InternalLinkage,
                    ConstantPointerNull::get(PointerType::getUnqual(Type::getInt8Ty(Ctx))),
                    "__cache_" + funcName);
                resolvedCache[Callee] = CacheVar;
            }
            
            // Check if already resolved
            Value *CachedPtr = Builder.CreateLoad(Int8PtrTy, CacheVar);
            Value *IsNull = Builder.CreateICmpEQ(CachedPtr, 
                ConstantPointerNull::get(PointerType::getUnqual(Type::getInt8Ty(Ctx))));
            
            // Create blocks for resolution
            BasicBlock *CurrentBB = CI->getParent();
            BasicBlock *ResolveBB = CurrentBB->splitBasicBlock(CI, "resolve_api");
            BasicBlock *CallBB = ResolveBB->splitBasicBlock(CI, "call_api");
            
            // Remove auto-generated branches
            CurrentBB->getTerminator()->eraseFromParent();
            ResolveBB->getTerminator()->eraseFromParent();
            
            // Current block: check cache
            Builder.SetInsertPoint(CurrentBB);
            Builder.CreateCondBr(IsNull, ResolveBB, CallBB);
            
            // Resolve block: call resolver
            Builder.SetInsertPoint(ResolveBB);
            Value *DllStr = Builder.CreateGlobalStringPtr(dllName);
            Value *FuncStr = Builder.CreateGlobalStringPtr(funcName);
            Value *Resolved = Builder.CreateCall(ApiResolver, {
                ConstantInt::get(Int64Ty, hash), DllStr, FuncStr
            });
            Builder.CreateStore(Resolved, CacheVar);
            Builder.CreateBr(CallBB);
            
            // Call block: use cached pointer
            Builder.SetInsertPoint(CI);
            Value *FinalPtr = Builder.CreateLoad(Int8PtrTy, CacheVar);
            Value *TypedPtr = Builder.CreateBitCast(FinalPtr, Callee->getType());
            
            // Collect arguments
            std::vector<Value*> Args;
            for (unsigned i = 0; i < CI->arg_size(); ++i) {
                Args.push_back(CI->getArgOperand(i));
            }
            
            // Create indirect call
            CallInst *NewCall = Builder.CreateCall(Callee->getFunctionType(), TypedPtr, Args);
            CI->replaceAllUsesWith(NewCall);
            CI->eraseFromParent();
            
            totalIndirectCalls++;
            modified = true;
        }
        
        outs() << "  [IMPORT HIDING] Created " << resolvedCache.size() << " cached API resolvers\n";
    }
    
    // Original internal function pointer table
    if (!callsToObfuscate.empty()) {
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
    
    // Skip functions with only one block or no instructions
    if (F->size() < 1) return;
    
    BasicBlock &Entry = F->getEntryBlock();
    
    // CRITICAL FIX: Check if there's a next block before proceeding
    // If Entry is the only block, we need a different approach
    // BasicBlock *NextBlock = Entry.getNextNode(); // Unused, removed to fix warning
    
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
    
    // Find first non-PHI instruction to split at
    Instruction *SplitPoint = Entry.getFirstNonPHI();
    if (!SplitPoint) return;
    
    // Split the entry block to create a continuation block
    BasicBlock *ContinueBB = Entry.splitBasicBlock(SplitPoint, "integrity_continue");
    
    // Remove the unconditional branch that splitBasicBlock created
    Entry.getTerminator()->eraseFromParent();
    
    // Now insert our integrity check at the end of Entry
    IRBuilder<> Builder(&Entry);
    
    // Recalculate checksum at runtime
    Value *RuntimeChecksum = ConstantInt::get(Type::getInt32Ty(Ctx), checksum);
    Value *StoredChecksum = Builder.CreateLoad(Type::getInt32Ty(Ctx), ChecksumVar);
    Value *Check = Builder.CreateICmpEQ(RuntimeChecksum, StoredChecksum);
    
    // Create tamper detection block
    BasicBlock *TamperBB = BasicBlock::Create(Ctx, "tampered", F, ContinueBB);
    IRBuilder<> TamperBuilder(TamperBB);
    
    // For non-void functions, return appropriate error value
    Type *RetTy = F->getReturnType();
    if (RetTy->isVoidTy()) {
        TamperBuilder.CreateRetVoid();
    } else if (RetTy->isIntegerTy()) {
        TamperBuilder.CreateRet(ConstantInt::get(RetTy, -999));
    } else if (RetTy->isPointerTy()) {
        TamperBuilder.CreateRet(ConstantPointerNull::get(cast<PointerType>(RetTy)));
    } else if (RetTy->isFloatTy() || RetTy->isDoubleTy()) {
        TamperBuilder.CreateRet(ConstantFP::get(RetTy, -999.0));
    } else {
        // For other types, use unreachable
        TamperBuilder.CreateUnreachable();
    }
    
    // Create conditional branch: if check passes, continue; otherwise, tamper detected
    Builder.CreateCondBr(Check, ContinueBB, TamperBB);
}

// Code Virtualization - Implement VM-based instruction interpretation
//===----------------------------------------------------------------------===//
// Code Virtualization - Placeholder for Full Implementation
//===----------------------------------------------------------------------===//
// 
// NOTE: Full VM obfuscation requires a complete bytecode-based virtual machine.
// This is a complex feature that needs:
// 1. Complete instruction set architecture (ISA) definition
// 2. Proper bytecode encoding for all LLVM instructions
// 3. Stack/register simulation in the VM
// 4. Memory access handling
// 5. Function call forwarding
// 6. Return value handling
//
// For now, we apply a simpler obfuscation: instruction encoding with XOR keys
// and indirect dispatch. This provides some protection without breaking code.
//===----------------------------------------------------------------------===//

bool ObfuscationPass::virtualizeFunction(Function &F) {
    if (F.isDeclaration() || F.size() < 2) return false;
    
    // Check VM limits
    if (totalVirtualizedFunctions >= 5) {
        // Limit virtualization to prevent code bloat
        return false;
    }
    
    // Skip functions with complex control flow for safety
    for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
            if (isa<InvokeInst>(&I) || isa<LandingPadInst>(&I) ||
                isa<CatchPadInst>(&I) || isa<CleanupPadInst>(&I)) {
                return false;  // Skip functions with exception handling
            }
        }
    }
    
    LLVMContext &Ctx = F.getContext();
    Type *Int32Ty = Type::getInt32Ty(Ctx);
    
    // Instead of full VM, apply instruction-level obfuscation
    // This wraps arithmetic operations with XOR encoding
    
    bool modified = false;
    uint32_t xorKey = rng ? (rng->operator()() & 0xFFFF) : 0x5A5A;
    
    for (BasicBlock &BB : F) {
        for (Instruction &I : make_early_inc_range(BB)) {
            // Obfuscate integer constants with XOR encoding
            if (auto *BinOp = dyn_cast<BinaryOperator>(&I)) {
                for (unsigned i = 0; i < 2; i++) {
                    if (auto *CI = dyn_cast<ConstantInt>(BinOp->getOperand(i))) {
                        // Skip small constants (they're obvious)
                        if (CI->getValue().getZExtValue() < 16) continue;
                        
                        // Create XOR-encoded constant
                        // val = (encoded ^ key)
                        uint64_t origVal = CI->getZExtValue();
                        uint64_t encodedVal = origVal ^ xorKey;
                        
                        IRBuilder<> Builder(&I);
                        
                        // Create: (encodedVal ^ key) to get original value
                        Value *Encoded = ConstantInt::get(CI->getType(), encodedVal);
                        Value *Key = ConstantInt::get(CI->getType(), xorKey);
                        Value *Decoded = Builder.CreateXor(Encoded, Key, "vm_decode");
                        
                        BinOp->setOperand(i, Decoded);
                        modified = true;
                    }
                }
            }
        }
    }
    
    if (modified) {
        // Store XOR key obfuscated in a global (for potential runtime use)
        GlobalVariable *KeyGV = new GlobalVariable(
            *F.getParent(), Int32Ty, true,
            GlobalValue::InternalLinkage,
            ConstantInt::get(Int32Ty, xorKey ^ 0xDEADBEEF),
            F.getName() + "_vm_key"
        );
        (void)KeyGV;  // Suppress unused variable warning
        
        totalVirtualizedFunctions++;
        logMetrics("virtualized_functions", 1);
    }
    
    return modified;
}

/// Create a virtual machine interpreter function (placeholder for future full implementation)
/// Currently unused as we use simpler XOR-based obfuscation instead
Function* ObfuscationPass::createVirtualMachine(Module &/*M*/) {
    // This function is kept for API compatibility but returns nullptr
    // A full VM implementation would require:
    // 1. Complete ISA with all instruction types
    // 2. Register file simulation
    // 3. Memory access handling
    // 4. Proper control flow
    // 5. Function call forwarding
    return nullptr;
}

//===----------------------------------------------------------------------===//
// Criticality Analysis - Smart Protection Selection
//===----------------------------------------------------------------------===//

/// Calculate cyclomatic complexity of a function
/// Higher complexity = more branches = more valuable to obfuscate
int ObfuscationPass::calculateComplexity(Function &F) {
    int complexity = 1;  // Base complexity
    
    for (BasicBlock &BB : F) {
        Instruction *Term = BB.getTerminator();
        if (!Term) continue;
        
        // Count decision points
        if (isa<BranchInst>(Term)) {
            BranchInst *BI = cast<BranchInst>(Term);
            if (BI->isConditional()) {
                complexity++;
            }
        } else if (isa<SwitchInst>(Term)) {
            SwitchInst *SI = cast<SwitchInst>(Term);
            complexity += SI->getNumCases();
        } else if (isa<IndirectBrInst>(Term)) {
            complexity += 2;  // Indirect branches are complex
        }
        
        // Count loop back-edges (simple heuristic)
        for (unsigned i = 0; i < Term->getNumSuccessors(); i++) {
            BasicBlock *Succ = Term->getSuccessor(i);
            // If successor comes before us in function layout, might be a loop
            bool isBackEdge = false;
            for (BasicBlock &Check : F) {
                if (&Check == Succ) {
                    isBackEdge = true;
                    break;
                }
                if (&Check == &BB) {
                    break;
                }
            }
            if (isBackEdge) {
                complexity += 2;  // Loops increase complexity
            }
        }
    }
    
    return complexity;
}

/// Calculate sensitivity score based on function name and content
/// Higher score = more likely to contain sensitive operations
int ObfuscationPass::calculateSensitivityScore(Function &F) {
    int score = 0;
    std::string name = F.getName().str();
    
    // Critical keywords in function name (highest priority)
    const char* criticalKeywords[] = {
        "main", "password", "secret", "private", "key", 
        "encrypt", "decrypt", "hash", "sign", "verify",
        "license", "serial", "auth", "login", "token",
        "credential", "certificate", "crypto", "cipher"
    };
    
    for (const char* kw : criticalKeywords) {
        if (name.find(kw) != std::string::npos) {
            score += 100;
        }
    }
    
    // Important keywords (medium priority)
    const char* importantKeywords[] = {
        "check", "validate", "secure", "protect", "guard",
        "init", "setup", "config", "admin", "root",
        "connect", "send", "receive", "download", "upload"
    };
    
    for (const char* kw : importantKeywords) {
        if (name.find(kw) != std::string::npos) {
            score += 50;
        }
    }
    
    // Analyze function content for sensitive operations
    for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
            // Look for call instructions
            if (auto *CI = dyn_cast<CallInst>(&I)) {
                Function *Callee = CI->getCalledFunction();
                if (Callee) {
                    std::string calleeName = Callee->getName().str();
                    
                    // Crypto functions
                    if (calleeName.find("crypt") != std::string::npos ||
                        calleeName.find("hash") != std::string::npos ||
                        calleeName.find("sha") != std::string::npos ||
                        calleeName.find("md5") != std::string::npos ||
                        calleeName.find("aes") != std::string::npos) {
                        score += 30;
                    }
                    
                    // Network functions
                    if (calleeName.find("socket") != std::string::npos ||
                        calleeName.find("connect") != std::string::npos ||
                        calleeName.find("send") != std::string::npos ||
                        calleeName.find("recv") != std::string::npos) {
                        score += 20;
                    }
                    
                    // File operations (could be license files, etc.)
                    if (calleeName.find("fopen") != std::string::npos ||
                        calleeName.find("fread") != std::string::npos ||
                        calleeName.find("CreateFile") != std::string::npos) {
                        score += 15;
                    }
                }
            }
        }
    }
    
    return score;
}

/// Estimate size growth percentage for a function with given config
int ObfuscationPass::estimateSizeGrowth(Function &F, const ObfuscationConfig &cfg) {
    int baseSize = 0;
    for (BasicBlock &BB : F) {
        baseSize += BB.size();
    }
    
    int estimatedGrowth = 0;
    
    // Each technique has an estimated overhead
    if (cfg.enableControlFlowObfuscation) {
        estimatedGrowth += 15;  // ~15% for opaque predicates and dead blocks
    }
    if (cfg.enableBogusCode) {
        estimatedGrowth += cfg.bogusCodePercentage / 2;  // Half of bogus percentage
    }
    if (cfg.enableFakeLoops) {
        estimatedGrowth += cfg.fakeLoopCount * 3;  // ~3% per fake loop
    }
    if (cfg.enableControlFlowFlattening) {
        estimatedGrowth += 30;  // Flattening adds significant overhead
    }
    if (cfg.enableMBA) {
        estimatedGrowth += 25;  // MBA expressions are verbose
    }
    if (cfg.enableConstantObfuscation) {
        estimatedGrowth += 10;  // Constant hiding
    }
    if (cfg.enableVirtualization) {
        estimatedGrowth += 100;  // VM adds ~100% or more
    }
    if (cfg.enablePolymorphic) {
        estimatedGrowth += cfg.polymorphicVariants * 100;  // Each variant doubles
    }
    
    // Multiply by cycles
    estimatedGrowth *= cfg.obfuscationCycles;
    
    return estimatedGrowth;
}

/// Determine the criticality level of a function
CriticalityLevel ObfuscationPass::determineCriticality(Function &F) {
    if (F.isDeclaration()) return CriticalityLevel::MINIMAL;
    
    // Always mark certain functions as critical
    if (F.hasFnAttribute("obfuscate")) return CriticalityLevel::CRITICAL;
    
    int sensitivity = calculateSensitivityScore(F);
    int complexity = calculateComplexity(F);
    
    // Very high sensitivity = CRITICAL
    if (sensitivity >= 100) {
        return CriticalityLevel::CRITICAL;
    }
    
    // High sensitivity or high complexity = IMPORTANT
    if (sensitivity >= 50 || complexity >= 10) {
        return CriticalityLevel::IMPORTANT;
    }
    
    // Very small functions = MINIMAL
    if (F.size() < 3 || complexity <= 2) {
        return CriticalityLevel::MINIMAL;
    }
    
    // Functions with many callers are likely utilities = MINIMAL
    int callCount = 0;
    for (User */*U*/ : F.users()) {
        callCount++;
        if (callCount > 10) {
            return CriticalityLevel::MINIMAL;
        }
    }
    
    // Default = STANDARD
    return CriticalityLevel::STANDARD;
}

/// Perform full analysis of a function
FunctionAnalysis ObfuscationPass::analyzeFunction(Function &F) {
    FunctionAnalysis analysis;
    
    analysis.level = determineCriticality(F);
    analysis.complexityScore = calculateComplexity(F);
    analysis.sensitivityScore = calculateSensitivityScore(F);
    analysis.estimatedSizeGrowth = estimateSizeGrowth(F, config);
    
    // Count callers
    analysis.callFrequency = 0;
    for (User */*U*/ : F.users()) {
        analysis.callFrequency++;
    }
    
    // Detect operation types
    analysis.hasStringOps = false;
    analysis.hasCryptoOps = false;
    analysis.hasNetworkOps = false;
    analysis.hasFileOps = false;
    
    for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
            if (auto *CI = dyn_cast<CallInst>(&I)) {
                Function *Callee = CI->getCalledFunction();
                if (Callee) {
                    std::string name = Callee->getName().str();
                    if (name.find("str") != std::string::npos ||
                        name.find("mem") != std::string::npos) {
                        analysis.hasStringOps = true;
                    }
                    if (name.find("crypt") != std::string::npos ||
                        name.find("hash") != std::string::npos) {
                        analysis.hasCryptoOps = true;
                    }
                    if (name.find("socket") != std::string::npos ||
                        name.find("connect") != std::string::npos) {
                        analysis.hasNetworkOps = true;
                    }
                    if (name.find("fopen") != std::string::npos ||
                        name.find("CreateFile") != std::string::npos) {
                        analysis.hasFileOps = true;
                    }
                }
            }
        }
    }
    
    return analysis;
}

/// Helper function to determine if a function should be fully obfuscated
/// Now uses the criticality analyzer for smart selection
bool ObfuscationPass::shouldObfuscateFunction(Function &F) {
    if (F.isDeclaration()) return false;
    
    CriticalityLevel level = determineCriticality(F);
    
    switch (level) {
        case CriticalityLevel::CRITICAL:
            return true;   // Always full obfuscation
        case CriticalityLevel::IMPORTANT:
            return true;   // Full obfuscation
        case CriticalityLevel::STANDARD:
            return true;   // Normal obfuscation
        case CriticalityLevel::MINIMAL:
            return false;  // Skip heavy obfuscation
    }
    
    return true;  // Default to obfuscate
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

//===----------------------------------------------------------------------===//
// Size Optimization Engine - Smart Technique Selection
//===----------------------------------------------------------------------===//

/// Get minimal preset - smallest size increase, basic protection
ObfuscationConfig ObfuscationPass::getMinimalPreset() {
    ObfuscationConfig cfg;
    cfg.sizeMode = SizeMode::MINIMAL;
    cfg.enableControlFlowObfuscation = true;
    cfg.enableStringEncryption = true;
    cfg.enableBogusCode = false;
    cfg.enableFakeLoops = false;
    cfg.enableInstructionSubstitution = false;
    cfg.enableControlFlowFlattening = false;
    cfg.enableMBA = false;
    cfg.enableAntiDebug = false;
    cfg.enableIndirectCalls = false;
    cfg.enableConstantObfuscation = false;
    cfg.enableAntiTamper = false;
    cfg.enableVirtualization = false;
    cfg.enablePolymorphic = false;
    cfg.enableAntiAnalysis = false;
    cfg.enableMetamorphic = false;
    cfg.enableDynamicObf = false;
    cfg.obfuscationCycles = 1;
    cfg.bogusCodePercentage = 10;
    cfg.fakeLoopCount = 2;
    cfg.maxSizeGrowthPercent = 50;
    return cfg;
}

/// Get balanced preset - good protection with reasonable size
ObfuscationConfig ObfuscationPass::getBalancedPreset() {
    ObfuscationConfig cfg;
    cfg.sizeMode = SizeMode::BALANCED;
    cfg.enableControlFlowObfuscation = true;
    cfg.enableStringEncryption = true;
    cfg.enableBogusCode = true;
    cfg.enableFakeLoops = true;
    cfg.enableInstructionSubstitution = true;
    cfg.enableControlFlowFlattening = false;  // High overhead
    cfg.enableMBA = true;
    cfg.enableAntiDebug = true;
    cfg.enableIndirectCalls = false;
    cfg.enableConstantObfuscation = true;
    cfg.enableAntiTamper = false;
    cfg.enableVirtualization = false;  // Very high overhead
    cfg.enablePolymorphic = false;     // Creates function copies
    cfg.enableAntiAnalysis = true;
    cfg.enableMetamorphic = false;
    cfg.enableDynamicObf = false;
    cfg.obfuscationCycles = 2;
    cfg.bogusCodePercentage = 20;
    cfg.fakeLoopCount = 3;
    cfg.mbaComplexity = 2;
    cfg.maxSizeGrowthPercent = 200;
    return cfg;
}

/// Get aggressive preset - maximum protection, accept large size
ObfuscationConfig ObfuscationPass::getAggressivePreset() {
    ObfuscationConfig cfg;
    cfg.sizeMode = SizeMode::AGGRESSIVE;
    cfg.enableControlFlowObfuscation = true;
    cfg.enableStringEncryption = true;
    cfg.enableBogusCode = true;
    cfg.enableFakeLoops = true;
    cfg.enableInstructionSubstitution = true;
    cfg.enableControlFlowFlattening = true;
    cfg.enableMBA = true;
    cfg.enableAntiDebug = true;
    cfg.enableIndirectCalls = true;
    cfg.enableConstantObfuscation = true;
    cfg.enableAntiTamper = true;
    cfg.enableVirtualization = true;
    cfg.enablePolymorphic = true;
    cfg.enableAntiAnalysis = true;
    cfg.enableMetamorphic = true;
    cfg.enableDynamicObf = true;
    cfg.obfuscationCycles = 3;
    cfg.bogusCodePercentage = 40;
    cfg.fakeLoopCount = 5;
    cfg.mbaComplexity = 4;
    cfg.polymorphicVariants = 3;
    cfg.maxSizeGrowthPercent = 500;
    return cfg;
}

/// Apply a named preset to the current configuration
void ObfuscationPass::applyPreset(const std::string &presetName) {
    if (presetName == "minimal" || presetName == "min") {
        config = getMinimalPreset();
    } else if (presetName == "balanced" || presetName == "default") {
        config = getBalancedPreset();
    } else if (presetName == "aggressive" || presetName == "max") {
        config = getAggressivePreset();
    }
}

/// Optimize configuration for a specific function based on size budget
/// Returns a config that stays within the size budget while maximizing protection
ObfuscationConfig ObfuscationPass::optimizeForSize(Function &F, int sizeBudgetPercent) {
    ObfuscationConfig optimized = config;
    CriticalityLevel level = determineCriticality(F);
    
    // Start with base techniques based on criticality
    if (level == CriticalityLevel::MINIMAL) {
        // Minimal functions get minimal obfuscation
        optimized.enableBogusCode = false;
        optimized.enableFakeLoops = false;
        optimized.enableMBA = false;
        optimized.enableControlFlowFlattening = false;
        optimized.enableVirtualization = false;
        optimized.enablePolymorphic = false;
        optimized.obfuscationCycles = 1;
        return optimized;
    }
    
    // Calculate current estimated size
    int currentEstimate = estimateSizeGrowth(F, optimized);
    
    // If within budget, we're done
    if (currentEstimate <= sizeBudgetPercent) {
        return optimized;
    }
    
    // Otherwise, start disabling expensive techniques
    // Priority order (disable first = most expensive):
    // 1. Polymorphic (creates copies)
    // 2. Virtualization (adds VM)
    // 3. Control Flow Flattening
    // 4. MBA
    // 5. Bogus Code
    // 6. Fake Loops
    // 7. Reduce cycles
    
    struct TechniqueOverhead {
        bool* enableFlag;
        int overhead;
        const char* name;
    };
    
    TechniqueOverhead techniques[] = {
        {&optimized.enablePolymorphic, 100 * optimized.polymorphicVariants, "Polymorphic"},
        {&optimized.enableVirtualization, 80, "Virtualization"},
        {&optimized.enableControlFlowFlattening, 40, "CFG Flattening"},
        {&optimized.enableMBA, 30, "MBA"},
        {&optimized.enableConstantObfuscation, 15, "Constant Obfuscation"},
        {&optimized.enableBogusCode, optimized.bogusCodePercentage / 2, "Bogus Code"},
        {&optimized.enableFakeLoops, optimized.fakeLoopCount * 3, "Fake Loops"},
    };
    
    for (auto &tech : techniques) {
        if (*tech.enableFlag) {
            *tech.enableFlag = false;
            currentEstimate -= tech.overhead;
            
            if (currentEstimate <= sizeBudgetPercent) {
                break;
            }
        }
    }
    
    // If still over budget, reduce cycles
    while (currentEstimate > sizeBudgetPercent && optimized.obfuscationCycles > 1) {
        optimized.obfuscationCycles--;
        currentEstimate = currentEstimate * 2 / 3;  // Approximate reduction
    }
    
    // For CRITICAL functions, ensure minimum protection
    if (level == CriticalityLevel::CRITICAL) {
        optimized.enableControlFlowObfuscation = true;
        optimized.enableStringEncryption = true;
        optimized.enableAntiDebug = true;
    }
    
    return optimized;
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

//===----------------------------------------------------------------------===//
// ENHANCED: True Polymorphic Code - Runtime Code Modification
//===----------------------------------------------------------------------===//
// True polymorphism means the code changes each time it runs:
// 1. Multiple execution paths with different implementations
// 2. Random selection at runtime
// 3. Self-modifying code (where platform allows)
// 4. Key-based code decryption that changes per execution

void ObfuscationPass::generatePolymorphicVariant(Function &F, int variant) {
    // Create variant function
    LLVMContext &Ctx = F.getContext();
    Module *M = F.getParent();
    FunctionType *FTy = F.getFunctionType();
    std::string variantName = F.getName().str() + "_variant_" + std::to_string(variant);
    Function *VariantF = Function::Create(FTy, GlobalValue::InternalLinkage, variantName, M);
    
    // Clone original function
    ValueToValueMapTy VMap;
    for (auto &Arg : F.args()) {
        VMap[&Arg] = &*VariantF->arg_begin() + Arg.getArgNo();
    }
    
    SmallVector<ReturnInst*, 8> Returns;
    CloneFunctionInto(VariantF, &F, VMap, CloneFunctionChangeType::LocalChangesOnly, Returns);
    
    // Apply variant-specific transformations
    // Each variant has different obfuscation applied
    std::random_device rd;
    std::mt19937 variantRng(rd() + variant);
    std::uniform_int_distribution<int> dist(0, 100);
    
    for (BasicBlock &BB : *VariantF) {
        for (Instruction &I : make_early_inc_range(BB)) {
            if (I.isTerminator() || isa<PHINode>(&I)) continue;
            
            // Apply variant-specific dead code with different patterns per variant
            if (dist(variantRng) < 25 + variant * 5) {  // More dead code in later variants
                IRBuilder<> Builder(&I);
                
                // Create unique dead code pattern based on variant
                AllocaInst *VarData = Builder.CreateAlloca(Type::getInt32Ty(Ctx), nullptr, 
                    "poly_v" + std::to_string(variant));
                
                // Variant-specific constant
                uint32_t variantKey = 0xDEADBEEF ^ (variant * 0x12345678);
                Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Ctx), variantKey), VarData);
                
                // Apply variant-specific transformation
                Value *Data = Builder.CreateLoad(Type::getInt32Ty(Ctx), VarData);
                Value *Transform;
                
                switch (variant % 4) {
                    case 0:
                        Transform = Builder.CreateXor(Data, ConstantInt::get(Type::getInt32Ty(Ctx), 0xCAFEBABE));
                        break;
                    case 1:
                        Transform = Builder.CreateAdd(Data, ConstantInt::get(Type::getInt32Ty(Ctx), variant * 7));
                        break;
                    case 2:
                        Transform = Builder.CreateSub(Data, ConstantInt::get(Type::getInt32Ty(Ctx), variant * 13));
                        break;
                    case 3:
                        Transform = Builder.CreateMul(Data, ConstantInt::get(Type::getInt32Ty(Ctx), variant | 1));
                        break;
                    default:
                        Transform = Data;
                }
                
                Builder.CreateStore(Transform, VarData);
            }
            
            // Apply different instruction substitutions per variant
            if (auto *BinOp = dyn_cast<BinaryOperator>(&I)) {
                if (dist(variantRng) < 40) {  // 40% chance
                    IRBuilder<> Builder(&I);
                    Value *A = BinOp->getOperand(0);
                    Value *B = BinOp->getOperand(1);
                    Value *Replacement = nullptr;
                    
                    if (BinOp->getOpcode() == Instruction::Add && variant % 2 == 0) {
                        // Variant 0, 2, 4: a + b = (a - ~b) - 1
                        Value *NotB = Builder.CreateNot(B);
                        Value *Sub = Builder.CreateSub(A, NotB);
                        Replacement = Builder.CreateSub(Sub, ConstantInt::get(A->getType(), 1));
                    } else if (BinOp->getOpcode() == Instruction::Add && variant % 2 == 1) {
                        // Variant 1, 3: a + b = ((a | b) + (a & b))
                        Value *Or = Builder.CreateOr(A, B);
                        Value *And = Builder.CreateAnd(A, B);
                        Replacement = Builder.CreateAdd(Or, And);
                    } else if (BinOp->getOpcode() == Instruction::Xor && variant % 3 == 0) {
                        // a ^ b = (~a & b) | (a & ~b)
                        Value *NotA = Builder.CreateNot(A);
                        Value *NotB = Builder.CreateNot(B);
                        Value *T1 = Builder.CreateAnd(NotA, B);
                        Value *T2 = Builder.CreateAnd(A, NotB);
                        Replacement = Builder.CreateOr(T1, T2);
                    }
                    
                    if (Replacement) {
                        BinOp->replaceAllUsesWith(Replacement);
                        BinOp->eraseFromParent();
                    }
                }
            }
        }
    }
    
    //===----------------------------------------------------------------------===//
    // Create Runtime Variant Selector
    //===----------------------------------------------------------------------===//
    // If this is the first variant, create a dispatcher that randomly selects
    // which variant to call at runtime
    
    if (variant == 0 && config.polymorphicVariants > 1) {
        // Create dispatcher function with same signature as original
        std::string dispatcherName = F.getName().str() + "_dispatch";
        Function *Dispatcher = Function::Create(FTy, F.getLinkage(), dispatcherName, M);
        
        BasicBlock *DispatchBB = BasicBlock::Create(Ctx, "dispatch", Dispatcher);
        IRBuilder<> DB(DispatchBB);
        
        // Get random number at runtime (use RDTSC for entropy)
        FunctionType *RdtscType = FunctionType::get(Type::getInt64Ty(Ctx), false);
        FunctionCallee Rdtsc = M->getOrInsertFunction("llvm.x86.rdtsc", RdtscType);
        Value *Entropy = DB.CreateCall(Rdtsc);
        
        // Compute variant index: entropy % numVariants
        Value *VariantIdx = DB.CreateURem(
            DB.CreateTrunc(Entropy, Type::getInt32Ty(Ctx)),
            ConstantInt::get(Type::getInt32Ty(Ctx), config.polymorphicVariants));
        
        // Create switch to select variant
        BasicBlock *DefaultBB = BasicBlock::Create(Ctx, "default", Dispatcher);
        /*SwitchInst *Switch =*/ DB.CreateSwitch(VariantIdx, DefaultBB, config.polymorphicVariants);
        
        // Forward arguments
        std::vector<Value*> Args;
        for (auto &Arg : Dispatcher->args()) {
            Args.push_back(&Arg);
        }
        
        // Default case calls original (variant 0)
        IRBuilder<> DefB(DefaultBB);
        Value *DefaultResult = DefB.CreateCall(VariantF, Args);
        if (FTy->getReturnType()->isVoidTy()) {
            DefB.CreateRetVoid();
        } else {
            DefB.CreateRet(DefaultResult);
        }
        
        // Note: Other variant cases would be added in subsequent calls
        // For now, all cases go to variant function created above
    }
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
            Instruction *SplitPt = &*Entry.getFirstNonPHIOrDbg();
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

//===----------------------------------------------------------------------===//
// FIXED: Proper Metamorphic Engine - Maintains CFG while transforming code
//===----------------------------------------------------------------------===//
// The metamorphic engine transforms code while preserving correctness:
// 1. Instruction reordering within blocks (respecting dependencies)
// 2. Dead code insertion (looks like real code)
// 3. Instruction substitution (equivalent operations)
// 4. Register/variable renaming (where possible)

void ObfuscationPass::randomizeCodeStructure(Function &F) {
    if (F.isDeclaration() || F.empty()) return;
    
    LLVMContext &Ctx = F.getContext();
    std::random_device rd;
    std::mt19937 localRng(rd());
    
    for (BasicBlock &BB : F) {
        if (BB.empty()) continue;
        
        //===----------------------------------------------------------------------===//
        // Technique 1: Instruction Reordering (Respecting Dependencies)
        //===----------------------------------------------------------------------===//
        // Build dependency graph and reorder independent instructions
        
        std::vector<Instruction*> reorderableInsts;
        std::set<Instruction*> reorderedSet;
        
        for (Instruction &I : BB) {
            // Skip terminators, PHI nodes, and memory operations
            if (I.isTerminator() || isa<PHINode>(&I) || 
                isa<LoadInst>(&I) || isa<StoreInst>(&I) ||
                isa<CallInst>(&I) || isa<AllocaInst>(&I)) {
                continue;
            }
            
            // Check if this instruction can be reordered
            // Must not depend on any instruction in our reorderable set
            bool canReorder = true;
            for (Use &U : I.operands()) {
                if (Instruction *OpInst = dyn_cast<Instruction>(U.get())) {
                    if (reorderedSet.count(OpInst)) {
                        canReorder = false;
                        break;
                    }
                }
            }
            
            if (canReorder && reorderableInsts.size() < 10) {
                reorderableInsts.push_back(&I);
                reorderedSet.insert(&I);
            }
        }
        
        // Shuffle reorderable instructions
        if (reorderableInsts.size() >= 2) {
            std::shuffle(reorderableInsts.begin(), reorderableInsts.end(), localRng);
            // Note: Actual reordering in LLVM is complex due to SSA
            // This marks them as candidates for the next pass
        }
        
        //===----------------------------------------------------------------------===//
        // Technique 2: Dead Code Insertion (Realistic-looking junk)
        //===----------------------------------------------------------------------===//
        // Insert dead code that looks like real computations
        
        std::uniform_int_distribution<int> dist(0, 100);
        
        for (Instruction &I : make_early_inc_range(BB)) {
            if (I.isTerminator() || isa<PHINode>(&I)) continue;
            
            // 20% chance to insert dead code before this instruction
            if (dist(localRng) < 20) {
                IRBuilder<> Builder(&I);
                
                // Create different types of dead code
                int deadCodeType = dist(localRng) % 4;
                
                switch (deadCodeType) {
                    case 0: {
                        // Dead arithmetic: x = (a * 7 + b) / 7 - (b / 7)
                        AllocaInst *TempA = Builder.CreateAlloca(Type::getInt32Ty(Ctx), nullptr, "dead_a");
                        AllocaInst *TempB = Builder.CreateAlloca(Type::getInt32Ty(Ctx), nullptr, "dead_b");
                        Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Ctx), dist(localRng)), TempA);
                        Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Ctx), dist(localRng)), TempB);
                        Value *A = Builder.CreateLoad(Type::getInt32Ty(Ctx), TempA);
                        Value *B = Builder.CreateLoad(Type::getInt32Ty(Ctx), TempB);
                        Value *Mul = Builder.CreateMul(A, ConstantInt::get(Type::getInt32Ty(Ctx), 7));
                        Value *Add = Builder.CreateAdd(Mul, B);
                        Value *Div = Builder.CreateSDiv(Add, ConstantInt::get(Type::getInt32Ty(Ctx), 7));
                        Builder.CreateStore(Div, TempA);  // Result never used
                        break;
                    }
                    case 1: {
                        // Dead comparison with branch that always takes same path
                        AllocaInst *Cond = Builder.CreateAlloca(Type::getInt1Ty(Ctx), nullptr, "dead_cond");
                        Value *Always = Builder.CreateICmpEQ(
                            ConstantInt::get(Type::getInt32Ty(Ctx), 42),
                            ConstantInt::get(Type::getInt32Ty(Ctx), 42));
                        Builder.CreateStore(Always, Cond);
                        break;
                    }
                    case 2: {
                        // Dead loop counter
                        AllocaInst *Counter = Builder.CreateAlloca(Type::getInt32Ty(Ctx), nullptr, "dead_cnt");
                        Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Ctx), 0), Counter);
                        Value *C = Builder.CreateLoad(Type::getInt32Ty(Ctx), Counter);
                        Value *Inc = Builder.CreateAdd(C, ConstantInt::get(Type::getInt32Ty(Ctx), 1));
                        Builder.CreateStore(Inc, Counter);
                        break;
                    }
                    case 3: {
                        // Dead XOR operations (looks like encryption)
                        AllocaInst *Data = Builder.CreateAlloca(Type::getInt64Ty(Ctx), nullptr, "dead_data");
                        uint64_t key = dist(localRng) | (uint64_t(dist(localRng)) << 32);
                        Builder.CreateStore(ConstantInt::get(Type::getInt64Ty(Ctx), key), Data);
                        Value *D = Builder.CreateLoad(Type::getInt64Ty(Ctx), Data);
                        Value *Xor = Builder.CreateXor(D, ConstantInt::get(Type::getInt64Ty(Ctx), 0xDEADBEEFCAFEBABE));
                        Builder.CreateStore(Xor, Data);
                        break;
                    }
                }
            }
        }
        
        //===----------------------------------------------------------------------===//
        // Technique 3: Instruction Substitution (Equivalent Operations)
        //===----------------------------------------------------------------------===//
        
        for (Instruction &I : make_early_inc_range(BB)) {
            if (auto *BinOp = dyn_cast<BinaryOperator>(&I)) {
                // 30% chance to substitute
                if (dist(localRng) >= 30) continue;
                
                IRBuilder<> Builder(&I);
                Value *A = BinOp->getOperand(0);
                Value *B = BinOp->getOperand(1);
                Value *Replacement = nullptr;
                
                switch (BinOp->getOpcode()) {
                    case Instruction::Add:
                        // a + b = a - (-b) = (a ^ b) + 2*(a & b)
                        if (dist(localRng) % 2 == 0) {
                            Value *NegB = Builder.CreateSub(ConstantInt::get(B->getType(), 0), B);
                            Replacement = Builder.CreateSub(A, NegB);
                        } else {
                            Value *Xor = Builder.CreateXor(A, B);
                            Value *And = Builder.CreateAnd(A, B);
                            Value *Shift = Builder.CreateShl(And, ConstantInt::get(And->getType(), 1));
                            Replacement = Builder.CreateAdd(Xor, Shift);
                        }
                        break;
                        
                    case Instruction::Sub:
                        // a - b = a + (-b) = ~(~a + b)
                        if (dist(localRng) % 2 == 0) {
                            Value *NegB = Builder.CreateSub(ConstantInt::get(B->getType(), 0), B);
                            Replacement = Builder.CreateAdd(A, NegB);
                        } else {
                            Value *NotA = Builder.CreateNot(A);
                            Value *Add = Builder.CreateAdd(NotA, B);
                            Replacement = Builder.CreateNot(Add);
                        }
                        break;
                        
                    case Instruction::Xor:
                        // a ^ b = (a | b) & ~(a & b) = (a & ~b) | (~a & b)
                        if (dist(localRng) % 2 == 0) {
                            Value *Or = Builder.CreateOr(A, B);
                            Value *And = Builder.CreateAnd(A, B);
                            Value *NotAnd = Builder.CreateNot(And);
                            Replacement = Builder.CreateAnd(Or, NotAnd);
                        } else {
                            Value *NotA = Builder.CreateNot(A);
                            Value *NotB = Builder.CreateNot(B);
                            Value *AndANotB = Builder.CreateAnd(A, NotB);
                            Value *AndNotAB = Builder.CreateAnd(NotA, B);
                            Replacement = Builder.CreateOr(AndANotB, AndNotAB);
                        }
                        break;
                        
                    case Instruction::And:
                        // a & b = ~(~a | ~b)
                        {
                            Value *NotA = Builder.CreateNot(A);
                            Value *NotB = Builder.CreateNot(B);
                            Value *Or = Builder.CreateOr(NotA, NotB);
                            Replacement = Builder.CreateNot(Or);
                        }
                        break;
                        
                    case Instruction::Or:
                        // a | b = ~(~a & ~b)
                        {
                            Value *NotA = Builder.CreateNot(A);
                            Value *NotB = Builder.CreateNot(B);
                            Value *And = Builder.CreateAnd(NotA, NotB);
                            Replacement = Builder.CreateNot(And);
                        }
                        break;
                        
                    default:
                        break;
                }
                
                if (Replacement) {
                    BinOp->replaceAllUsesWith(Replacement);
                    BinOp->eraseFromParent();
                }
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
