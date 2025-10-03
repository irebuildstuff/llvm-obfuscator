using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using System.Windows.Input;
using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Controls;
using Avalonia.Platform.Storage;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using LLVMObfuscatorAvalonia.Services;
using Microsoft.Extensions.DependencyInjection;

namespace LLVMObfuscatorAvalonia.ViewModels;

public partial class FileSelectionViewModel : ObservableObject
{
    private readonly IDialogService? _dialogService;

    [ObservableProperty]
    private string _selectedFilePath = "";

    [ObservableProperty]
    private string _fileStatusText = "No file selected";

    [ObservableProperty]
    private string _fileInfoText = "Supported: .ll (LLVM IR), .bc (LLVM Bitcode) files • Max size: 50MB";

    [ObservableProperty]
    private bool _isFileSelected = false;

    [ObservableProperty]
    private string _outputFileName = "obfuscated_output.ll";

    [ObservableProperty]
    private string _reportFileName = "obfuscation_report.txt";

    public ObservableCollection<ObfuscationOption> ObfuscationOptions { get; } = new();
    public ObservableCollection<PresetOption> PresetOptions { get; } = new();

    public event EventHandler<string>? FileSelected;

    public FileSelectionViewModel()
    {
        // Fallback constructor for design-time
        _dialogService = Program.ServiceProvider?.GetService<IDialogService>();
        InitializeObfuscationOptions();
        InitializePresetOptions();
    }

    public FileSelectionViewModel(IDialogService dialogService)
    {
        _dialogService = dialogService ?? throw new ArgumentNullException(nameof(dialogService));
        InitializeObfuscationOptions();
        InitializePresetOptions();
    }

    [RelayCommand]
    private async Task BrowseFiles()
    {
        try
        {
            string? filePath = null;
            
            if (_dialogService != null)
            {
                // Use the dialog service if available
                filePath = await _dialogService.ShowOpenFileDialogAsync(
                    "Select LLVM IR File", 
                    new[] { "ll", "bc" }
                );
            }
            else
            {
                // Fallback to direct file picker
                var topLevel = GetTopLevel();
                if (topLevel?.StorageProvider is not { } storageProvider)
                {
                    FileStatusText = "File dialog not available";
                    return;
                }

                var fileTypes = new[]
                {
                    new FilePickerFileType("LLVM IR Files")
                    {
                        Patterns = new[] { "*.ll", "*.bc" },
                        MimeTypes = new[] { "text/plain", "application/octet-stream" }
                    },
                    new FilePickerFileType("All Files")
                    {
                        Patterns = new[] { "*.*" }
                    }
                };

                var options = new FilePickerOpenOptions
                {
                    Title = "Select LLVM IR File",
                    AllowMultiple = false,
                    FileTypeFilter = fileTypes
                };

                var files = await storageProvider.OpenFilePickerAsync(options);
                filePath = files.Count > 0 ? files[0].Path.LocalPath : null;
            }
            
            if (!string.IsNullOrEmpty(filePath))
            {
                await LoadFile(filePath);
            }
        }
        catch (Exception ex)
        {
            FileStatusText = $"Error: {ex.Message}";
        }
    }

    private static TopLevel? GetTopLevel()
    {
        if (Application.Current?.ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
        {
            return desktop.MainWindow;
        }
        return null;
    }

    [RelayCommand]
    private void ApplyPreset(string presetName)
    {
        var preset = PresetOptions.FirstOrDefault(p => p.Name == presetName);
        if (preset == null) return;

        foreach (var option in ObfuscationOptions)
        {
            option.IsEnabled = preset.Options.Contains(option.Name);
        }
    }

    public async Task LoadFile(string filePath)
    {
        if (!File.Exists(filePath)) 
        {
            FileStatusText = "File not found";
            return;
        }

        // Validate file extension
        var extension = Path.GetExtension(filePath).ToLowerInvariant();
        if (extension != ".ll" && extension != ".bc")
        {
            FileStatusText = "❌ Invalid file type";
            FileInfoText = "Only .ll (LLVM IR) and .bc (LLVM Bitcode) files are supported";
            IsFileSelected = false;
            return;
        }

        SelectedFilePath = filePath;
        var fileInfo = new FileInfo(filePath);
        
        FileStatusText = $"✓ {fileInfo.Name}";
        FileInfoText = $"Size: {FormatFileSize(fileInfo.Length)} • Modified: {fileInfo.LastWriteTime:yyyy-MM-dd}";
        IsFileSelected = true;

        // Update output filename based on input
        var inputName = Path.GetFileNameWithoutExtension(filePath);
        var inputExt = Path.GetExtension(filePath);
        OutputFileName = $"{inputName}_obfuscated{inputExt}";
        ReportFileName = $"{inputName}_report.txt";

        FileSelected?.Invoke(this, filePath);
        
        // Add await to satisfy async pattern
        await Task.CompletedTask;
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
        ObfuscationOptions.Add(new ObfuscationOption("Code Virtualization", false));
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

    private static string FormatFileSize(long bytes)
    {
        string[] sizes = { "B", "KB", "MB", "GB" };
        double len = bytes;
        int order = 0;
        while (len >= 1024 && order < sizes.Length - 1)
        {
            order++;
            len = len / 1024;
        }
        return $"{len:0.##} {sizes[order]}";
    }
}

public partial class ObfuscationOption : ObservableObject
{
    [ObservableProperty]
    private string _name;

    [ObservableProperty]
    private bool _isEnabled;

    public ObfuscationOption(string name, bool isEnabled)
    {
        Name = name;
        IsEnabled = isEnabled;
    }
}

public class PresetOption
{
    public string Name { get; }
    public string Description { get; }
    public string[] Options { get; }

    public PresetOption(string name, string description, string[] options)
    {
        Name = name;
        Description = description;
        Options = options;
    }
}
