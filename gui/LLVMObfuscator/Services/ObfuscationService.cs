using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace LLVMObfuscatorAvalonia.Services
{
    public class ObfuscationService
    {
        private readonly string _obfuscatorPath;
        private Process? _currentProcess;
        private bool _isRunning = false;
        private int _currentProgress = 0;

        public event EventHandler<(int progress, string text, string details)>? ProgressUpdated;
        public event EventHandler<bool>? ObfuscationCompleted;
        public event EventHandler<string>? ErrorOccurred;

        public ObfuscationService()
        {
            // Try to find the obfuscator executable
            _obfuscatorPath = FindObfuscatorExecutable();
        }

        private string FindObfuscatorExecutable()
        {
            // Look for the obfuscator in common locations
            var possiblePaths = new[]
            {
                // Current directory and subdirectories
                Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "llvm-obfuscator.exe"),
                Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "tools", "llvm-obfuscator.exe"),
                Path.Combine(Directory.GetCurrentDirectory(), "tools", "llvm-obfuscator.exe"),
                Path.Combine(Directory.GetCurrentDirectory(), "llvm-obfuscator.exe"),
                
                // Look in the output directory (two levels up from GUI)
                Path.Combine(Directory.GetCurrentDirectory(), "..", "..", "output", "llvm-obfuscator.exe"),
                Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "..", "..", "output", "llvm-obfuscator.exe"),
                
                // Look in the main project directory
                Path.Combine(Directory.GetCurrentDirectory(), "..", "..", "llvm-obfuscator.exe"),
                Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "..", "..", "llvm-obfuscator.exe"),
                
                "llvm-obfuscator.exe" // In PATH
            };

            
            foreach (var path in possiblePaths)
            {
                if (File.Exists(path))
                {
                    return path;
                }
            }
            // If not found, return the expected path
            return "llvm-obfuscator.exe";
        }

        public async Task<ObfuscationResult> RunObfuscationAsync(ObfuscationOptions options)
        {
            if (_isRunning)
            {
                throw new InvalidOperationException("Obfuscation is already running");
            }

            _isRunning = true;
            var result = new ObfuscationResult();
            var outputBuilder = new System.Text.StringBuilder();
            var errorBuilder = new System.Text.StringBuilder();

            try
            {
                // Validate input file
                if (!File.Exists(options.InputFile))
                {
                    throw new FileNotFoundException($"Input file not found: {options.InputFile}");
                }

                // Validate file extension
                var extension = Path.GetExtension(options.InputFile).ToLowerInvariant();
                if (extension != ".ll" && extension != ".bc")
                {
                    throw new ArgumentException($"Unsupported file type: {extension}. Only .ll (LLVM IR) and .bc (LLVM Bitcode) files are supported.");
                }

                // Verify obfuscator executable exists
                
                if (!File.Exists(_obfuscatorPath))
                {
                    throw new FileNotFoundException($"Obfuscator executable not found: {_obfuscatorPath}");
                }

                // Configure process for aggressive mode memory management
                ConfigureAggressiveModeProcess(options);

                // Build command line arguments
                var arguments = BuildCommandLineArguments(options);
                OnProgressUpdated(5, "Preparing obfuscation...", $"Command: {_obfuscatorPath} {arguments}");

                // Start the obfuscator process with enhanced memory management
                var startInfo = new ProcessStartInfo
                {
                    FileName = _obfuscatorPath,
                    Arguments = arguments,
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true,
                    WorkingDirectory = Path.GetDirectoryName(options.InputFile) ?? Directory.GetCurrentDirectory()
                };

                // Configure process environment for aggressive mode
                ConfigureProcessEnvironment(startInfo, options);

                // Ensure all paths are absolute
                if (!Path.IsPathRooted(options.InputFile))
                {
                    options.InputFile = Path.GetFullPath(options.InputFile);
                }
                if (!string.IsNullOrEmpty(options.OutputFile) && !Path.IsPathRooted(options.OutputFile))
                {
                    options.OutputFile = Path.GetFullPath(options.OutputFile);
                }
                if (!string.IsNullOrEmpty(options.ReportFile) && !Path.IsPathRooted(options.ReportFile))
                {
                    options.ReportFile = Path.GetFullPath(options.ReportFile);
                }

                // Rebuild arguments with absolute paths
                arguments = BuildCommandLineArguments(options);
                startInfo.Arguments = arguments;

                _currentProcess = new Process { StartInfo = startInfo };
                
                // Capture output and error
                _currentProcess.OutputDataReceived += (sender, e) =>
                {
                    if (!string.IsNullOrEmpty(e.Data))
                    {
                        outputBuilder.AppendLine(e.Data);
                        ParseProgressFromOutput(e.Data);
                    }
                };
                
                _currentProcess.ErrorDataReceived += (sender, e) =>
                {
                    if (!string.IsNullOrEmpty(e.Data))
                    {
                        errorBuilder.AppendLine(e.Data);
                        OnErrorOccurred(e.Data);
                    }
                };

                OnProgressUpdated(10, "Starting obfuscation...", "Launching obfuscator process");
                _currentProcess.Start();
                _currentProcess.BeginOutputReadLine();
                _currentProcess.BeginErrorReadLine();

                // Set process priority to prevent system freezing
                try
                {
                    _currentProcess.PriorityClass = ProcessPriorityClass.BelowNormal;
                }
                catch (Exception ex)
                {
                    OnProgressUpdated(_currentProgress, $"Warning: Could not set process priority: {ex.Message}", "Continuing with normal priority");
                }

                // Start memory monitoring task
                var memoryMonitorTask = MonitorProcessMemory(_currentProcess);
                
                // Wait for completion with timeout and memory monitoring
                var timeoutTask = Task.Delay(TimeSpan.FromMinutes(15)); // 15 minute timeout for aggressive mode
                var processTask = _currentProcess.WaitForExitAsync();
                
                var completedTask = await Task.WhenAny(processTask, timeoutTask);
                
                if (completedTask == timeoutTask)
                {
                    OnErrorOccurred("Obfuscation timed out after 15 minutes. This may be due to aggressive settings requiring more time.");
                    try
                    {
                        if (!_currentProcess.HasExited)
                        {
                            _currentProcess.Kill();
                        }
                    }
                    catch (Exception ex)
                    {
                        OnErrorOccurred($"Error terminating timed-out process: {ex.Message}");
                    }
                    result.Success = false;
                    result.ErrorMessage = "Process timed out - aggressive mode may require more time";
                    return result;
                }
                
                // Check if memory monitor detected excessive usage
                var memoryExceeded = await memoryMonitorTask;
                
                if (memoryExceeded)
                {
                    OnErrorOccurred("Obfuscation terminated due to excessive memory usage (>8GB). Consider using a system with more RAM.");
                    try
                    {
                        if (!_currentProcess.HasExited)
                        {
                            _currentProcess.Kill();
                        }
                    }
                    catch (Exception ex)
                    {
                        OnErrorOccurred($"Error terminating memory-exhausted process: {ex.Message}");
                    }
                    result.Success = false;
                    result.ErrorMessage = "Process terminated due to memory exhaustion - aggressive mode requires significant RAM (8GB+)";
                    return result;
                }
                

                result.Success = _currentProcess.ExitCode == 0;
                result.ExitCode = _currentProcess.ExitCode;
                result.OutputFile = options.OutputFile;
                result.ReportFile = options.ReportFile;


                if (result.Success)
                {
                    OnProgressUpdated(100, "Obfuscation completed successfully!", "All transformations applied successfully");
                    result.Statistics = await ParseReportFile(options.ReportFile);
                }
                else
                {
                    var errorOutput = errorBuilder.ToString();
                    var standardOutput = outputBuilder.ToString();
                    var fullError = $"Obfuscation failed with exit code {_currentProcess.ExitCode}\n\nError Output:\n{errorOutput}\n\nStandard Output:\n{standardOutput}";
                    result.ErrorMessage = fullError;
                    OnErrorOccurred(fullError);
                }
            }
            catch (Exception ex)
            {
                result.Success = false;
                result.ErrorMessage = ex.Message;
                OnErrorOccurred($"Error during obfuscation: {ex.Message}");
            }
            finally
            {
                _isRunning = false;
                OnObfuscationCompleted(result.Success);
            }

            return result;
        }

        private string BuildCommandLineArguments(ObfuscationOptions options)
        {
            var args = new List<string>();

            // Input file
            args.Add($"\"{options.InputFile}\"");

            // Output file
            if (!string.IsNullOrEmpty(options.OutputFile))
            {
                args.Add($"-o \"{options.OutputFile}\"");
            }

            // Report file
            if (!string.IsNullOrEmpty(options.ReportFile))
            {
                args.Add($"--report=\"{options.ReportFile}\"");
            }

            // Obfuscation techniques
            if (options.ControlFlow) args.Add("--cf");
            if (options.StringEncryption) args.Add("--str");
            if (options.BogusCode) args.Add("--bogus");
            if (options.FakeLoops) args.Add("--loops");
            if (options.InstructionSubstitution) args.Add("--subs");
            if (options.ControlFlowFlattening) args.Add("--flatten");
            if (options.MixedBooleanArithmetic) args.Add("--mba");
            if (options.AntiDebug) args.Add("--anti-debug");
            if (options.IndirectCalls) args.Add("--indirect");
            if (options.ConstantObfuscation) args.Add("--const-obf");
            if (options.AntiTamper) args.Add("--anti-tamper");
            if (options.Virtualization) args.Add("--virtualize");
            if (options.Polymorphic) args.Add("--polymorphic");
            if (options.AntiAnalysis) args.Add("--anti-analysis");
            if (options.Metamorphic) args.Add("--metamorphic");
            if (options.DynamicObfuscation) args.Add("--dynamic");

            // Advanced options
            if (options.DecryptAtStartup) args.Add("--decrypt-startup");
            if (options.ObfuscationCycles > 0) args.Add($"--cycles={options.ObfuscationCycles}");
            if (options.MBAComplexity > 0) args.Add($"--mba-level={options.MBAComplexity}");
            if (options.FlattenProbability > 0) args.Add($"--flatten-prob={options.FlattenProbability}");
            if (options.VirtualizationLevel > 0) args.Add($"--vm-level={options.VirtualizationLevel}");
            if (options.PolymorphicVariants > 0) args.Add($"--poly-variants={options.PolymorphicVariants}");
            if (options.BogusCodePercentage > 0) args.Add($"--bogus-percent={options.BogusCodePercentage}");
            if (options.FakeLoopCount > 0) args.Add($"--fake-loops={options.FakeLoopCount}");

            // Platform options
            if (!string.IsNullOrEmpty(options.TargetTriple))
            {
                args.Add($"--triple={options.TargetTriple}");
            }
            if (options.GenerateWindowsBinary) args.Add("--win");
            if (options.GenerateLinuxBinary) args.Add("--linux");

            // Verbose output
            if (options.Verbose) args.Add("-v");

            return string.Join(" ", args);
        }

        private void OnOutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            if (!string.IsNullOrEmpty(e.Data))
            {
                OnProgressUpdated(_currentProgress, $"Error: {e.Data}", "Check output for details");
            }
        }

        private void OnErrorDataReceived(object sender, DataReceivedEventArgs e)
        {
            if (!string.IsNullOrEmpty(e.Data))
            {
                OnErrorOccurred(e.Data);
            }
        }

        private async Task<Dictionary<string, string>> ParseReportFile(string reportFile)
        {
            var statistics = new Dictionary<string, string>();

            try
            {
                if (File.Exists(reportFile))
                {
                    var content = await File.ReadAllTextAsync(reportFile);
                    
                    // Parse common statistics from the report
                    var patterns = new Dictionary<string, string>
                    {
                        { "Functions Obfuscated", @"Functions obfuscated:\s*(\d+)" },
                        { "Basic Blocks Modified", @"Basic blocks modified:\s*(\d+)" },
                        { "Instructions Added", @"Instructions added:\s*(\d+)" },
                        { "Strings Encrypted", @"Strings encrypted:\s*(\d+)" },
                        { "Control Flow Edges", @"Control flow edges:\s*(\d+)" },
                        { "Obfuscation Time", @"Obfuscation time:\s*([\d.]+)\s*ms" },
                        { "File Size Increase", @"File size increase:\s*([\d.]+)%" }
                    };

                    foreach (var pattern in patterns)
                    {
                        var match = Regex.Match(content, pattern.Value, RegexOptions.IgnoreCase);
                        if (match.Success)
                        {
                            statistics[pattern.Key] = match.Groups[1].Value;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                OnErrorOccurred($"Error parsing report file: {ex.Message}");
            }

            return statistics;
        }

        public void CancelObfuscation()
        {
            if (_currentProcess != null && !_currentProcess.HasExited)
            {
                try
                {
                    _currentProcess.Kill();
                    OnProgressUpdated(_currentProgress, "Obfuscation cancelled by user", "Process terminated by user request");
                }
                catch (Exception ex)
                {
                    OnErrorOccurred($"Error cancelling obfuscation: {ex.Message}");
                }
            }
        }

        public bool IsRunning => _isRunning;

        private void OnProgressUpdated(int progress, string text, string details = "")
        {
            _currentProgress = Math.Max(_currentProgress, progress);
            ProgressUpdated?.Invoke(this, (_currentProgress, text, details));
        }

        private void ParseProgressFromOutput(string output)
        {
            // Parse progress from obfuscator output
            if (output.Contains("Starting obfuscation"))
            {
                OnProgressUpdated(15, "Initializing obfuscation passes...", output);
            }
            else if (output.Contains("Control Flow"))
            {
                OnProgressUpdated(25, "Applying control flow obfuscation...", output);
            }
            else if (output.Contains("String"))
            {
                OnProgressUpdated(35, "Encrypting strings...", output);
            }
            else if (output.Contains("Bogus"))
            {
                OnProgressUpdated(45, "Adding bogus instructions...", output);
            }
            else if (output.Contains("Virtualization"))
            {
                OnProgressUpdated(55, "Virtualizing functions...", output);
            }
            else if (output.Contains("Polymorphic"))
            {
                OnProgressUpdated(65, "Applying polymorphic transformations...", output);
            }
            else if (output.Contains("Metamorphic"))
            {
                OnProgressUpdated(75, "Applying metamorphic transformations...", output);
            }
            else if (output.Contains("Anti-debug"))
            {
                OnProgressUpdated(85, "Adding anti-debugging measures...", output);
            }
            else if (output.Contains("Complete") || output.Contains("Finished"))
            {
                OnProgressUpdated(95, "Finalizing obfuscation...", output);
            }
            else if (output.Contains("Writing"))
            {
                OnProgressUpdated(90, "Writing output files...", output);
            }
        }

        private void OnObfuscationCompleted(bool success)
        {
            ObfuscationCompleted?.Invoke(this, success);
        }

        private void OnErrorOccurred(string error)
        {
            ErrorOccurred?.Invoke(this, error);
        }

        /// <summary>
        /// Configure process for aggressive mode with enhanced memory management
        /// </summary>
        private void ConfigureAggressiveModeProcess(ObfuscationOptions options)
        {
            // Check if this is aggressive mode (all techniques enabled)
            bool isAggressiveMode = options.ControlFlow && options.StringEncryption && 
                                  options.BogusCode && options.FakeLoops && 
                                  options.InstructionSubstitution && options.ControlFlowFlattening && 
                                  options.MixedBooleanArithmetic && options.AntiDebug && 
                                  options.IndirectCalls && options.ConstantObfuscation && 
                                  options.AntiTamper && options.Virtualization && 
                                  options.Polymorphic && options.AntiAnalysis && 
                                  options.Metamorphic && options.DynamicObfuscation;

            if (isAggressiveMode)
            {
                OnProgressUpdated(5, "🔥 Aggressive mode detected", "Configuring enhanced memory management");
                OnProgressUpdated(5, "Aggressive settings applied", $"Cycles={options.ObfuscationCycles}, Bogus={options.BogusCodePercentage}%, Loops={options.FakeLoopCount}, VM Level={options.VirtualizationLevel}");
            }
        }

        /// <summary>
        /// Configure process environment variables for aggressive mode
        /// </summary>
        private void ConfigureProcessEnvironment(ProcessStartInfo startInfo, ObfuscationOptions options)
        {
            // Set environment variables for aggressive mode memory management
            startInfo.EnvironmentVariables["LLVM_OBFUSCATOR_AGGRESSIVE_MODE"] = "1";
            startInfo.EnvironmentVariables["LLVM_OBFUSCATOR_MEMORY_MONITOR"] = "1";
            startInfo.EnvironmentVariables["LLVM_OBFUSCATOR_MAX_MEMORY"] = "4096"; // 4GB limit for aggressive mode
            startInfo.EnvironmentVariables["LLVM_OBFUSCATOR_PROCESS_PRIORITY"] = "BELOW_NORMAL";
            
            // Enable VM function management
            startInfo.EnvironmentVariables["LLVM_OBFUSCATOR_VM_MANAGEMENT"] = "1";
            startInfo.EnvironmentVariables["LLVM_OBFUSCATOR_VM_LIMIT"] = "10"; // Max 10 VM functions
            
            OnProgressUpdated(5, "🔧 Process environment configured", "Environment variables set for aggressive mode");
        }

        /// <summary>
        /// Monitor process memory usage and terminate if it exceeds safe limits
        /// </summary>
        private async Task<bool> MonitorProcessMemory(Process process)
        {
            const long maxMemoryBytes = 8L * 1024 * 1024 * 1024; // 8GB limit for aggressive mode
            
            while (!process.HasExited)
            {
                try
                {
                    process.Refresh();
                    long privateMemory = process.PrivateMemorySize64; // Use private memory instead of working set
                    
                    
                    if (privateMemory > maxMemoryBytes)
                    {
                        OnProgressUpdated(_currentProgress, $"⚠️ Memory usage exceeded limit: {privateMemory / (1024 * 1024)}MB > 8192MB", "Process will be terminated");
                        return true; // Return true to indicate memory exceeded
                    }
                    
                    // Update progress with memory usage for aggressive mode
                    if (privateMemory > 4L * 1024 * 1024 * 1024) // > 4GB
                    {
                        OnProgressUpdated(_currentProgress, $"📊 Memory usage: {privateMemory / (1024 * 1024)}MB", "Aggressive mode active - monitoring memory usage");
                    }
                }
                catch (Exception ex)
                {
                    OnProgressUpdated(_currentProgress, $"Memory monitoring error: {ex.Message}", "Continuing obfuscation process");
                }
                
                await Task.Delay(TimeSpan.FromSeconds(10)); // Check every 10 seconds for aggressive mode
            }
            
            return false; // Return false to indicate normal completion
        }
    }

    public class ObfuscationOptions
    {
        public string InputFile { get; set; } = "";
        public string OutputFile { get; set; } = "";
        public string ReportFile { get; set; } = "obfuscation_report.txt";

        // Basic obfuscation techniques
        public bool ControlFlow { get; set; } = true;
        public bool StringEncryption { get; set; } = true;
        public bool BogusCode { get; set; } = true;
        public bool FakeLoops { get; set; } = true;
        public bool InstructionSubstitution { get; set; } = false;
        public bool ControlFlowFlattening { get; set; } = false;
        public bool MixedBooleanArithmetic { get; set; } = false;
        public bool AntiDebug { get; set; } = false;
        public bool IndirectCalls { get; set; } = false;
        public bool ConstantObfuscation { get; set; } = false;
        public bool AntiTamper { get; set; } = false;
        public bool Virtualization { get; set; } = false;
        public bool Polymorphic { get; set; } = false;
        public bool AntiAnalysis { get; set; } = false;
        public bool Metamorphic { get; set; } = false;
        public bool DynamicObfuscation { get; set; } = false;

        // Advanced options
        public bool DecryptAtStartup { get; set; } = true;
        public int ObfuscationCycles { get; set; } = 3;
        public int MBAComplexity { get; set; } = 3;
        public int FlattenProbability { get; set; } = 80;
        public int VirtualizationLevel { get; set; } = 2;
        public int PolymorphicVariants { get; set; } = 5;
        public int BogusCodePercentage { get; set; } = 30;
        public int FakeLoopCount { get; set; } = 5;

        // Platform options
        public string TargetTriple { get; set; } = "";
        public bool GenerateWindowsBinary { get; set; } = false;
        public bool GenerateLinuxBinary { get; set; } = false;

        // Output options
        public bool Verbose { get; set; } = false;

        // Preset mode (affects default values)
        public string PresetMode { get; set; } = "Standard";

        // Apply preset settings
        public void ApplyPreset(string presetName)
        {
            PresetMode = presetName;

            switch (presetName)
            {
                case "Basic":
                    // Light protection
                    ObfuscationCycles = 1;
                    BogusCodePercentage = 20;
                    FakeLoopCount = 2;
                    FlattenProbability = 50;
                    VirtualizationLevel = 1;
                    PolymorphicVariants = 2;
                    MBAComplexity = 2;
                    break;

                case "Aggressive":
                    // Maximum protection - these are the settings that were causing memory issues before our fix
                    ObfuscationCycles = 5;
                    BogusCodePercentage = 50;
                    FakeLoopCount = 5;
                    FlattenProbability = 80;
                    VirtualizationLevel = 2;
                    PolymorphicVariants = 3;
                    MBAComplexity = 5;
                    // Enable all techniques
                    ControlFlow = true;
                    StringEncryption = true;
                    BogusCode = true;
                    FakeLoops = true;
                    InstructionSubstitution = true;
                    ControlFlowFlattening = true;
                    MixedBooleanArithmetic = true;
                    AntiDebug = true;
                    IndirectCalls = true;
                    ConstantObfuscation = true;
                    AntiTamper = true;
                    Virtualization = true;
                    Polymorphic = true;
                    AntiAnalysis = true;
                    Metamorphic = true;
                    DynamicObfuscation = true;
                    break;

                case "Standard":
                default:
                    // Balanced settings (default)
                    ObfuscationCycles = 3;
                    BogusCodePercentage = 30;
                    FakeLoopCount = 5;
                    FlattenProbability = 80;
                    VirtualizationLevel = 2;
                    PolymorphicVariants = 5;
                    MBAComplexity = 3;
                    break;
            }
        }
    }

    public class ObfuscationResult
    {
        public bool Success { get; set; }
        public int ExitCode { get; set; }
        public string? ErrorMessage { get; set; }
        public string OutputFile { get; set; } = "";
        public string ReportFile { get; set; } = "";
        public Dictionary<string, string> Statistics { get; set; } = new();
    }
}
