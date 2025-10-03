using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using System.Windows.Input;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using LLVMObfuscatorAvalonia.Services;
using Microsoft.Extensions.DependencyInjection;

namespace LLVMObfuscatorAvalonia.ViewModels;

public partial class ConfigurationViewModel : ObservableObject
{
    private readonly ObfuscationService _obfuscationService;

    [ObservableProperty]
    private string _selectedFilePath = "";

    [ObservableProperty]
    private bool _isObfuscationRunning = false;

    [ObservableProperty]
    private string _outputFileName = "obfuscated_output.ll";

    [ObservableProperty]
    private string _reportFileName = "obfuscation_report.txt";

    public ObservableCollection<ObfuscationOption> ObfuscationOptions { get; } = new();
    public ObservableCollection<PresetOption> PresetOptions { get; } = new();

    public event EventHandler? ObfuscationStarted;
    public event EventHandler<bool>? ObfuscationCompleted;
    public event EventHandler<(int progress, string text, string details)>? ProgressUpdated;
    public event EventHandler<string>? ErrorOccurred;

    public ConfigurationViewModel()
    {
        // Fallback constructor for design-time
        _obfuscationService = Program.ServiceProvider?.GetService<ObfuscationService>() ?? new ObfuscationService();
        _obfuscationService.ProgressUpdated += (sender, progressData) => OnProgressUpdated(progressData);
        _obfuscationService.ObfuscationCompleted += (sender, success) => OnObfuscationCompleted(success);
        _obfuscationService.ErrorOccurred += (sender, error) => OnErrorOccurred(error);

        InitializeObfuscationOptions();
        InitializePresetOptions();
    }

    public ConfigurationViewModel(ObfuscationService obfuscationService)
    {
        _obfuscationService = obfuscationService ?? throw new ArgumentNullException(nameof(obfuscationService));
        _obfuscationService.ProgressUpdated += (sender, progressData) => OnProgressUpdated(progressData);
        _obfuscationService.ObfuscationCompleted += (sender, success) => OnObfuscationCompleted(success);
        _obfuscationService.ErrorOccurred += (sender, error) => OnErrorOccurred(error);

        InitializeObfuscationOptions();
        InitializePresetOptions();
    }

    [RelayCommand]
    private async Task StartObfuscation()
    {
        if (string.IsNullOrEmpty(SelectedFilePath))
        {
            OnErrorOccurred("Please select an input file first");
            return;
        }

        if (_obfuscationService.IsRunning)
        {
            OnErrorOccurred("Obfuscation is already running");
            return;
        }

        try
        {
            IsObfuscationRunning = true;
            OnProgressUpdated((10, "Preparing obfuscation...", "Setting up obfuscation parameters"));
            ObfuscationStarted?.Invoke(this, EventArgs.Empty);

            var options = CreateObfuscationOptions();
            var result = await _obfuscationService.RunObfuscationAsync(options);

            if (result.Success)
            {
                OnProgressUpdated((100, "Obfuscation completed successfully!", $"Output: {result.OutputFile}"));
            }
            else
            {
                OnErrorOccurred($"Obfuscation failed: {result.ErrorMessage}");
            }
        }
        catch (Exception ex)
        {
            OnErrorOccurred($"Error starting obfuscation: {ex.Message}");
        }
        finally
        {
            IsObfuscationRunning = false;
        }
    }

    [RelayCommand]
    private void CancelObfuscation()
    {
        if (_obfuscationService.IsRunning)
        {
            _obfuscationService.CancelObfuscation();
            OnProgressUpdated((0, "Obfuscation cancelled", "Process terminated by user"));
        }
        IsObfuscationRunning = false;
    }

    [RelayCommand]
    private void ApplyPreset(PresetOption preset)
    {
        if (preset == null) return;

        // Enable/disable options based on the preset
        foreach (var option in ObfuscationOptions)
        {
            option.IsEnabled = preset.Options.Contains(option.Name);
        }

        OnProgressUpdated((5, $"Applied preset: {preset.Name}", $"Configured {preset.Name} obfuscation settings"));
    }

    private ObfuscationOptions CreateObfuscationOptions()
    {
        var options = new ObfuscationOptions
        {
            InputFile = SelectedFilePath,
            OutputFile = OutputFileName,
            ReportFile = ReportFileName,
            ControlFlow = GetOptionValue("Control Flow Obfuscation"),
            StringEncryption = GetOptionValue("String Encryption"),
            BogusCode = GetOptionValue("Bogus Code Insertion"),
            FakeLoops = GetOptionValue("Fake Loop Insertion"),
            InstructionSubstitution = GetOptionValue("Instruction Substitution"),
            ControlFlowFlattening = GetOptionValue("Control Flow Flattening"),
            MixedBooleanArithmetic = GetOptionValue("Mixed Boolean Arithmetic"),
            AntiDebug = GetOptionValue("Anti-Debugging"),
            IndirectCalls = GetOptionValue("Indirect Function Calls"),
            ConstantObfuscation = GetOptionValue("Constant Obfuscation"),
            AntiTamper = GetOptionValue("Anti-Tampering"),
            Virtualization = GetOptionValue("Code Virtualization"),
            Polymorphic = GetOptionValue("Polymorphic Code"),
            AntiAnalysis = GetOptionValue("Anti-Analysis"),
            Metamorphic = GetOptionValue("Metamorphic Code"),
            DynamicObfuscation = GetOptionValue("Dynamic Obfuscation"),
            Verbose = true
        };

        // Apply the selected preset to get the appropriate advanced settings
        // Check if user has enabled many advanced options (suggesting aggressive mode)
        var enabledAdvancedOptions = ObfuscationOptions.Count(o => o.IsEnabled &&
            (o.Name.Contains("Virtualization") || o.Name.Contains("Polymorphic") ||
             o.Name.Contains("Metamorphic") || o.Name.Contains("Dynamic")));

        if (enabledAdvancedOptions >= 2)
        {
            // User has enabled multiple advanced options, use aggressive settings
            options.ApplyPreset("Aggressive");
        }
        else
        {
            options.ApplyPreset("Standard");
        }

        return options;
    }

    private bool GetOptionValue(string optionName)
    {
        var option = ObfuscationOptions.FirstOrDefault(o => o.Name == optionName);
        return option?.IsEnabled ?? false;
    }

    private void OnProgressUpdated((int progress, string text, string details) progressData)
    {
        ProgressUpdated?.Invoke(this, progressData);
    }

    private void OnObfuscationCompleted(bool success)
    {
        ObfuscationCompleted?.Invoke(this, success);
    }

    private void OnErrorOccurred(string error)
    {
        ErrorOccurred?.Invoke(this, error);
    }

    private void InitializeObfuscationOptions()
    {
        ObfuscationOptions.Clear();
        ObfuscationOptions.Add(new ObfuscationOption("Control Flow Obfuscation", true));
        ObfuscationOptions.Add(new ObfuscationOption("String Encryption", true));
        ObfuscationOptions.Add(new ObfuscationOption("Bogus Code Insertion", false));
        ObfuscationOptions.Add(new ObfuscationOption("Fake Loop Insertion", false));
        ObfuscationOptions.Add(new ObfuscationOption("Instruction Substitution", false));
        ObfuscationOptions.Add(new ObfuscationOption("Control Flow Flattening", false));
        ObfuscationOptions.Add(new ObfuscationOption("Mixed Boolean Arithmetic", false));
        ObfuscationOptions.Add(new ObfuscationOption("Anti-Debugging", false));
        ObfuscationOptions.Add(new ObfuscationOption("Indirect Function Calls", false));
        ObfuscationOptions.Add(new ObfuscationOption("Constant Obfuscation", false));
        ObfuscationOptions.Add(new ObfuscationOption("Anti-Tampering", false));
        ObfuscationOptions.Add(new ObfuscationOption("Code Virtualization", false));
        ObfuscationOptions.Add(new ObfuscationOption("Polymorphic Code", false));
        ObfuscationOptions.Add(new ObfuscationOption("Anti-Analysis", false));
        ObfuscationOptions.Add(new ObfuscationOption("Metamorphic Code", false));
        ObfuscationOptions.Add(new ObfuscationOption("Dynamic Obfuscation", false));
    }

    private void InitializePresetOptions()
    {
        PresetOptions.Clear();
        PresetOptions.Add(new PresetOption("Basic", "Light protection\nMinimal overhead", 
            new[] { "Control Flow Obfuscation", "String Encryption" }));
        PresetOptions.Add(new PresetOption("Standard", "Balanced security\nRecommended", 
            new[] { "Control Flow Obfuscation", "String Encryption", "Bogus Code Insertion", "Fake Loop Insertion" }));
        PresetOptions.Add(new PresetOption("Aggressive", "Maximum security\nHigher overhead", 
            ObfuscationOptions.Select(o => o.Name).ToArray()));
        PresetOptions.Add(new PresetOption("Custom", "Manual selection\nFull control", 
            Array.Empty<string>()));
    }
}
