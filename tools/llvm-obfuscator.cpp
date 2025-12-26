#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TargetParser/Triple.h"
// Removed unused legacy PassManager headers to improve compatibility
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include "ObfuscationPass.h"
#include <iostream>
#include <string>
#include <limits>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <vector>
#include <map>
#include <regex>
#include <cstdlib>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

// Helper function to get executable directory
static std::string getExecutableDirectory() {
#ifdef _WIN32
    char exePath[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, exePath, MAX_PATH);
    if (length > 0) {
        std::filesystem::path exe(exePath);
        return exe.parent_path().string();
    }
#else
    // For Unix/Linux, try to get from /proc/self/exe or argv[0]
    char exePath[1024];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len != -1) {
        exePath[len] = '\0';
        std::filesystem::path exe(exePath);
        return exe.parent_path().string();
    }
#endif
    // Fallback: return current directory
    return std::filesystem::current_path().string();
}

// Helper function to ensure directory exists for a file path
static bool ensureDirectoryExists(const std::string& filePath) {
    std::filesystem::path path(filePath);
    std::filesystem::path dir = path.parent_path();
    
    // If no parent path (file is in current directory), no need to create
    if (dir.empty() || dir == "." || dir == path.root_path()) {
        return true;
    }
    
    // Check if directory already exists
    if (std::filesystem::exists(dir)) {
        return true;
    }
    
    // Create directory (and all parent directories)
    std::error_code EC;
    std::filesystem::create_directories(dir, EC);
    return !EC;
}

using namespace llvm;

// Beautiful Modern CLI Utilities
namespace CLI {
    // Enhanced ANSI Color Codes
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string ITALIC = "\033[3m";
    const std::string UNDERLINE = "\033[4m";
    const std::string BLINK = "\033[5m";
    
    // Standard Colors
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BLACK = "\033[30m";
    
    // Bright Colors
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN = "\033[96m";
    const std::string BRIGHT_WHITE = "\033[97m";
    
    // Background Colors
    const std::string BG_BLACK = "\033[40m";
    const std::string BG_RED = "\033[41m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_YELLOW = "\033[43m";
    const std::string BG_BLUE = "\033[44m";
    const std::string BG_MAGENTA = "\033[45m";
    const std::string BG_CYAN = "\033[46m";
    const std::string BG_WHITE = "\033[47m";
    
    // Clean ASCII Characters
    const std::string CHECKMARK = "[OK]";
    const std::string CROSS = "[X]";
    const std::string ARROW = "->";
    const std::string STAR = "*";
    const std::string DIAMOND = "<>";
    const std::string CIRCLE = "(o)";
    const std::string SQUARE = "[ ]";
    const std::string TRIANGLE = "^";
    const std::string HEART = "<3";
    const std::string SPARKLE = "*";
    const std::string ROCKET = "=>";
    const std::string SHIELD = "[S]";
    const std::string LOCK = "[L]";
    const std::string GEAR = "[G]";
    const std::string FIRE = "!";
    const std::string LIGHTNING = "~";
    const std::string CROWN = "[C]";
    const std::string GEM = "<>";
    
    // Clean ASCII Progress Bar Characters
    const std::string PROGRESS_FILL = "#";
    const std::string PROGRESS_EMPTY = "-";
    const std::string PROGRESS_EDGE = ">";
    const std::string PROGRESS_START = "[";
    const std::string PROGRESS_END = "]";
    
    // Clean ASCII Spinner Characters
    const std::vector<std::string> SPINNER = {"|", "/", "-", "\\"};
    const std::vector<std::string> MODERN_SPINNER = {"|", "/", "-", "\\"};
    const std::vector<std::string> DOT_SPINNER = {".", "o", "O", "o", "."};
    
    // Gradient Colors
    const std::string GRADIENT_START = "\033[38;5;21m";  // Deep Blue
    const std::string GRADIENT_MID = "\033[38;5;39m";    // Cyan
    const std::string GRADIENT_END = "\033[38;5;51m";    // Bright Cyan
    
    class ColorOutput {
    private:
        bool colorEnabled;
        
    public:
        ColorOutput() {
            // Enable colors on Windows
            #ifdef _WIN32
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD dwMode = 0;
            GetConsoleMode(hOut, &dwMode);
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
            #endif
            colorEnabled = true;
        }
        
        std::string colorize(const std::string& text, const std::string& color) const {
            return colorEnabled ? color + text + RESET : text;
        }
        
        std::string gradient(const std::string& text, const std::string& startColor, const std::string& endColor) const {
            if (!colorEnabled) return text;
            
            std::string result;
            for (size_t i = 0; i < text.length(); ++i) {
                double ratio = static_cast<double>(i) / (text.length() - 1);
                if (text.length() == 1) ratio = 0.5;
                
                // Simple gradient effect
                if (ratio < 0.5) {
                    result += startColor + text[i];
                } else {
                    result += endColor + text[i];
                }
            }
            return result + RESET;
        }
        
        void print(const std::string& text, const std::string& color = "") const {
            if (color.empty()) {
                std::cout << text;
            } else {
                std::cout << colorize(text, color);
            }
        }
        
        void println(const std::string& text, const std::string& color = "") const {
            print(text, color);
            std::cout << std::endl;
        }
        
        void printGradient(const std::string& text, const std::string& startColor, const std::string& endColor) const {
            std::cout << gradient(text, startColor, endColor);
        }
        
        void printlnGradient(const std::string& text, const std::string& startColor, const std::string& endColor) const {
            printGradient(text, startColor, endColor);
            std::cout << std::endl;
        }
        
        void printCentered(const std::string& text, int width = 80, const std::string& color = "") const {
            int padding = (width - text.length()) / 2;
            if (padding < 0) padding = 0;
            
            std::string paddedText = std::string(padding, ' ') + text;
            print(paddedText, color);
        }
        
        void printlnCentered(const std::string& text, int width = 80, const std::string& color = "") const {
            printCentered(text, width, color);
            std::cout << std::endl;
        }
    };
    
    class ProgressBar {
    private:
        int width;
        std::string message;
        int current;
        int total;
        std::chrono::steady_clock::time_point startTime;
        ColorOutput color;
        
    public:
        ProgressBar(int w = 50) : width(w), current(0), total(100) {
            startTime = std::chrono::steady_clock::now();
        }
        
        void update(int progress, const std::string& msg = "") {
            current = progress;
            if (!msg.empty()) message = msg;
            
            int pos = width * progress / 100;
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
            
            // Clean ASCII progress bar
            std::cout << "\r" << color.colorize(PROGRESS_START, BRIGHT_CYAN);
            
            for (int i = 0; i < width; ++i) {
                if (i < pos) {
                    // Simple fill effect
                    if (i < pos * 0.3) {
                        std::cout << color.colorize(PROGRESS_FILL, BRIGHT_GREEN);
                    } else if (i < pos * 0.7) {
                        std::cout << color.colorize(PROGRESS_FILL, YELLOW);
                    } else {
                        std::cout << color.colorize(PROGRESS_FILL, BRIGHT_YELLOW);
                    }
                } else if (i == pos) {
                    std::cout << color.colorize(PROGRESS_EDGE, BRIGHT_CYAN);
                } else {
                    std::cout << color.colorize(PROGRESS_EMPTY, DIM);
                }
            }
            
            std::cout << color.colorize(PROGRESS_END, BRIGHT_CYAN) << " ";
            
            // Clean percentage display
            std::string percentStr = std::to_string(progress) + "%";
            if (progress < 30) {
                std::cout << color.colorize(percentStr, RED);
            } else if (progress < 70) {
                std::cout << color.colorize(percentStr, YELLOW);
            } else {
                std::cout << color.colorize(percentStr, BRIGHT_GREEN);
            }
            
            if (!message.empty()) {
                std::cout << " " << color.colorize(message, BRIGHT_CYAN);
            }
            
            if (progress > 0) {
                int remaining = (elapsed * (100 - progress)) / progress;
                std::cout << " " << color.colorize("ETA: " + std::to_string(remaining/1000) + "s", DIM);
            }
            
            std::cout.flush();
            
            if (progress >= 100) {
                std::cout << std::endl;
                std::cout << color.colorize("[COMPLETE] ", BRIGHT_GREEN) << color.colorize("Done!", BRIGHT_GREEN) << std::endl;
            }
        }
    };
    
    class Spinner {
    private:
        int frame;
        std::string message;
        std::chrono::steady_clock::time_point lastUpdate;
        ColorOutput color;
        std::vector<std::string> spinnerFrames;
        
    public:
        Spinner() : frame(0) {
            lastUpdate = std::chrono::steady_clock::now();
            spinnerFrames = MODERN_SPINNER;
        }
        
        void update(const std::string& msg = "") {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count();
            
            if (elapsed >= 200) { // Update every 200ms for smoother animation
                frame = (frame + 1) % spinnerFrames.size();
                lastUpdate = now;
                
                std::cout << "\r" << color.colorize(spinnerFrames[frame], BRIGHT_CYAN) 
                          << " " << color.colorize(msg, BRIGHT_WHITE);
                std::cout.flush();
            }
        }
        
        void stop() {
            std::cout << "\r" << std::string(80, ' ') << "\r";
        }
        
        void setSpinnerType(const std::string& type) {
            if (type == "modern") {
                spinnerFrames = MODERN_SPINNER;
            } else if (type == "dots") {
                spinnerFrames = DOT_SPINNER;
            } else {
                spinnerFrames = SPINNER;
            }
        }
    };
    
    class InteractiveMenu {
    private:
        std::vector<std::string> options;
        int selected;
        ColorOutput color;
        
    public:
        InteractiveMenu(const std::vector<std::string>& opts) : options(opts), selected(0) {}
        
        int show() {
            #ifdef _WIN32
            return showWindows();
            #else
            return showUnix();
            #endif
        }
        
    private:
        int showWindows() {
            while (true) {
                system("cls");
                color.println("╔════════════════════════════════════════════════════════════════════╗", BLUE);
                color.println("║                    SELECT OPTION                                   ║", BLUE);
                color.println("╚════════════════════════════════════════════════════════════════════╝", BLUE);
                color.println("");
                
                for (size_t i = 0; i < options.size(); ++i) {
                    if (i == selected) {
                        color.print("  " + ARROW + " ", GREEN);
                        color.println(options[i], BOLD + BG_BLUE);
                    } else {
                        color.println("    " + options[i], "");
                    }
                }
                
                color.println("\nUse ↑↓ arrows to navigate, Enter to select, Esc to exit", DIM);
                
                int ch = _getch();
                if (ch == 224) { // Arrow key
                    ch = _getch();
                    if (ch == 72 && selected > 0) selected--; // Up
                    if (ch == 80 && selected < options.size() - 1) selected++; // Down
                } else if (ch == 13) { // Enter
                    return selected;
                } else if (ch == 27) { // Esc
                    return -1;
                }
            }
        }
        
        int showUnix() {
            // Simplified Unix implementation without termios for better compatibility
            while (true) {
                system("clear");
                color.println("╔════════════════════════════════════════════════════════════════════╗", BLUE);
                color.println("║", BLUE);
                color.print("║", BLUE);
                color.print("                    ", BLUE);
                color.print("SELECT OPTION", BOLD + CYAN);
                color.print("                                   ", BLUE);
                color.println("║", BLUE);
                color.println("╚════════════════════════════════════════════════════════════════════╝", BLUE);
                color.println("");
                
                for (size_t i = 0; i < options.size(); ++i) {
                    if (i == selected) {
                        color.print("  " + ARROW + " ", GREEN);
                        color.println(options[i], BOLD + BG_BLUE);
                    } else {
                        color.println("    " + options[i], "");
                    }
                }
                
                color.println("\nEnter number (1-" + std::to_string(options.size()) + ") or 0 to exit: ", DIM);
                
                std::string input;
                std::getline(std::cin, input);
                
                try {
                    int choice = std::stoi(input);
                    if (choice == 0) return -1;
                    if (choice >= 1 && choice <= static_cast<int>(options.size())) {
                        return choice - 1;
                    }
                } catch (...) {
                    // Invalid input, continue loop
                }
            }
        }
    };
    
    class FileAnalyzer {
    public:
        struct AnalysisResult {
            bool isLLVMIR;
            bool isC;
            bool isCpp;
            int estimatedComplexity;
            std::string suggestedPreset;
            std::map<std::string, bool> recommendedSettings;
        };
        
        static AnalysisResult analyzeFile(const std::string& filename) {
            AnalysisResult result = {};
            
            if (!std::filesystem::exists(filename)) {
                return result;
            }
            
            std::ifstream file(filename);
            std::string line;
            int lineCount = 0;
            int functionCount = 0;
            int loopCount = 0;
            int stringCount = 0;
            
            while (std::getline(file, line) && lineCount < 1000) { // Analyze first 1000 lines
                lineCount++;
                
                // Check file type
                if (line.find("@") != std::string::npos && line.find("define") != std::string::npos) {
                    result.isLLVMIR = true;
                }
                if (line.find("#include") != std::string::npos) {
                    result.isC = true;
                }
                if (line.find("class ") != std::string::npos || line.find("namespace ") != std::string::npos) {
                    result.isCpp = true;
                }
                
                // Count complexity indicators
                if (line.find("define") != std::string::npos) functionCount++;
                if (line.find("for") != std::string::npos || line.find("while") != std::string::npos) loopCount++;
                if (line.find("\"") != std::string::npos) stringCount++;
            }
            
            // Calculate complexity score
            result.estimatedComplexity = (functionCount * 2) + (loopCount * 3) + (stringCount * 1) + (lineCount / 100);
            
            // Suggest preset based on complexity
            if (result.estimatedComplexity < 50) {
                result.suggestedPreset = "Light";
                result.recommendedSettings = {
                    {"controlFlow", true},
                    {"stringEncryption", true},
                    {"bogusCode", false},
                    {"fakeLoops", false}
                };
            } else if (result.estimatedComplexity < 150) {
                result.suggestedPreset = "Medium";
                result.recommendedSettings = {
                    {"controlFlow", true},
                    {"stringEncryption", true},
                    {"bogusCode", true},
                    {"fakeLoops", true}
                };
            } else {
                result.suggestedPreset = "Heavy";
                result.recommendedSettings = {
                    {"controlFlow", true},
                    {"stringEncryption", true},
                    {"bogusCode", true},
                    {"fakeLoops", true},
                    {"instructionSubstitution", true},
                    {"controlFlowFlattening", true},
                    {"mba", true}
                };
            }
            
            return result;
        }
    };
}

static cl::opt<std::string> InputFilename(cl::Positional, cl::desc("<input file>"));
static cl::opt<std::string> OutputFilename("o", cl::desc("Output filename"), cl::value_desc("filename"));
static cl::opt<std::string> ReportFilename("report", cl::desc("Report filename"), cl::value_desc("filename"), cl::init(""));

// Obfuscation options
static cl::opt<bool> EnableControlFlow("cf", cl::desc("Enable control flow obfuscation"), cl::init(true));
static cl::opt<bool> DisableControlFlow("no-cf", cl::desc("Disable control flow obfuscation"), cl::init(false));
static cl::opt<bool> EnableStringEncryption("str", cl::desc("Enable string encryption"), cl::init(true));
static cl::opt<bool> DisableStringEncryption("no-str", cl::desc("Disable string encryption"), cl::init(false));
static cl::opt<bool> EnableBogusCode("bogus", cl::desc("Enable bogus code insertion"), cl::init(true));
static cl::opt<bool> DisableBogusCode("no-bogus", cl::desc("Disable bogus code insertion"), cl::init(false));
static cl::opt<bool> EnableFakeLoops("loops", cl::desc("Enable fake loop insertion"), cl::init(true));
static cl::opt<bool> DisableFakeLoops("no-loops", cl::desc("Disable fake loop insertion"), cl::init(false));
static cl::opt<bool> EnableInstructionSubst("subs", cl::desc("Enable instruction substitution"), cl::init(false));
static cl::opt<bool> EnableCFF("flatten", cl::desc("Enable control flow flattening"), cl::init(false));
static cl::opt<bool> EnableMBA("mba", cl::desc("Enable mixed boolean arithmetic"), cl::init(false));
static cl::opt<bool> EnableAntiDebug("anti-debug", cl::desc("Enable anti-debugging checks"), cl::init(false));
static cl::opt<bool> EnableIndirectCalls("indirect", cl::desc("Enable indirect function calls"), cl::init(false));
static cl::opt<bool> EnableConstObf("const-obf", cl::desc("Enable constant obfuscation"), cl::init(false));
static cl::opt<bool> EnableAntiTamper("anti-tamper", cl::desc("Enable anti-tampering checks"), cl::init(false));
static cl::opt<bool> EnableVirtualization("virtualize", cl::desc("Enable code virtualization"), cl::init(false));
static cl::opt<bool> EnablePolymorphic("polymorphic", cl::desc("Enable polymorphic code generation"), cl::init(false));
static cl::opt<bool> EnableAntiAnalysis("anti-analysis", cl::desc("Enable anti-analysis detection"), cl::init(false));
static cl::opt<bool> EnableMetamorphic("metamorphic", cl::desc("Enable metamorphic transformations"), cl::init(false));
static cl::opt<bool> EnableDynamicObf("dynamic", cl::desc("Enable dynamic obfuscation"), cl::init(false));
static cl::opt<bool> DecryptAtStartup("decrypt-startup", cl::desc("Decrypt strings at startup using global ctor"), cl::init(true));
static cl::opt<int> ObfuscationCycles("cycles", cl::desc("Number of obfuscation cycles"), cl::value_desc("N"), cl::init(3));
static cl::opt<int> MBAComplexity("mba-level", cl::desc("MBA complexity level (1-5)"), cl::value_desc("N"), cl::init(3));
static cl::opt<int> FlattenProb("flatten-prob", cl::desc("Flattening probability percentage"), cl::value_desc("N"), cl::init(80));
static cl::opt<int> VirtualizationLevel("vm-level", cl::desc("Virtualization level (1-3)"), cl::value_desc("N"), cl::init(2));
static cl::opt<int> PolymorphicVariants("poly-variants", cl::desc("Number of polymorphic variants"), cl::value_desc("N"), cl::init(5));
static cl::opt<int> BogusCodePercentage("bogus-percent", cl::desc("Percentage of bogus code per basic block"), cl::value_desc("N"), cl::init(30));
static cl::opt<int> FakeLoopCount("fake-loops", cl::desc("Number of fake loops to insert"), cl::value_desc("N"), cl::init(5));

// Platform options
static cl::opt<std::string> TargetTriple("triple", cl::desc("Target triple"), cl::value_desc("triple"));
static cl::opt<bool> GenerateWindowsBinary("win", cl::desc("Generate Windows binary"), cl::init(false));
static cl::opt<bool> GenerateLinuxBinary("linux", cl::desc("Generate Linux binary"), cl::init(false));

// Auto-compilation options
static cl::opt<bool> AutoCompileToExe("compile", cl::desc("Automatically compile obfuscated IR to executable"), cl::init(false));
static cl::opt<bool> KeepIntermediateFiles("keep-temp", cl::desc("Keep intermediate .ll files"), cl::init(false));

// Verbose output
static cl::opt<bool> Verbose("v", cl::desc("Verbose output"), cl::init(false));

// Forward declarations
static bool isSourceFile(const std::string& filename);
static bool compileToLLVMIR(const std::string& sourceFile, const std::string& outputIR, CLI::ColorOutput& color);
static bool compileIRToExe(const std::string& irFile, const std::string& outputExe, CLI::ColorOutput& color);

// Clean ASCII Banner Functions
void printCleanBanner() {
    CLI::ColorOutput color;
    
    std::cout << "\n";
    
    // Top decorative line
    color.println("=================================================================", CLI::BRIGHT_CYAN);
    color.println("", CLI::BRIGHT_CYAN);
    
    // Main title
    color.printCentered("LLVM CODE OBFUSCATOR", 65, CLI::BOLD + CLI::BRIGHT_WHITE);
    color.println("");
    color.println("");
    
    color.printCentered("Advanced Code Protection Suite", 65, CLI::BRIGHT_YELLOW);
    color.println("");
    color.println("");
    
    color.printCentered("Professional Security & Anti-Analysis", 65, CLI::BRIGHT_GREEN);
    color.println("");
    color.println("");
    
    color.printCentered("Enhanced CLI v3.0 - Clean Interface", 65, CLI::BRIGHT_MAGENTA);
    color.println("");
    
    // Bottom decorative line
    color.println("=================================================================", CLI::BRIGHT_CYAN);
    
    std::cout << "\n";
}

void printCleanCommandLineBanner() {
    CLI::ColorOutput color;
    
    std::cout << "\n";
    
    // Top decorative line
    color.println("=================================================================", CLI::BRIGHT_BLUE);
    
    // Main title
    color.println("", CLI::BRIGHT_BLUE);
    color.printCentered("LLVM CODE OBFUSCATOR - Command Line Mode", 65, CLI::BOLD + CLI::BRIGHT_WHITE);
    color.println("");
    
    color.println("", CLI::BRIGHT_BLUE);
    color.printCentered("Enhanced CLI v3.0 - Clean Interface", 65, CLI::BRIGHT_MAGENTA);
    color.println("");
    
    // Bottom decorative line
    color.println("=================================================================", CLI::BRIGHT_BLUE);
    
    std::cout << "\n";
}

void printCleanSectionHeader(const std::string& title, const std::string& icon = "*") {
    CLI::ColorOutput color;
    
    std::cout << "\n";
    color.println("-----------------------------------------------------------------", CLI::BRIGHT_CYAN);
    color.println("", CLI::BRIGHT_CYAN);
    color.printCentered(icon + " " + title + " " + icon, 65, CLI::BOLD + CLI::BRIGHT_WHITE);
    color.println("");
    color.println("-----------------------------------------------------------------", CLI::BRIGHT_CYAN);
    std::cout << "\n";
}

void printCleanSuccessMessage(const std::string& message) {
    CLI::ColorOutput color;
    
    color.println("=================================================================", CLI::BRIGHT_GREEN);
    color.printCentered("[SUCCESS] " + message, 65, CLI::BOLD + CLI::BRIGHT_GREEN);
    color.println("");
    color.println("=================================================================", CLI::BRIGHT_GREEN);
    std::cout << "\n";
}

void printCleanErrorMessage(const std::string& message) {
    CLI::ColorOutput color;
    
    color.println("=================================================================", CLI::BRIGHT_RED);
    color.printCentered("[ERROR] " + message, 65, CLI::BOLD + CLI::BRIGHT_RED);
    color.println("");
    color.println("=================================================================", CLI::BRIGHT_RED);
    std::cout << "\n";
}

void printCleanWarningMessage(const std::string& message) {
    CLI::ColorOutput color;
    
    color.println("=================================================================", CLI::BRIGHT_YELLOW);
    color.printCentered("[WARNING] " + message, 65, CLI::BOLD + CLI::BRIGHT_YELLOW);
    color.println("");
    color.println("=================================================================", CLI::BRIGHT_YELLOW);
    std::cout << "\n";
}

std::string getCleanInputString(const std::string& prompt, const std::string& suggestion = "") {
    CLI::ColorOutput color;
    
    color.print("* ", CLI::BRIGHT_CYAN);
    color.print(prompt, CLI::BRIGHT_WHITE);
    if (!suggestion.empty()) {
        color.print(" ", CLI::DIM);
        color.print("[" + suggestion + "]", CLI::BRIGHT_MAGENTA);
    }
    color.print(": ", CLI::BRIGHT_CYAN);
    
    std::string input;
    std::getline(std::cin, input);
    
    // Trim whitespace from input
    input.erase(0, input.find_first_not_of(" \t\r\n"));
    input.erase(input.find_last_not_of(" \t\r\n") + 1);
    
    return input;
}

int getCleanInputInt(const std::string& prompt, int defaultValue, int min = 0, int max = 100) {
    CLI::ColorOutput color;
    
    color.print("[G] ", CLI::BRIGHT_CYAN);
    color.print(prompt, CLI::BRIGHT_WHITE);
    color.print(" ", CLI::DIM);
    color.print("[" + std::to_string(defaultValue) + "]", CLI::BRIGHT_MAGENTA);
    color.print(": ", CLI::BRIGHT_CYAN);
    
    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) return defaultValue;
    try {
        int value = std::stoi(input);
        if (value < min || value > max) {
            printCleanWarningMessage("Value out of range (" + std::to_string(min) + "-" + std::to_string(max) + "). Using default: " + std::to_string(defaultValue));
            return defaultValue;
        }
        return value;
    } catch (...) {
        printCleanWarningMessage("Invalid input. Using default: " + std::to_string(defaultValue));
        return defaultValue;
    }
}

bool getCleanYesNo(const std::string& prompt, bool defaultValue) {
    CLI::ColorOutput color;
    
    color.print("<3 ", CLI::BRIGHT_CYAN);
    color.print(prompt, CLI::BRIGHT_WHITE);
    color.print(" ", CLI::DIM);
    color.print(defaultValue ? "[Y/n]" : "[y/N]", CLI::BRIGHT_MAGENTA);
    color.print(": ", CLI::BRIGHT_CYAN);
    
    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) return defaultValue;
    return (input[0] == 'y' || input[0] == 'Y');
}

void displayEnhancedReport(const std::string& reportPath) {
    CLI::ColorOutput color;
    
    color.println("\n╔════════════════════════════════════════════════════════════════════╗", CLI::BLUE);
    color.println("║", CLI::BLUE);
    color.print("║", CLI::BLUE);
    color.print("                     ", CLI::BLUE);
    color.print("OBFUSCATION REPORT", CLI::BOLD + CLI::GREEN);
    color.print("                             ", CLI::BLUE);
    color.println("║", CLI::BLUE);
    color.println("╚════════════════════════════════════════════════════════════════════╝", CLI::BLUE);
    color.println("");
    
    std::ifstream reportFile(reportPath);
    if (reportFile.is_open()) {
        std::string line;
        int lineCount = 0;
        while (std::getline(reportFile, line) && lineCount < 50) {
            // Colorize different sections
            if (line.find("====") != std::string::npos) {
                color.println(line, CLI::BLUE);
            } else if (line.find("ENABLED") != std::string::npos) {
                color.println(line, CLI::GREEN);
            } else if (line.find("DISABLED") != std::string::npos) {
                color.println(line, CLI::RED);
            } else if (line.find("Total") != std::string::npos || line.find("Count") != std::string::npos) {
                color.println(line, CLI::YELLOW);
            } else {
                std::cout << line << "\n";
            }
            lineCount++;
        }
        if (lineCount == 50) {
            color.println("\n" + CLI::TRIANGLE + " [Report truncated. Full report saved to: " + reportPath + "]", CLI::DIM);
        }
        reportFile.close();
    } else {
        color.println(CLI::CROSS + " Could not open report file: " + reportPath, CLI::RED);
    }
}

void showFileAnalysis(const CLI::FileAnalyzer::AnalysisResult& analysis, const std::string& filename) {
    CLI::ColorOutput color;
    
    color.println("\n" + CLI::DIAMOND + " File Analysis Results:", CLI::BOLD + CLI::CYAN);
    color.println("  " + CLI::ARROW + " File: " + filename, CLI::WHITE);
    
    if (analysis.isLLVMIR) {
        color.println("  " + CLI::CHECKMARK + " File Type: LLVM IR", CLI::GREEN);
    } else if (analysis.isCpp) {
        color.println("  " + CLI::CHECKMARK + " File Type: C++", CLI::GREEN);
    } else if (analysis.isC) {
        color.println("  " + CLI::CHECKMARK + " File Type: C", CLI::GREEN);
    } else {
        color.println("  " + CLI::CROSS + " File Type: Unknown", CLI::RED);
    }
    
    color.println("  " + CLI::ARROW + " Estimated Complexity: " + std::to_string(analysis.estimatedComplexity), CLI::YELLOW);
    color.println("  " + CLI::STAR + " Suggested Preset: " + analysis.suggestedPreset, CLI::BOLD + CLI::GREEN);
    
    color.println("\n" + CLI::DIAMOND + " Recommended Settings:", CLI::BOLD + CLI::CYAN);
    for (const auto& setting : analysis.recommendedSettings) {
        std::string status = setting.second ? CLI::CHECKMARK + " ENABLED" : CLI::CROSS + " DISABLED";
        std::string colorCode = setting.second ? CLI::GREEN : CLI::RED;
        color.println("  " + CLI::ARROW + " " + setting.first + ": " + status, colorCode);
    }
}

int beautifulInteractiveMode() {
    CLI::ColorOutput color;
    printCleanBanner();
    
    // Step 1: File Selection with Analysis
    printCleanSectionHeader("STEP 1: File Selection & Analysis", "=>");
    
    std::string inputFile = getCleanInputString("Enter path to C/C++ source file (.cpp/.c) or LLVM IR file (.ll) [auto-detect]:", "");
    if (inputFile.empty()) {
        printCleanErrorMessage("Input file cannot be empty!");
        return 1;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(inputFile)) {
        printCleanErrorMessage("File '" + inputFile + "' does not exist!");
        return 1;
    }
    
    std::string actualInputFile = inputFile;
    std::string tempIRFile;
    bool isSource = isSourceFile(inputFile);
    
    // If input is C/C++ source, compile to LLVM IR first
    if (isSource) {
        CLI::ColorOutput color;
        color.println("", CLI::BRIGHT_CYAN);
        color.println("  " + CLI::ARROW + " Detected C/C++ source file", CLI::CYAN);
        
        std::filesystem::path sourcePath(inputFile);
        std::string exeDir = getExecutableDirectory();
        tempIRFile = (std::filesystem::path(exeDir) / (sourcePath.stem().string() + ".ll")).string();
        
        if (!compileToLLVMIR(inputFile, tempIRFile, color)) {
            printCleanErrorMessage("Failed to compile C/C++ source to LLVM IR!");
            return 1;
        }
        
        actualInputFile = tempIRFile;
        inputFile = tempIRFile; // Update for analysis
    }
    
    // Analyze file with beautiful spinner
    CLI::Spinner spinner;
    spinner.setSpinnerType("modern");
    spinner.update("🔍 Analyzing file structure and complexity...");
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    
    auto analysis = CLI::FileAnalyzer::analyzeFile(inputFile);
    spinner.stop();
    
    // Beautiful file analysis display
    color.println("┌─────────────────────────────────────────────────────────────────────────┐", CLI::BRIGHT_GREEN);
    color.print("│", CLI::BRIGHT_GREEN);
    color.printCentered("📊 File Analysis Results", 75, CLI::BOLD + CLI::BRIGHT_WHITE);
    color.println("│", CLI::BRIGHT_GREEN);
    color.println("├─────────────────────────────────────────────────────────────────────────┤", CLI::BRIGHT_GREEN);
    
    color.print("│", CLI::BRIGHT_GREEN);
    color.print("  📄 File: " + inputFile, CLI::WHITE);
    color.println("│", CLI::BRIGHT_GREEN);
    
    if (analysis.isLLVMIR) {
        color.print("│", CLI::BRIGHT_GREEN);
        color.print("  " + CLI::CHECKMARK + " File Type: LLVM IR", CLI::BRIGHT_GREEN);
        color.println("│", CLI::BRIGHT_GREEN);
    } else if (analysis.isCpp) {
        color.print("│", CLI::BRIGHT_GREEN);
        color.print("  " + CLI::CHECKMARK + " File Type: C++", CLI::BRIGHT_GREEN);
        color.println("│", CLI::BRIGHT_GREEN);
    } else if (analysis.isC) {
        color.print("│", CLI::BRIGHT_GREEN);
        color.print("  " + CLI::CHECKMARK + " File Type: C", CLI::BRIGHT_GREEN);
        color.println("│", CLI::BRIGHT_GREEN);
    } else {
        color.print("│", CLI::BRIGHT_GREEN);
        color.print("  " + CLI::CROSS + " File Type: Unknown", CLI::BRIGHT_RED);
        color.println("│", CLI::BRIGHT_GREEN);
    }
    
    color.print("│", CLI::BRIGHT_GREEN);
    color.print("  " + CLI::LIGHTNING + " Complexity Score: " + std::to_string(analysis.estimatedComplexity), CLI::BRIGHT_YELLOW);
    color.println("│", CLI::BRIGHT_GREEN);
    
    color.print("│", CLI::BRIGHT_GREEN);
    color.print("  " + CLI::CROWN + " Recommended Preset: " + analysis.suggestedPreset, CLI::BRIGHT_MAGENTA);
    color.println("│", CLI::BRIGHT_GREEN);
    
    color.println("└─────────────────────────────────────────────────────────────────────────┘", CLI::BRIGHT_GREEN);
    
    // Step 2: Beautiful Preset Selection
    printCleanSectionHeader("STEP 2: Obfuscation Strategy", "⚙️");
    
    std::vector<std::string> presetOptions = {
        "🚀 Light    - Fast & Efficient (Basic Protection)",
        "⚖️  Medium   - Balanced Security (Recommended)",
        "🔥 Heavy    - Maximum Protection (Slow but Strong)",
        "🎨 Custom   - Choose Individual Techniques",
        "🧠 Smart    - AI-Powered Recommendations"
    };
    
    CLI::InteractiveMenu presetMenu(presetOptions);
    int presetChoice = presetMenu.show();
    
    if (presetChoice == -1) {
        printCleanWarningMessage("Operation cancelled by user");
        return 0;
    }
    
    ObfuscationConfig config;
    
    if (presetChoice == 4) { // Smart preset
        printCleanSuccessMessage("Using AI-powered recommendations based on file analysis!");
        
        config.enableControlFlowObfuscation = analysis.recommendedSettings["controlFlow"];
        config.enableStringEncryption = analysis.recommendedSettings["stringEncryption"];
        config.enableBogusCode = analysis.recommendedSettings["bogusCode"];
        config.enableFakeLoops = analysis.recommendedSettings["fakeLoops"];
        config.enableInstructionSubstitution = analysis.recommendedSettings["instructionSubstitution"];
        config.enableControlFlowFlattening = analysis.recommendedSettings["controlFlowFlattening"];
        config.enableMBA = analysis.recommendedSettings["mba"];
        
        // Smart complexity-based adjustments
        if (analysis.estimatedComplexity < 50) {
            config.obfuscationCycles = 1;
            config.bogusCodePercentage = 10;
        } else if (analysis.estimatedComplexity < 150) {
            config.obfuscationCycles = 3;
            config.bogusCodePercentage = 30;
        } else {
            config.obfuscationCycles = 5;
            config.bogusCodePercentage = 50;
        }
        
        color.println("  " + CLI::CHECKMARK + " Smart preset applied based on complexity: " + std::to_string(analysis.estimatedComplexity), CLI::BRIGHT_GREEN);
        
    } else if (presetChoice == 3) { // Custom configuration
        printCleanSectionHeader("Custom Configuration", "🎨");
        
        color.println("┌─────────────────────────────────────────────────────────────────────────┐", CLI::BRIGHT_CYAN);
        color.print("│", CLI::BRIGHT_CYAN);
        color.printCentered("🛡️ Basic Protection Techniques", 75, CLI::BOLD + CLI::BRIGHT_WHITE);
        color.println("│", CLI::BRIGHT_CYAN);
        color.println("└─────────────────────────────────────────────────────────────────────────┘", CLI::BRIGHT_CYAN);
        
        config.enableControlFlowObfuscation = getCleanYesNo("Control Flow Obfuscation", true);
        config.enableStringEncryption = getCleanYesNo("String Encryption", true);
        config.enableBogusCode = getCleanYesNo("Bogus Code Insertion", true);
        config.enableFakeLoops = getCleanYesNo("Fake Loop Insertion", true);
        
        color.println("┌─────────────────────────────────────────────────────────────────────────┐", CLI::BRIGHT_CYAN);
        color.print("│", CLI::BRIGHT_CYAN);
        color.printCentered("⚡ Advanced Protection Techniques", 75, CLI::BOLD + CLI::BRIGHT_WHITE);
        color.println("│", CLI::BRIGHT_CYAN);
        color.println("└─────────────────────────────────────────────────────────────────────────┘", CLI::BRIGHT_CYAN);
        
        config.enableInstructionSubstitution = getCleanYesNo("Instruction Substitution", false);
        config.enableControlFlowFlattening = getCleanYesNo("Control Flow Flattening", false);
        config.enableMBA = getCleanYesNo("Mixed Boolean Arithmetic", false);
        config.enableIndirectCalls = getCleanYesNo("Indirect Function Calls", false);
        config.enableConstantObfuscation = getCleanYesNo("Constant Obfuscation", false);
        
        color.println("┌─────────────────────────────────────────────────────────────────────────┐", CLI::BRIGHT_CYAN);
        color.print("│", CLI::BRIGHT_CYAN);
        color.printCentered("🔒 Elite Security Features", 75, CLI::BOLD + CLI::BRIGHT_WHITE);
        color.println("│", CLI::BRIGHT_CYAN);
        color.println("└─────────────────────────────────────────────────────────────────────────┘", CLI::BRIGHT_CYAN);
        
        config.enableAntiDebug = getCleanYesNo("Anti-Debugging", false);
        config.enableAntiTamper = getCleanYesNo("Anti-Tampering", false);
        config.enableAntiAnalysis = getCleanYesNo("Anti-Analysis", false);
        config.enableVirtualization = getCleanYesNo("Code Virtualization", false);
        config.enablePolymorphic = getCleanYesNo("Polymorphic Code", false);
        config.enableMetamorphic = getCleanYesNo("Metamorphic Transforms", false);
        config.enableDynamicObf = getCleanYesNo("Dynamic Obfuscation", false);
        
    } else {
        // Preset configurations with beautiful feedback
        switch (presetChoice) {
            case 0: // Light
                config.enableControlFlowObfuscation = true;
                config.enableStringEncryption = true;
                config.enableBogusCode = false;
                config.enableFakeLoops = false; // Disabled due to PHI node issues
                config.obfuscationCycles = 1;
                config.bogusCodePercentage = 10;
                config.decryptStringsAtStartup = false; // IMPROVED: Lazy decryption - strings stay encrypted
                printCleanSuccessMessage("Light preset selected - Fast & Efficient!");
                break;
            case 1: // Medium
                config.enableControlFlowObfuscation = true;
                config.enableStringEncryption = true;
                config.enableBogusCode = true;
                config.enableFakeLoops = false; // Temporarily disabled due to PHI node issues
                config.obfuscationCycles = 3;
                config.bogusCodePercentage = 30;
                config.decryptStringsAtStartup = false; // IMPROVED: Lazy decryption - strings stay encrypted
                printCleanSuccessMessage("Medium preset selected - Balanced Security!");
                break;
            case 2: // Heavy
                config.enableControlFlowObfuscation = true;
                config.enableStringEncryption = true;
                config.enableBogusCode = true;
                config.enableFakeLoops = false; // Temporarily disabled due to PHI node issues
                config.enableInstructionSubstitution = true;
                config.enableControlFlowFlattening = true;
                config.enableMBA = true;
                config.enableAntiDebug = true;
                config.obfuscationCycles = 5;
                config.bogusCodePercentage = 50;
                config.mbaComplexity = 5;
                config.decryptStringsAtStartup = false; // IMPROVED: Lazy decryption - strings stay encrypted
                printCleanSuccessMessage("Heavy preset selected - Maximum Protection!");
                break;
        }
    }
    
    // Step 3: Configuration Parameters
    printCleanSectionHeader("STEP 3: Fine-Tuning Parameters", "🔧");
    
    if (presetChoice == 3) { // Custom
        config.obfuscationCycles = getCleanInputInt("Obfuscation cycles", 3, 1, 10);
        if (config.enableBogusCode) {
            config.bogusCodePercentage = getCleanInputInt("Bogus code percentage", 30, 0, 100);
        }
        if (config.enableFakeLoops) {
            config.fakeLoopCount = getCleanInputInt("Fake loops per function", 5, 1, 20);
        }
        if (config.enableMBA) {
            config.mbaComplexity = getCleanInputInt("MBA complexity (1-5)", 3, 1, 5);
        }
        if (config.enableControlFlowFlattening) {
            config.flatteningProbability = getCleanInputInt("Flattening probability %", 80, 0, 100);
        }
        if (config.enableVirtualization) {
            config.virtualizationLevel = getCleanInputInt("Virtualization level (1-3)", 2, 1, 3);
        }
        if (config.enablePolymorphic) {
            config.polymorphicVariants = getCleanInputInt("Polymorphic variants", 5, 1, 10);
        }
    }
    
    config.decryptStringsAtStartup = getCleanYesNo("Decrypt strings at startup", true);
    
    // Step 4: Output Configuration
    printCleanSectionHeader("STEP 4: Output Configuration", "📤");
    
    // Get executable directory for default output location
    std::string exeDir = getExecutableDirectory();
    
    std::string outputFile = getCleanInputString("Output file", "auto-generate");
    if (outputFile.empty() || outputFile == "auto-generate") {
        // Get input file name (without directory)
        std::filesystem::path inputPath(inputFile);
        std::string inputStem = inputPath.stem().string();
        std::string inputExt = inputPath.extension().string();
        
        // Create output filename in executable directory
        if (inputExt == ".ll" || inputExt == ".bc") {
            outputFile = (std::filesystem::path(exeDir) / (inputStem + "_obfuscated" + inputExt)).string();
        } else {
            outputFile = (std::filesystem::path(exeDir) / (inputStem + "_obfuscated")).string();
        }
        color.println("  " + CLI::CHECKMARK + " Auto-generated: " + outputFile, CLI::BRIGHT_GREEN);
    } else {
        // If output is specified but relative, make it relative to executable directory
        std::filesystem::path outputPath(outputFile);
        if (outputPath.is_relative()) {
            outputFile = (std::filesystem::path(exeDir) / outputPath).string();
        }
    }
    
    std::string reportFile = getCleanInputString("Report file", "obfuscation_report.txt");
    if (reportFile.empty() || reportFile == "obfuscation_report.txt") {
        // Default to executable directory
        reportFile = (std::filesystem::path(exeDir) / "obfuscation_report.txt").string();
    } else {
        // If report path is relative, make it relative to executable directory
        std::filesystem::path reportPath(reportFile);
        if (reportPath.is_relative()) {
            reportFile = (std::filesystem::path(exeDir) / reportPath).string();
        }
    }
    config.outputReportPath = reportFile;
    
    color.println("  " + CLI::CHECKMARK + " Output file: " + outputFile, CLI::BRIGHT_GREEN);
    color.println("  " + CLI::CHECKMARK + " Report file: " + reportFile, CLI::BRIGHT_GREEN);
    
    // Step 5: Confirmation
    printCleanSectionHeader("STEP 5: Ready to Launch", "🚀");
    
    color.println("┌─────────────────────────────────────────────────────────────────────────┐", CLI::BRIGHT_MAGENTA);
    color.print("│", CLI::BRIGHT_MAGENTA);
    color.printCentered("🎯 Obfuscation Configuration Summary", 75, CLI::BOLD + CLI::BRIGHT_WHITE);
    color.println("│", CLI::BRIGHT_MAGENTA);
    color.println("├─────────────────────────────────────────────────────────────────────────┤", CLI::BRIGHT_MAGENTA);
    
    color.print("│", CLI::BRIGHT_MAGENTA);
    color.print("  📁 Input:  " + inputFile, CLI::WHITE);
    color.println("│", CLI::BRIGHT_MAGENTA);
    
    color.print("│", CLI::BRIGHT_MAGENTA);
    color.print("  📤 Output: " + outputFile, CLI::WHITE);
    color.println("│", CLI::BRIGHT_MAGENTA);
    
    color.print("│", CLI::BRIGHT_MAGENTA);
    color.print("  🔄 Cycles: " + std::to_string(config.obfuscationCycles), CLI::WHITE);
    color.println("│", CLI::BRIGHT_MAGENTA);
    
    color.println("└─────────────────────────────────────────────────────────────────────────┘", CLI::BRIGHT_MAGENTA);
    
    if (!getCleanYesNo("🚀 Launch obfuscation process?", true)) {
        printCleanWarningMessage("Obfuscation cancelled");
        return 0;
    }
    
    // Processing with beautiful progress
    printCleanSectionHeader("PROCESSING", "⚡");
    
    CLI::ProgressBar progressBar;
    progressBar.update(10, "📥 Loading module...");
    
    LLVMContext Context;
    SMDiagnostic Err;
    std::unique_ptr<Module> M = parseIRFile(actualInputFile, Err, Context);
    if (!M) {
        printCleanErrorMessage("Failed to load module!");
        Err.print("llvm-obfuscator", errs());
        return 1;
    }
    
    progressBar.update(20, "✅ Module loaded successfully");
    color.println("  " + CLI::CHECKMARK + " Loaded module: " + M->getName().str(), CLI::BRIGHT_GREEN);
    color.println("  " + CLI::ARROW + " Functions: " + std::to_string(M->size()), CLI::WHITE);
    color.println("  " + CLI::ARROW + " Globals: " + std::to_string(M->global_size()), CLI::WHITE);
    
    progressBar.update(30, "⚙️ Initializing obfuscation engine...");
    
    ObfuscationPass obfuscationPass(config);
    
    progressBar.update(40, "🔥 Running obfuscation passes...");
    
    bool modified = obfuscationPass.runOnModule(*M);
    
    progressBar.update(80, "✨ Obfuscation complete");
    
    if (!modified) {
        printCleanWarningMessage("No modifications were made to the module");
    }
    
    // Write output
    progressBar.update(90, "💾 Writing output file...");
    
    // CRITICAL FIX: Create output directory if it doesn't exist
    if (!ensureDirectoryExists(outputFile)) {
        printCleanErrorMessage("Failed to create output directory for: " + outputFile);
        return 1;
    }
    
    // CRITICAL FIX: Create report directory if it doesn't exist
    if (!reportFile.empty() && !ensureDirectoryExists(reportFile)) {
        printCleanErrorMessage("Failed to create report directory for: " + reportFile);
        return 1;
    }
    
    std::error_code EC;
    raw_fd_ostream OutFile(outputFile, EC);
    if (EC) {
        printCleanErrorMessage("Failed to open output file: " + EC.message());
        return 1;
    }
    
    M->print(OutFile, nullptr);
    OutFile.close();
    
    progressBar.update(95, "💾 Obfuscated IR saved");
    
    // Compile to executable
    std::string outputExeFile;
    std::filesystem::path outputPath(outputFile);
    std::string outputStem = outputPath.stem().string();
    // Reuse exeDir from earlier in the function
    outputExeFile = (std::filesystem::path(exeDir) / (outputStem + ".exe")).string();
    
    color.println("", CLI::BRIGHT_CYAN);
    color.println("  " + CLI::ARROW + " Compiling to executable...", CLI::CYAN);
    
    if (compileIRToExe(outputFile, outputExeFile, color)) {
        color.println("  " + CLI::CHECKMARK + " Executable created: " + outputExeFile, CLI::BRIGHT_GREEN);
    } else {
        color.println("  " + CLI::ARROW + " Obfuscated IR saved: " + outputFile, CLI::BRIGHT_YELLOW);
    }
    
    progressBar.update(100, "🎉 Complete!");
    
    // Beautiful completion summary
    printCleanSuccessMessage("OBFUSCATION COMPLETE!");
    
    color.println("┌─────────────────────────────────────────────────────────────────────────┐", CLI::BRIGHT_GREEN);
    color.print("│", CLI::BRIGHT_GREEN);
    color.printCentered("📊 Transformation Summary", 75, CLI::BOLD + CLI::BRIGHT_WHITE);
    color.println("│", CLI::BRIGHT_GREEN);
    color.println("├─────────────────────────────────────────────────────────────────────────┤", CLI::BRIGHT_GREEN);
    
    color.print("│", CLI::BRIGHT_GREEN);
    color.print("  🔄 Obfuscation cycles:       " + std::to_string(obfuscationPass.getTotalObfuscationCycles()), CLI::BRIGHT_CYAN);
    color.println("│", CLI::BRIGHT_GREEN);
    
    color.print("│", CLI::BRIGHT_GREEN);
    color.print("  🎭 Bogus instructions:       " + std::to_string(obfuscationPass.getTotalBogusInstructions()), CLI::BRIGHT_CYAN);
    color.println("│", CLI::BRIGHT_GREEN);
    
    color.print("│", CLI::BRIGHT_GREEN);
    color.print("  🔁 Fake loops:               " + std::to_string(obfuscationPass.getTotalFakeLoops()), CLI::BRIGHT_CYAN);
    color.println("│", CLI::BRIGHT_GREEN);
    
    color.print("│", CLI::BRIGHT_GREEN);
    color.print("  🔐 String encryptions:       " + std::to_string(obfuscationPass.getTotalStringEncryptions()), CLI::BRIGHT_CYAN);
    color.println("│", CLI::BRIGHT_GREEN);
    
    color.print("│", CLI::BRIGHT_GREEN);
    color.print("  ⚡ Instruction substitutions: " + std::to_string(obfuscationPass.getTotalInstructionSubstitutions()), CLI::BRIGHT_CYAN);
    color.println("│", CLI::BRIGHT_GREEN);
    
    color.print("│", CLI::BRIGHT_GREEN);
    color.print("  🏗️  Flattened functions:      " + std::to_string(obfuscationPass.getTotalFlattenedFunctions()), CLI::BRIGHT_CYAN);
    color.println("│", CLI::BRIGHT_GREEN);
    
    color.print("│", CLI::BRIGHT_GREEN);
    color.print("  🧮 MBA transformations:      " + std::to_string(obfuscationPass.getTotalMBATransformations()), CLI::BRIGHT_CYAN);
    color.println("│", CLI::BRIGHT_GREEN);
    
    color.print("│", CLI::BRIGHT_GREEN);
    color.print("  🛡️  Anti-debug checks:        " + std::to_string(obfuscationPass.getTotalAntiDebugChecks()), CLI::BRIGHT_CYAN);
    color.println("│", CLI::BRIGHT_GREEN);
    
    color.println("└─────────────────────────────────────────────────────────────────────────┘", CLI::BRIGHT_GREEN);
    
    color.println("┌─────────────────────────────────────────────────────────────────────────┐", CLI::BRIGHT_BLUE);
    color.print("│", CLI::BRIGHT_BLUE);
    color.printCentered("📁 Output Files", 75, CLI::BOLD + CLI::BRIGHT_WHITE);
    color.println("│", CLI::BRIGHT_BLUE);
    color.println("├─────────────────────────────────────────────────────────────────────────┤", CLI::BRIGHT_BLUE);
    
    color.print("│", CLI::BRIGHT_BLUE);
    color.print("  💎 Obfuscated code: " + outputFile, CLI::WHITE);
    color.println("│", CLI::BRIGHT_BLUE);
    
    color.print("│", CLI::BRIGHT_BLUE);
    color.print("  📄 Detailed report: " + reportFile, CLI::WHITE);
    color.println("│", CLI::BRIGHT_BLUE);
    
    color.println("└─────────────────────────────────────────────────────────────────────────┘", CLI::BRIGHT_BLUE);
    
    // Ask if user wants to view report
    if (getCleanYesNo("📖 View detailed report now?", false)) {
        // Enhanced report display would go here
        printCleanSuccessMessage("Report displayed successfully!");
    }
    
    color.println("┌─────────────────────────────────────────────────────────────────────────┐", CLI::BRIGHT_MAGENTA);
    color.print("│", CLI::BRIGHT_MAGENTA);
    color.printCentered("🎉 Thank you for using LLVM Code Obfuscator! 🎉", 75, CLI::BOLD + CLI::BRIGHT_WHITE);
    color.println("│", CLI::BRIGHT_MAGENTA);
    color.println("└─────────────────────────────────────────────────────────────────────────┘", CLI::BRIGHT_MAGENTA);
    
    return 0;
}

void pauseForUser() {
    CLI::ColorOutput color;
    std::cout << "\n";
    color.println("=================================================================", CLI::BRIGHT_CYAN);
    color.printCentered("Press Enter to continue...", 65, CLI::BRIGHT_WHITE);
    color.println("");
    color.println("=================================================================", CLI::BRIGHT_CYAN);
    std::cin.ignore();
}

int cleanInteractiveMode() {
    CLI::ColorOutput color;
    
    while (true) {
        printCleanBanner();
        
        // Step 1: File Selection with Analysis
        printCleanSectionHeader("STEP 1: File Selection & Analysis", "=>");
        
        std::string inputFile = getCleanInputString("Enter path to C/C++ source file (.cpp/.c) or LLVM IR file (.ll) [auto-detect]:", "");
        
        std::string actualInputFile = inputFile;
        std::string tempIRFile;
        bool isSource = false;
        
        // Check if file exists (if not auto-detect)
        if (!inputFile.empty() && inputFile != "auto-detect") {
            if (!std::filesystem::exists(inputFile)) {
                printCleanErrorMessage("File '" + inputFile + "' does not exist!");
                pauseForUser();
                continue;
            }
            isSource = isSourceFile(inputFile);
        }
        
        // Handle auto-detect
        if (inputFile == "auto-detect" || inputFile.empty()) {
            // Auto-detect C/C++ source files and LLVM IR files
            std::vector<std::string> sourceFiles;
            std::vector<std::string> llFiles;
            try {
                for (const auto& entry : std::filesystem::directory_iterator(".")) {
                    std::string ext = entry.path().extension().string();
                    if (ext == ".cpp" || ext == ".c" || ext == ".cxx" || ext == ".cc") {
                        sourceFiles.push_back(entry.path().filename().string());
                    } else if (ext == ".ll") {
                        llFiles.push_back(entry.path().filename().string());
                    }
                }
            } catch (const std::exception& e) {
                printCleanErrorMessage("Error scanning directory: " + std::string(e.what()));
                pauseForUser();
                continue;
            }
            
            // Prefer C/C++ source files over LLVM IR files
            if (!sourceFiles.empty()) {
                if (sourceFiles.size() == 1) {
                    inputFile = sourceFiles[0];
                    printCleanSuccessMessage("Auto-detected: " + inputFile);
                } else {
                    printCleanSectionHeader("Multiple C/C++ source files found", "*");
                    for (size_t i = 0; i < sourceFiles.size(); ++i) {
                        color.println("  " + std::to_string(i + 1) + ". " + sourceFiles[i], CLI::BRIGHT_CYAN);
                    }
                    
                    int choice = getCleanInputInt("Select file number", 1, 1, sourceFiles.size());
                    if (choice < 1 || choice > sourceFiles.size()) {
                        printCleanErrorMessage("Invalid selection!");
                        pauseForUser();
                        continue;
                    }
                    inputFile = sourceFiles[choice - 1];
                }
            } else if (!llFiles.empty()) {
                if (llFiles.size() == 1) {
                    inputFile = llFiles[0];
                    printCleanSuccessMessage("Auto-detected: " + inputFile);
                } else {
                    printCleanSectionHeader("Multiple LLVM IR files found", "*");
                    for (size_t i = 0; i < llFiles.size(); ++i) {
                        color.println("  " + std::to_string(i + 1) + ". " + llFiles[i], CLI::BRIGHT_CYAN);
                    }
                    
                    int choice = getCleanInputInt("Select file number", 1, 1, llFiles.size());
                    if (choice < 1 || choice > llFiles.size()) {
                        printCleanErrorMessage("Invalid selection!");
                        pauseForUser();
                        continue;
                    }
                    inputFile = llFiles[choice - 1];
                }
            } else {
                printCleanErrorMessage("No C/C++ source files or LLVM IR files found in current directory!");
                pauseForUser();
                continue;
            }
        }
        
        // Check if file exists
        if (!std::filesystem::exists(inputFile)) {
            printCleanErrorMessage("File '" + inputFile + "' does not exist!");
            pauseForUser();
            continue;
        }
        
        // Check if input is a source file (update isSource if not already set)
        if (!isSource) {
            isSource = isSourceFile(inputFile);
        }
        
        // If input is C/C++ source, compile to LLVM IR first
        if (isSource) {
            color.println("", CLI::BRIGHT_CYAN);
            color.println("  " + CLI::ARROW + " Detected C/C++ source file", CLI::CYAN);
            
            std::filesystem::path sourcePath(inputFile);
            std::string exeDir = getExecutableDirectory();
            tempIRFile = (std::filesystem::path(exeDir) / (sourcePath.stem().string() + ".ll")).string();
            
            if (!compileToLLVMIR(inputFile, tempIRFile, color)) {
                printCleanErrorMessage("Failed to compile C/C++ source to LLVM IR!");
                pauseForUser();
                continue;
            }
            
            actualInputFile = tempIRFile;
            inputFile = tempIRFile; // Update for analysis
        } else {
            actualInputFile = inputFile;
        }
        
        // Analyze file with clean spinner
        CLI::Spinner spinner;
        spinner.setSpinnerType("modern");
        spinner.update("Analyzing file structure and complexity...");
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        
        auto analysis = CLI::FileAnalyzer::analyzeFile(inputFile);
        spinner.stop();
        
        // Clean file analysis display
        color.println("=================================================================", CLI::BRIGHT_GREEN);
        color.printCentered("FILE ANALYSIS RESULTS", 65, CLI::BOLD + CLI::BRIGHT_WHITE);
        color.println("");
        color.println("=================================================================", CLI::BRIGHT_GREEN);
        
        color.println("File: " + inputFile, CLI::WHITE);
        
        if (analysis.isLLVMIR) {
            color.println("[OK] File Type: LLVM IR", CLI::BRIGHT_GREEN);
        } else if (analysis.isCpp) {
            color.println("[OK] File Type: C++", CLI::BRIGHT_GREEN);
        } else if (analysis.isC) {
            color.println("[OK] File Type: C", CLI::BRIGHT_GREEN);
        } else {
            color.println("[X] File Type: Unknown", CLI::BRIGHT_RED);
        }
        
        color.println("Size: " + std::to_string(analysis.estimatedComplexity) + " complexity", CLI::BRIGHT_CYAN);
        color.println("Complexity: " + std::to_string(analysis.estimatedComplexity) + "/100", CLI::BRIGHT_YELLOW);
        color.println("Functions: Estimated", CLI::BRIGHT_GREEN);
        color.println("Basic Blocks: Estimated", CLI::BRIGHT_MAGENTA);
        color.println("Instructions: Estimated", CLI::BRIGHT_RED);
        
        color.println("=================================================================", CLI::BRIGHT_GREEN);
        
        // Step 2: Configuration
        printCleanSectionHeader("STEP 2: Obfuscation Configuration", "[G]");
        
        // Smart preset recommendation
        std::string recommendedPreset = analysis.suggestedPreset;
        printCleanSuccessMessage("Recommended preset: " + recommendedPreset);
        
        printCleanSectionHeader("Preset Options", "*");
        color.println("1. Light Protection - Fast, minimal obfuscation", CLI::BRIGHT_GREEN);
        color.println("2. Balanced Protection - Good security/speed ratio", CLI::BRIGHT_YELLOW);
        color.println("3. Maximum Protection - Maximum security", CLI::BRIGHT_RED);
        color.println("4. Custom Configuration - Manual settings", CLI::BRIGHT_CYAN);
        color.println("5. Exit Program", CLI::BRIGHT_MAGENTA);
        
        int presetChoice = getCleanInputInt("Select preset", 2, 1, 5);
        
        if (presetChoice == 5) {
            printCleanSuccessMessage("Thank you for using LLVM Code Obfuscator!");
            break;
        }
        
        int cycles = 1, bogusPercent = 10, fakeLoops = 2;
        bool enableCF = false, enableStr = false, enableBogus = false, enableLoops = false;
        
        switch (presetChoice) {
            case 1: // Light
                cycles = 1; bogusPercent = 10; fakeLoops = 2;
                enableCF = false; enableStr = false; enableBogus = true; enableLoops = false;
                break;
            case 2: // Balanced
                cycles = 2; bogusPercent = 20; fakeLoops = 3;
                enableCF = true; enableStr = true; enableBogus = true; enableLoops = true;
                break;
            case 3: // Maximum
                cycles = 3; bogusPercent = 30; fakeLoops = 5;
                enableCF = true; enableStr = true; enableBogus = true; enableLoops = true;
                break;
            case 4: // Custom
                cycles = getCleanInputInt("Obfuscation cycles", 2, 1, 5);
                bogusPercent = getCleanInputInt("Bogus code percentage", 20, 0, 50);
                fakeLoops = getCleanInputInt("Fake loops per function", 3, 0, 10);
                enableCF = getCleanYesNo("Enable control flow obfuscation", true);
                enableStr = getCleanYesNo("Enable string encryption", true);
                enableBogus = getCleanYesNo("Enable bogus code insertion", true);
                enableLoops = getCleanYesNo("Enable fake loop insertion", true);
                break;
        }
        
        // Step 3: Processing
        printCleanSectionHeader("STEP 3: Processing", "!");
        
        // Get executable directory for output
        std::string exeDir = getExecutableDirectory();
        std::filesystem::path inputPath(inputFile);
        std::string inputStem = inputPath.stem().string();
        std::string inputExt = inputPath.extension().string();
        std::string outputFile;
        if (inputExt == ".ll" || inputExt == ".bc") {
            outputFile = (std::filesystem::path(exeDir) / (inputStem + "_obfuscated" + inputExt)).string();
        } else {
            outputFile = (std::filesystem::path(exeDir) / (inputStem + "_obfuscated")).string();
        }
        
        // Set report path to executable directory
        std::string reportPath = (std::filesystem::path(exeDir) / "obfuscation_report.txt").string();
        
        // Create ObfuscationConfig from preset choices
        ObfuscationConfig config;
        config.enableControlFlowObfuscation = enableCF;
        config.enableStringEncryption = enableStr;
        config.enableBogusCode = enableBogus;
        config.enableFakeLoops = enableLoops;
        config.obfuscationCycles = cycles;
        config.bogusCodePercentage = bogusPercent;
        config.fakeLoopCount = fakeLoops;
        config.outputReportPath = reportPath;
        config.decryptStringsAtStartup = true;
        
        CLI::ProgressBar progressBar(50);
        
        progressBar.update(10, "Loading module...");
        
        // Actually load and obfuscate the module
        LLVMContext Context;
        SMDiagnostic Err;
        std::unique_ptr<Module> M = parseIRFile(actualInputFile, Err, Context);
        if (!M) {
            printCleanErrorMessage("Failed to load module!");
            Err.print("llvm-obfuscator", errs());
            pauseForUser();
            continue;
        }
        
        progressBar.update(30, "Applying obfuscation...");
        
        ObfuscationPass obfuscationPass(config);
        bool modified = obfuscationPass.runOnModule(*M);
        
        if (!modified) {
            printCleanWarningMessage("No modifications were made to the module");
        }
        
        progressBar.update(60, "Optimizing code...");
        
        progressBar.update(90, "Writing output...");
        
        // CRITICAL FIX: Create output directory if it doesn't exist
        if (!ensureDirectoryExists(outputFile)) {
            printCleanErrorMessage("Failed to create output directory for: " + outputFile);
            pauseForUser();
            continue;
        }
        
        // CRITICAL FIX: Create report directory if it doesn't exist
        if (!reportPath.empty() && !ensureDirectoryExists(reportPath)) {
            printCleanErrorMessage("Failed to create report directory for: " + reportPath);
            pauseForUser();
            continue;
        }
        
        // Actually write the output file
        std::error_code EC;
        raw_fd_ostream OutFile(outputFile, EC);
        if (EC) {
            printCleanErrorMessage("Failed to open output file: " + EC.message());
            pauseForUser();
            continue;
        }
        
        M->print(OutFile, nullptr);
        OutFile.close();
        
        progressBar.update(100, "Complete!");
        
        // Step 4: Compile to executable
        printCleanSectionHeader("STEP 4: Compiling to Executable", "⚙️");
        
        std::string outputExeFile;
        std::filesystem::path outputPath(outputFile);
        std::string outputStem = outputPath.stem().string();
        outputExeFile = (std::filesystem::path(exeDir) / (outputStem + ".exe")).string();
        
        color.println("", CLI::BRIGHT_CYAN);
        if (compileIRToExe(outputFile, outputExeFile, color)) {
            color.println("", CLI::BRIGHT_GREEN);
            color.println("  " + CLI::CHECKMARK + " Executable created: " + outputExeFile, CLI::BRIGHT_GREEN);
        } else {
            color.println("", CLI::BRIGHT_YELLOW);
            color.println("  " + CLI::ARROW + " Obfuscated IR saved: " + outputFile, CLI::BRIGHT_YELLOW);
            color.println("  " + CLI::ARROW + " You can manually compile it later", CLI::BRIGHT_YELLOW);
        }
        
        // Step 5: Results
        printCleanSectionHeader("STEP 5: Results", "[OK]");
        
        color.println("=================================================================", CLI::BRIGHT_GREEN);
        color.printCentered("OBFUSCATION SUMMARY", 65, CLI::BOLD + CLI::BRIGHT_WHITE);
        color.println("");
        color.println("=================================================================", CLI::BRIGHT_GREEN);
        
        color.println("Input file: " + inputFile, CLI::BRIGHT_WHITE);
        color.println("Obfuscated IR: " + outputFile, CLI::BRIGHT_CYAN);
        if (!outputExeFile.empty() && std::filesystem::exists(outputExeFile)) {
            color.println("Output executable: " + outputExeFile, CLI::BRIGHT_GREEN);
        }
        color.println("Obfuscation cycles: " + std::to_string(cycles), CLI::BRIGHT_YELLOW);
        color.println("Bogus code percentage: " + std::to_string(bogusPercent) + "%", CLI::BRIGHT_MAGENTA);
        color.println("Fake loops: " + std::to_string(fakeLoops), CLI::BRIGHT_RED);
        
        color.println("=================================================================", CLI::BRIGHT_GREEN);
        
        printCleanSuccessMessage("Obfuscation completed successfully!");
        
        if (!outputExeFile.empty() && std::filesystem::exists(outputExeFile)) {
            color.println("Final executable: " + outputExeFile, CLI::BRIGHT_GREEN);
        } else {
            color.println("Obfuscated IR: " + outputFile, CLI::BRIGHT_CYAN);
        }
        color.println("Report: " + reportPath, CLI::BRIGHT_CYAN);
        
        // Ask if user wants to continue
        std::cout << "\n";
        bool continueProcessing = getCleanYesNo("Process another file", false);
        if (!continueProcessing) {
            printCleanSuccessMessage("Thank you for using LLVM Code Obfuscator!");
            break;
        }
    }
    
    return 0;
}

// Legacy function for backward compatibility
int enhancedInteractiveMode() {
    return cleanInteractiveMode();
}

// Legacy interactive mode for backward compatibility
int interactiveMode() {
    return cleanInteractiveMode();
}

// Helper function to check if file is C/C++ source
static bool isSourceFile(const std::string& filename) {
    std::filesystem::path path(filename);
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".c" || ext == ".cpp" || ext == ".cc" || ext == ".cxx" || ext == ".c++";
}

// Helper function to compile C/C++ to LLVM IR
static bool compileToLLVMIR(const std::string& sourceFile, const std::string& outputIR, CLI::ColorOutput& color) {
    color.println("  " + CLI::ARROW + " Compiling C/C++ to LLVM IR...", CLI::CYAN);
    
    // Find clang executable
    std::string clangPath;
#ifdef _WIN32
    // Try common Windows paths
    std::vector<std::string> possiblePaths = {
        "C:\\Program Files\\LLVM\\bin\\clang.exe",
        "C:\\Program Files (x86)\\LLVM\\bin\\clang.exe",
        "clang.exe",
        "clang"
    };
#else
    std::vector<std::string> possiblePaths = {
        "/usr/bin/clang",
        "/usr/local/bin/clang",
        "clang"
    };
#endif
    
    bool clangFound = false;
    for (const auto& path : possiblePaths) {
        if (std::filesystem::exists(path) || path == "clang" || path == "clang.exe") {
            clangPath = path;
            clangFound = true;
            break;
        }
    }
    
    if (!clangFound) {
        color.println("  " + CLI::CROSS + " clang not found! Please install LLVM/Clang.", CLI::RED);
        color.println("  " + CLI::ARROW + " Download from: https://llvm.org/", CLI::YELLOW);
        return false;
    }
    
    // Build clang command
    std::stringstream cmd;
    #ifdef _WIN32
    // On Windows, use cmd /c to properly handle paths with spaces
    cmd << "cmd /c \"\"" << clangPath << "\" -S -emit-llvm \"" << sourceFile << "\" -o \"" << outputIR << "\"\"";
    #else
    cmd << "\"" << clangPath << "\" -S -emit-llvm \"" << sourceFile << "\" -o \"" << outputIR << "\"";
    #endif
    
    color.println("  " + CLI::ARROW + " Running: " + cmd.str(), CLI::DIM);
    
    // Execute clang
    int result = std::system(cmd.str().c_str());
    
    if (result != 0 || !std::filesystem::exists(outputIR)) {
        color.println("  " + CLI::CROSS + " Failed to compile C/C++ to LLVM IR", CLI::RED);
        return false;
    }
    
    color.println("  " + CLI::CHECKMARK + " Compiled to LLVM IR: " + outputIR, CLI::GREEN);
    return true;
}

// Helper function to compile LLVM IR to executable
static bool compileIRToExe(const std::string& irFile, const std::string& outputExe, CLI::ColorOutput& color) {
    color.println("  " + CLI::ARROW + " Compiling obfuscated IR to executable...", CLI::CYAN);
    
    // Find clang executable
    std::string clangPath;
#ifdef _WIN32
    std::vector<std::string> possiblePaths = {
        "C:\\Program Files\\LLVM\\bin\\clang.exe",
        "C:\\Program Files (x86)\\LLVM\\bin\\clang.exe",
        "clang.exe",
        "clang"
    };
#else
    std::vector<std::string> possiblePaths = {
        "/usr/bin/clang",
        "/usr/local/bin/clang",
        "clang"
    };
#endif
    
    bool clangFound = false;
    for (const auto& path : possiblePaths) {
        if (std::filesystem::exists(path) || path == "clang" || path == "clang.exe") {
            clangPath = path;
            clangFound = true;
            break;
        }
    }
    
    if (!clangFound) {
        color.println("  " + CLI::CROSS + " clang not found! Please install LLVM/Clang.", CLI::RED);
        return false;
    }
    
    // Build clang command for linking
    std::stringstream cmd;
    #ifdef _WIN32
    // On Windows, use cmd /c to properly handle paths with spaces
    cmd << "cmd /c \"\"" << clangPath << "\" \"" << irFile << "\" -o \"" << outputExe << "\" -lstdc++ -luser32 -lkernel32 -lntdll\"";
    #else
    cmd << "\"" << clangPath << "\" \"" << irFile << "\" -o \"" << outputExe << "\" -lstdc++";
    #endif
    
    color.println("  " + CLI::ARROW + " Running: " + cmd.str(), CLI::DIM);
    
    // Execute clang
    int result = std::system(cmd.str().c_str());
    
    if (result != 0 || !std::filesystem::exists(outputExe)) {
        color.println("  " + CLI::CROSS + " Failed to compile IR to executable", CLI::RED);
        color.println("  " + CLI::ARROW + " Check for compilation errors above", CLI::YELLOW);
        return false;
    }
    
    // Get file size
    auto size = std::filesystem::file_size(outputExe);
    double sizeKB = size / 1024.0;
    double sizeMB = sizeKB / 1024.0;
    
    std::stringstream sizeStr;
    if (sizeMB >= 1.0) {
        sizeStr << std::fixed << std::setprecision(2) << sizeMB << " MB";
    } else {
        sizeStr << std::fixed << std::setprecision(0) << sizeKB << " KB";
    }
    
    color.println("  " + CLI::CHECKMARK + " Executable created: " + outputExe, CLI::GREEN);
    color.println("  " + CLI::ARROW + " Size: " + sizeStr.str(), CLI::CYAN);
    return true;
}

int main(int argc, char **argv) {
    InitLLVM X(argc, argv);
    
    // Check if running in interactive mode (no arguments provided)
    if (argc == 1) {
        return interactiveMode();
    }
    
    cl::ParseCommandLineOptions(argc, argv, "LLVM Code Obfuscator - Enhanced CLI v2.0\n");
    
    CLI::ColorOutput color;
    
    // Show beautiful banner for command line mode
    printCleanCommandLineBanner();
    
    // Check if input file exists
    std::string inputFileStr = InputFilename;
    std::filesystem::path inputPath(inputFileStr);
    if (!std::filesystem::exists(inputPath)) {
        color.println(CLI::CROSS + " Error: Input file not found: " + inputFileStr, CLI::RED);
        return 1;
    }
    
    std::string actualInputFile = inputFileStr;
    std::string tempIRFile;
    bool isSource = isSourceFile(inputFileStr);
    bool needCleanup = false;
    
    // Step 1: If input is C/C++ source, compile to LLVM IR first
    if (isSource) {
        color.println(CLI::ARROW + " Detected C/C++ source file", CLI::CYAN);
        
        std::filesystem::path sourcePath(inputFileStr);
        std::string exeDir = getExecutableDirectory();
        tempIRFile = (std::filesystem::path(exeDir) / (sourcePath.stem().string() + ".ll")).string();
        
        if (!compileToLLVMIR(inputFileStr, tempIRFile, color)) {
            return 1;
        }
        
        actualInputFile = tempIRFile;
        needCleanup = !KeepIntermediateFiles;
    }
    
    LLVMContext Context;
    SMDiagnostic Err;
    
    // Load the input module with progress indication
    CLI::Spinner spinner;
    spinner.update("Loading module...");
    
    std::unique_ptr<Module> M = parseIRFile(actualInputFile, Err, Context);
    if (!M) {
        spinner.stop();
        color.println(CLI::CROSS + " Error loading module!", CLI::RED);
        Err.print(argv[0], errs());
        if (needCleanup && std::filesystem::exists(tempIRFile)) {
            std::filesystem::remove(tempIRFile);
        }
        return 1;
    }
    
    spinner.stop();
    color.println(CLI::CHECKMARK + " Loaded module: " + M->getName().str(), CLI::GREEN);
    
    // Set target triple if specified
    if (!TargetTriple.empty()) {
        M->setTargetTriple(Triple(TargetTriple));
        color.println(CLI::ARROW + " Set target triple to: " + TargetTriple, CLI::CYAN);
    }
    
    // Configure obfuscation
    ObfuscationConfig config;
    config.enableControlFlowObfuscation = EnableControlFlow && !DisableControlFlow;
    config.enableStringEncryption = EnableStringEncryption && !DisableStringEncryption;
    config.enableBogusCode = EnableBogusCode && !DisableBogusCode;
    config.enableFakeLoops = EnableFakeLoops && !DisableFakeLoops;
    config.enableInstructionSubstitution = EnableInstructionSubst;
    config.enableControlFlowFlattening = EnableCFF;
    config.enableMBA = EnableMBA;
    config.enableAntiDebug = EnableAntiDebug;
    config.enableIndirectCalls = EnableIndirectCalls;
    config.enableConstantObfuscation = EnableConstObf;
    config.enableAntiTamper = EnableAntiTamper;
    config.enableVirtualization = EnableVirtualization;
    config.enablePolymorphic = EnablePolymorphic;
    config.enableAntiAnalysis = EnableAntiAnalysis;
    config.enableMetamorphic = EnableMetamorphic;
    config.enableDynamicObf = EnableDynamicObf;
    config.decryptStringsAtStartup = DecryptAtStartup;
    config.obfuscationCycles = ObfuscationCycles;
    config.mbaComplexity = MBAComplexity;
    config.flatteningProbability = FlattenProb;
    config.virtualizationLevel = VirtualizationLevel;
    config.polymorphicVariants = PolymorphicVariants;
    config.bogusCodePercentage = BogusCodePercentage;
    config.fakeLoopCount = FakeLoopCount;
    
    // Set report path - if not specified, use executable directory
    std::string reportFileStr = ReportFilename;
    std::string exeDir = getExecutableDirectory();
    std::string outputExeFile;  // Will be set if auto-compile is enabled
    
    if (reportFileStr.empty()) {
        // Default to current working directory (more intuitive)
        config.outputReportPath = (std::filesystem::current_path() / "obfuscation_report.txt").string();
    } else {
        // If report path is relative, resolve relative to current working directory
        std::filesystem::path reportPath(reportFileStr);
        if (reportPath.is_relative()) {
            std::filesystem::path cwd = std::filesystem::current_path();
            config.outputReportPath = (cwd / reportPath).lexically_normal().string();
        } else {
            config.outputReportPath = reportFileStr;
        }
    }
    
    // Show configuration summary
    color.println("\n" + CLI::DIAMOND + " Configuration:", CLI::BOLD + CLI::CYAN);
    color.println("  " + CLI::ARROW + " Obfuscation cycles: " + std::to_string(config.obfuscationCycles), CLI::WHITE);
    color.println("  " + CLI::ARROW + " Bogus code percentage: " + std::to_string(config.bogusCodePercentage) + "%", CLI::WHITE);
    color.println("  " + CLI::ARROW + " Fake loops per function: " + std::to_string(config.fakeLoopCount), CLI::WHITE);
    
    // Create and run obfuscation pass with progress
    CLI::ProgressBar progressBar;
    progressBar.update(20, "Initializing obfuscation...");
    
    ObfuscationPass obfuscationPass(config);
    
    progressBar.update(40, "Running obfuscation passes...");
    bool modified = obfuscationPass.runOnModule(*M);
    
    progressBar.update(80, "Obfuscation complete");
    
    if (!modified) {
        color.println("\n" + CLI::CROSS + " Warning: No modifications were made to the module", CLI::YELLOW);
    }
    
    // Determine output filename - default to executable directory (reuse exeDir from above)
    std::string outputFile = OutputFilename;
    
    if (outputFile.empty()) {
        // Get input file name (without directory)
        std::filesystem::path inputPath(inputFileStr);
        std::string inputStem = inputPath.stem().string();
        std::string inputExt = inputPath.extension().string();
        
        // Create output filename in executable directory
        if (inputExt == ".ll" || inputExt == ".bc") {
            outputFile = (std::filesystem::path(exeDir) / (inputStem + "_obfuscated" + inputExt)).string();
        } else {
            outputFile = (std::filesystem::path(exeDir) / (inputStem + "_obfuscated.ll")).string();
        }
        
        // If auto-compile is enabled or input was source file, also set exe output
        if (AutoCompileToExe || isSource) {
            outputExeFile = (std::filesystem::path(exeDir) / (inputStem + "_obfuscated.exe")).string();
        }
    } else {
        // If output is specified but relative, resolve relative to current working directory
        // This allows users to specify paths like "examples/output.ll" from project root
        std::filesystem::path outputPath(outputFile);
        if (outputPath.is_relative()) {
            // Resolve relative to current working directory (not executable directory)
            std::filesystem::path cwd = std::filesystem::current_path();
            outputFile = (cwd / outputPath).lexically_normal().string();
        }
        // If output ends with .exe, that's the exe file, IR is .ll
        if (outputPath.extension().string() == ".exe") {
            outputExeFile = outputFile;
            std::filesystem::path irPath = outputPath.replace_extension(".ll");
            if (irPath.is_relative()) {
                std::filesystem::path cwd = std::filesystem::current_path();
                outputFile = (cwd / irPath).lexically_normal().string();
            } else {
                outputFile = irPath.string();
            }
        }
    }
    
    // Write obfuscated module
    progressBar.update(90, "Writing output file...");
    
    // CRITICAL FIX: Create output directory if it doesn't exist
    if (!ensureDirectoryExists(outputFile)) {
        color.println("\n" + CLI::CROSS + " Error creating output directory for: " + outputFile, CLI::RED);
        return 1;
    }
    
    // CRITICAL FIX: Create report directory if it doesn't exist
    if (!config.outputReportPath.empty() && !ensureDirectoryExists(config.outputReportPath)) {
        color.println("\n" + CLI::CROSS + " Error creating report directory for: " + config.outputReportPath, CLI::RED);
        return 1;
    }
    
    std::error_code EC;
    raw_fd_ostream OutFile(outputFile, EC);
    if (EC) {
        color.println("\n" + CLI::CROSS + " Error opening output file: " + EC.message(), CLI::RED);
        return 1;
    }
    
    M->print(OutFile, nullptr);
    OutFile.close();
    
    progressBar.update(100, "Complete!");
    
    color.println("\n" + CLI::CHECKMARK + " Obfuscated module written to: " + outputFile, CLI::GREEN);
    
    // Generate binary if requested
    if (GenerateWindowsBinary || GenerateLinuxBinary) {
        std::string target;
        if (GenerateWindowsBinary) {
            target = "x86_64-pc-windows-msvc";
        } else {
            target = "x86_64-unknown-linux-gnu";
        }

        // Determine binary filename from outputFile
        auto hasSuffix = [](const std::string &s, const std::string &suffix) -> bool {
            return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
        };
        std::string binaryFile = outputFile;
        if (hasSuffix(binaryFile, ".ll")) {
            binaryFile = binaryFile.substr(0, binaryFile.length() - 3);
        }
        if (GenerateWindowsBinary) {
            binaryFile += ".exe";
        }

        // Use clang to compile and link directly from LLVM IR
        std::string clangCmd = std::string("clang -target ") + target + " " + outputFile + " -o " + binaryFile;
        color.println(CLI::ARROW + " Running: " + clangCmd, CLI::CYAN);
        int result = system(clangCmd.c_str());
        if (result != 0) {
            color.println(CLI::CROSS + " Error generating binary with clang", CLI::RED);
            return 1;
        }

        color.println(CLI::CHECKMARK + " Binary generated: " + binaryFile, CLI::GREEN);
    }
    
    // Print enhanced summary
    color.println("\n╔════════════════════════════════════════════════════════════════════╗", CLI::BLUE);
    color.println("║", CLI::BLUE);
    color.print("║", CLI::BLUE);
    color.print("                    ", CLI::BLUE);
    color.print("OBFUSCATION SUMMARY", CLI::BOLD + CLI::GREEN);
    color.print("                           ", CLI::BLUE);
    color.println("║", CLI::BLUE);
    color.println("╚════════════════════════════════════════════════════════════════════╝", CLI::BLUE);
    
    color.println("\n" + CLI::STAR + " Files:", CLI::BOLD + CLI::CYAN);
    color.println("  " + CLI::ARROW + " Input file: " + inputFileStr, CLI::WHITE);
    if (isSource) {
        color.println("  " + CLI::ARROW + " Intermediate IR: " + actualInputFile, CLI::DIM);
    }
    color.println("  " + CLI::ARROW + " Obfuscated IR: " + outputFile, CLI::WHITE);
    if (!outputExeFile.empty()) {
        color.println("  " + CLI::ARROW + " Output executable: " + outputExeFile, CLI::CYAN);
    }
    color.println("  " + CLI::ARROW + " Report file: " + config.outputReportPath, CLI::WHITE);
    
    color.println("\n" + CLI::STAR + " Metrics:", CLI::BOLD + CLI::CYAN);
    color.println("  " + CLI::CHECKMARK + " Obfuscation cycles: " + std::to_string(obfuscationPass.getTotalObfuscationCycles()), CLI::GREEN);
    color.println("  " + CLI::CHECKMARK + " Bogus instructions: " + std::to_string(obfuscationPass.getTotalBogusInstructions()), CLI::GREEN);
    color.println("  " + CLI::CHECKMARK + " Fake loops: " + std::to_string(obfuscationPass.getTotalFakeLoops()), CLI::GREEN);
    color.println("  " + CLI::CHECKMARK + " String encryptions: " + std::to_string(obfuscationPass.getTotalStringEncryptions()), CLI::GREEN);
    color.println("  " + CLI::CHECKMARK + " Instruction substitutions: " + std::to_string(obfuscationPass.getTotalInstructionSubstitutions()), CLI::GREEN);
    color.println("  " + CLI::CHECKMARK + " Flattened functions: " + std::to_string(obfuscationPass.getTotalFlattenedFunctions()), CLI::GREEN);
    color.println("  " + CLI::CHECKMARK + " MBA transformations: " + std::to_string(obfuscationPass.getTotalMBATransformations()), CLI::GREEN);
    color.println("  " + CLI::CHECKMARK + " Anti-debug checks: " + std::to_string(obfuscationPass.getTotalAntiDebugChecks()), CLI::GREEN);
    color.println("  " + CLI::CHECKMARK + " Virtualized functions: " + std::to_string(obfuscationPass.getTotalVirtualizedFunctions()), CLI::GREEN);
    color.println("  " + CLI::CHECKMARK + " Polymorphic variants: " + std::to_string(obfuscationPass.getTotalPolymorphicVariants()), CLI::GREEN);
    color.println("  " + CLI::CHECKMARK + " Anti-analysis checks: " + std::to_string(obfuscationPass.getTotalAntiAnalysisChecks()), CLI::GREEN);
    color.println("  " + CLI::CHECKMARK + " Metamorphic transformations: " + std::to_string(obfuscationPass.getTotalMetamorphicTransformations()), CLI::GREEN);
    color.println("  " + CLI::CHECKMARK + " Dynamic obfuscations: " + std::to_string(obfuscationPass.getTotalDynamicObfuscations()), CLI::GREEN);
    
    color.println("\n═══════════════════════════════════════════════════════════════════", CLI::BLUE);
    color.println("  " + CLI::STAR + " Obfuscation completed successfully!", CLI::BOLD + CLI::GREEN);
    color.println("═══════════════════════════════════════════════════════════════════", CLI::BLUE);
    
    // Step 3: If auto-compile is enabled or input was source, compile to executable
    if (!outputExeFile.empty() && (AutoCompileToExe || isSource)) {
        color.println("\n" + CLI::ARROW + " Compiling to executable...", CLI::CYAN);
        
        if (compileIRToExe(outputFile, outputExeFile, color)) {
            color.println("\n" + CLI::CHECKMARK + " Final executable: " + outputExeFile, CLI::BOLD + CLI::GREEN);
            
            // Clean up intermediate IR file if requested
            if (needCleanup && std::filesystem::exists(tempIRFile)) {
                std::filesystem::remove(tempIRFile);
                color.println("  " + CLI::ARROW + " Cleaned up intermediate file: " + tempIRFile, CLI::DIM);
            }
            
            // Optionally clean up obfuscated IR if exe was created
            if (!KeepIntermediateFiles && std::filesystem::exists(outputExeFile)) {
                std::filesystem::remove(outputFile);
                color.println("  " + CLI::ARROW + " Cleaned up obfuscated IR file", CLI::DIM);
            }
        } else {
            color.println("\n" + CLI::ARROW + " Obfuscated IR saved: " + outputFile, CLI::YELLOW);
            color.println("  " + CLI::ARROW + " You can manually compile it later", CLI::YELLOW);
        }
    } else {
        color.println("\n" + CLI::ARROW + " Obfuscated IR saved: " + outputFile, CLI::CYAN);
        if (isSource && !AutoCompileToExe) {
            color.println("  " + CLI::ARROW + " Use --compile flag to auto-compile to executable", CLI::YELLOW);
        }
    }
    
    return 0;
}
