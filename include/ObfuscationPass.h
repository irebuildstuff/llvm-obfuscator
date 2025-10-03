#ifndef OBFUSCATION_PASS_H
#define OBFUSCATION_PASS_H

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/RandomNumberGenerator.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/PassManager.h"
#include <map>
#include <vector>
#include <string>
#include <set>
#include <random>

namespace llvm {

struct ObfuscationConfig {
    bool enableControlFlowObfuscation = true;
    bool enableStringEncryption = true;
    bool enableBogusCode = true;
    bool enableFakeLoops = true;
    bool enableInstructionSubstitution = false;
    bool enableControlFlowFlattening = false;
    bool enableMBA = false;
    bool enableAntiDebug = false;
    bool enableIndirectCalls = false;
    bool enableConstantObfuscation = false;
    bool enableAntiTamper = false;
    bool enableVirtualization = false;
    bool enablePolymorphic = false;
    bool enableAntiAnalysis = false;
    bool enableMetamorphic = false;
    bool enableDynamicObf = false;
    bool decryptStringsAtStartup = true;
    int obfuscationCycles = 3;
    int bogusCodePercentage = 30;
    int fakeLoopCount = 5;
    int stringEncryptionCount = 0;
    int mbaComplexity = 3;
    int flatteningProbability = 80;
    int virtualizationLevel = 2;
    int polymorphicVariants = 5;
    std::string outputReportPath = "obfuscation_report.txt";
};

class ObfuscationPass : public ModulePass {
private:
    ObfuscationConfig config;
    std::unique_ptr<RandomNumberGenerator> rng;
    std::map<std::string, int> obfuscationMetrics;
    std::vector<std::pair<GlobalVariable*, unsigned>> encryptedStringGlobals;
    std::set<std::string> originalFunctionNames; // Track original functions to prevent exponential variants
    
    // Metrics tracking
    int totalBogusInstructions = 0;
    int totalFakeLoops = 0;
    int totalStringEncryptions = 0;
    int totalObfuscationCycles = 0;
    int totalInstructionSubstitutions = 0;
    int totalFlattenedFunctions = 0;
    int totalMBATransformations = 0;
    int totalAntiDebugChecks = 0;
    int totalIndirectCalls = 0;
    int totalObfuscatedConstants = 0;
    int totalVirtualizedFunctions = 0;
    int totalPolymorphicVariants = 0;
    int totalAntiAnalysisChecks = 0;
    int totalMetamorphicTransformations = 0;
    int totalDynamicObfuscations = 0;
    
public:
    static char ID;
    
    ObfuscationPass() : ModulePass(ID) {}
    ObfuscationPass(const ObfuscationConfig& cfg) : ModulePass(ID), config(cfg) {}
    
    bool runOnModule(Module &M) override;
    
    // Obfuscation techniques
    bool obfuscateControlFlow(Function &F);
    bool obfuscateStrings(Module &M);
    bool insertBogusCode(Function &F);
    bool insertFakeLoops(Function &F);
    bool substituteInstructions(Function &F);
    bool flattenControlFlow(Function &F);
    bool applyMBA(Function &F);
    bool insertAntiDebug(Module &M);
    bool obfuscateCalls(Module &M);
    bool obfuscateConstants(Function &F);
    bool insertAntiTamper(Module &M);
    bool virtualizeFunction(Function &F);
    bool generatePolymorphicCode(Function &F);
    bool insertAntiAnalysis(Module &M);
    bool applyMetamorphicTransform(Function &F);
    bool insertDynamicObfuscation(Module &M);
    
    // Utility functions
    void generateReport(const Module &M);
    void logMetrics(const std::string& key, int value);
    Value* createOpaquePredicate(IRBuilder<> &Builder);
    void encryptString(GlobalVariable *GV);
    void addDecryptionGlobalCtor(Module &M);
    Value* createMBAExpression(IRBuilder<> &Builder, Value *A, Value *B, int op);
    BasicBlock* createDispatchBlock(Function &F, std::vector<BasicBlock*> &Blocks);
    void insertIntegrityCheck(Module &M, Function *F);
    Function* createVirtualMachine(Module &M);
    void generatePolymorphicVariant(Function &F, int variant);
    void insertAnalysisDetection(Module &M);
    void randomizeCodeStructure(Function &F);
    bool isOriginalFunction(Function &F);
    
    // Getters for metrics
    int getTotalBogusInstructions() const { return totalBogusInstructions; }
    int getTotalFakeLoops() const { return totalFakeLoops; }
    int getTotalStringEncryptions() const { return totalStringEncryptions; }
    int getTotalObfuscationCycles() const { return totalObfuscationCycles; }
    int getTotalInstructionSubstitutions() const { return totalInstructionSubstitutions; }
    int getTotalFlattenedFunctions() const { return totalFlattenedFunctions; }
    int getTotalMBATransformations() const { return totalMBATransformations; }
    int getTotalAntiDebugChecks() const { return totalAntiDebugChecks; }
    int getTotalVirtualizedFunctions() const { return totalVirtualizedFunctions; }
    int getTotalPolymorphicVariants() const { return totalPolymorphicVariants; }
    int getTotalAntiAnalysisChecks() const { return totalAntiAnalysisChecks; }
    int getTotalMetamorphicTransformations() const { return totalMetamorphicTransformations; }
    int getTotalDynamicObfuscations() const { return totalDynamicObfuscations; }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
        AU.setPreservesCFG();
    }
};

} // namespace llvm

#endif // OBFUSCATION_PASS_H
