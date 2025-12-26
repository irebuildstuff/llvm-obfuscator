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

//===----------------------------------------------------------------------===//
// Criticality Analysis for Smart Protection Selection
//===----------------------------------------------------------------------===//

/// Criticality levels for functions - determines protection intensity
enum class CriticalityLevel {
    CRITICAL,    ///< Maximum protection (main, auth, crypto, license functions)
    IMPORTANT,   ///< High protection (business logic, sensitive operations)
    STANDARD,    ///< Normal protection (regular functions)
    MINIMAL      ///< Minimal protection (getters/setters, small utilities)
};

/// Analysis result for a function
struct FunctionAnalysis {
    CriticalityLevel level;
    int complexityScore;      ///< Cyclomatic complexity
    int sensitivityScore;     ///< Keyword/pattern matching score
    int callFrequency;        ///< Number of callers (high = utility function)
    int estimatedSizeGrowth;  ///< Estimated size increase after obfuscation (%)
    bool hasStringOps;        ///< Contains string operations
    bool hasCryptoOps;        ///< Contains crypto-related operations
    bool hasNetworkOps;       ///< Contains network operations
    bool hasFileOps;          ///< Contains file operations
};

/// Size optimization mode
enum class SizeMode {
    NONE,        ///< No size constraints
    MINIMAL,     ///< Minimize size growth (< 1.5x)
    BALANCED,    ///< Balance protection and size (< 3x)
    AGGRESSIVE   ///< Maximum protection, accept any size
};

//===----------------------------------------------------------------------===//
// RC4 Stream Cipher Implementation for Strong String Encryption
//===----------------------------------------------------------------------===//

/// RC4 state structure for stream cipher
struct RC4State {
    uint8_t S[256];  ///< Permutation array
    uint8_t i, j;    ///< State indices
    
    /// Initialize RC4 with key
    void init(const uint8_t* key, size_t keyLen) {
        i = j = 0;
        for (int k = 0; k < 256; k++) {
            S[k] = static_cast<uint8_t>(k);
        }
        uint8_t jj = 0;
        for (int k = 0; k < 256; k++) {
            jj = jj + S[k] + key[k % keyLen];
            std::swap(S[k], S[jj]);
        }
    }
    
    /// Generate next byte of keystream
    uint8_t nextByte() {
        i = i + 1;
        j = j + S[i];
        std::swap(S[i], S[j]);
        return S[(S[i] + S[j]) & 0xFF];
    }
    
    /// Encrypt/decrypt data in place (RC4 is symmetric)
    void process(uint8_t* data, size_t len) {
        for (size_t k = 0; k < len; k++) {
            data[k] ^= nextByte();
        }
    }
};

/// PBKDF2-SHA256 simplified implementation for key derivation
/// This provides protection against known-plaintext attacks
struct PBKDF2 {
    /// Simple hash function (FNV-1a variant for speed)
    static uint64_t fnvHash(const uint8_t* data, size_t len, uint64_t seed = 0xcbf29ce484222325ULL) {
        uint64_t hash = seed;
        for (size_t i = 0; i < len; i++) {
            hash ^= data[i];
            hash *= 0x100000001b3ULL;
        }
        return hash;
    }
    
    /// Derive key from password, salt, and iteration count
    /// Returns 32 bytes (256-bit key)
    static std::vector<uint8_t> deriveKey(
        const uint8_t* password, size_t passLen,
        const uint8_t* salt, size_t saltLen,
        int iterations = 1000
    ) {
        std::vector<uint8_t> result(32, 0);
        
        // Combine password and salt
        std::vector<uint8_t> combined;
        combined.insert(combined.end(), password, password + passLen);
        combined.insert(combined.end(), salt, salt + saltLen);
        
        // Iterate to strengthen key
        uint64_t h1 = fnvHash(combined.data(), combined.size());
        uint64_t h2 = fnvHash(combined.data(), combined.size(), h1);
        uint64_t h3 = fnvHash(combined.data(), combined.size(), h2);
        uint64_t h4 = fnvHash(combined.data(), combined.size(), h3);
        
        for (int iter = 0; iter < iterations; iter++) {
            h1 = fnvHash(reinterpret_cast<uint8_t*>(&h1), 8, h4);
            h2 = fnvHash(reinterpret_cast<uint8_t*>(&h2), 8, h1);
            h3 = fnvHash(reinterpret_cast<uint8_t*>(&h3), 8, h2);
            h4 = fnvHash(reinterpret_cast<uint8_t*>(&h4), 8, h3);
        }
        
        // Pack into result
        for (int i = 0; i < 8; i++) {
            result[i] = (h1 >> (i * 8)) & 0xFF;
            result[i + 8] = (h2 >> (i * 8)) & 0xFF;
            result[i + 16] = (h3 >> (i * 8)) & 0xFF;
            result[i + 24] = (h4 >> (i * 8)) & 0xFF;
        }
        
        return result;
    }
};

/// String encryption method
enum class StringEncryptionMethod {
    XOR_ROTATING,  ///< Legacy: XOR with rotating key (weak)
    RC4_SIMPLE,    ///< RC4 with random key (medium)
    RC4_PBKDF2     ///< RC4 with PBKDF2-derived key from code hash (strong)
};

struct ObfuscationConfig {
    // Core obfuscation techniques
    bool enableControlFlowObfuscation = true;
    bool enableStringEncryption = true;
    
    // String encryption settings
    StringEncryptionMethod stringEncryptionMethod = StringEncryptionMethod::RC4_PBKDF2;
    int pbkdf2Iterations = 1000;  ///< Higher = slower but more secure
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
    
    // Technique parameters
    int obfuscationCycles = 3;
    int bogusCodePercentage = 30;
    int fakeLoopCount = 5;
    int stringEncryptionCount = 0;
    int mbaComplexity = 3;
    int flatteningProbability = 80;
    int virtualizationLevel = 2;
    int polymorphicVariants = 5;
    
    // Size optimization settings
    SizeMode sizeMode = SizeMode::BALANCED;
    int maxSizeGrowthPercent = 200;  ///< Maximum allowed size growth (200 = 2x)
    bool autoSelectTechniques = true; ///< Auto-select based on criticality
    
    // Output paths
    std::string outputReportPath = "obfuscation_report.txt";
};

class ObfuscationPass : public ModulePass {
private:
    ObfuscationConfig config;
    std::unique_ptr<RandomNumberGenerator> rng;
    std::map<std::string, int> obfuscationMetrics;
    // Store encrypted strings with their keys and lengths
    struct EncryptedStringInfo {
        GlobalVariable* GV;
        unsigned length;
        
        // Legacy XOR encryption fields (for backwards compatibility)
        std::vector<uint8_t> keys;  // Multi-byte key for XOR encryption
        uint8_t baseKey;            // Base key for XOR obfuscation
        
        // RC4 + PBKDF2 encryption fields (new strong encryption)
        std::vector<uint8_t> salt;      // 8-byte random salt per string
        std::vector<uint8_t> derivedKey; // 32-byte key from PBKDF2
        StringEncryptionMethod method;   // Which encryption method was used
        uint64_t codeHashSeed;           // Seed from code section hash
    };
    std::vector<EncryptedStringInfo> encryptedStringGlobals;
    
    // Code hash for key derivation (computed once at start)
    uint64_t moduleCodeHash = 0;
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
    void encryptStringRC4(GlobalVariable *GV);  // New RC4 encryption
    void addDecryptionGlobalCtor(Module &M);
    void addDecryptionGlobalCtorRC4(Module &M);  // New RC4 decryption ctor
    Function* createLazyDecryptor(Module &M, GlobalVariable *GV, EncryptedStringInfo &Info);
    Function* createLazyDecryptorRC4(Module &M, GlobalVariable *GV, EncryptedStringInfo &Info);
    uint64_t computeModuleCodeHash(Module &M);  // Compute hash of code section
    Function* createRC4DecryptFunction(Module &M);  // Create RC4 decrypt helper
    Value* createMBAExpression(IRBuilder<> &Builder, Value *A, Value *B, int op);
    BasicBlock* createDispatchBlock(Function &F, std::vector<BasicBlock*> &Blocks);
    void insertIntegrityCheck(Module &M, Function *F);
    Function* createVirtualMachine(Module &M);
    void generatePolymorphicVariant(Function &F, int variant);
    void insertAnalysisDetection(Module &M);
    void randomizeCodeStructure(Function &F);
    bool isOriginalFunction(Function &F);
    bool shouldObfuscateFunction(Function &F);
    
    // Criticality analysis for smart protection selection
    FunctionAnalysis analyzeFunction(Function &F);
    CriticalityLevel determineCriticality(Function &F);
    int calculateComplexity(Function &F);
    int calculateSensitivityScore(Function &F);
    int estimateSizeGrowth(Function &F, const ObfuscationConfig &cfg);
    
    // Size optimization engine
    ObfuscationConfig optimizeForSize(Function &F, int sizeBudgetPercent);
    void applyPreset(const std::string &presetName);
    static ObfuscationConfig getMinimalPreset();
    static ObfuscationConfig getBalancedPreset();
    static ObfuscationConfig getAggressivePreset();
    
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
        // We heavily modify the CFG, so we don't preserve anything
        // This is critical for correctness with other LLVM passes
    }
};

} // namespace llvm

#endif // OBFUSCATION_PASS_H
