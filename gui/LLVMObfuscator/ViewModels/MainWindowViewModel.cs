using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using Avalonia;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using LLVMObfuscatorAvalonia.Services;
using LLVMObfuscatorAvalonia.Views;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;

namespace LLVMObfuscatorAvalonia.ViewModels;

/// <summary>
/// Main window view model with dependency injection and advanced features
/// </summary>
public partial class MainWindowViewModel : ObservableObject
{
    private readonly ILogger<MainWindowViewModel>? _logger;
    private readonly IConfigurationService? _configService;
    private readonly IDialogService? _dialogService;
    private readonly INotificationService? _notificationService;

    [ObservableProperty]
    private string _statusMessage = "Ready";

    [ObservableProperty]
    private string _themeButtonText = "Dark";

    [ObservableProperty]
    private bool _isDarkTheme = false;

    [ObservableProperty]
    private string _applicationTitle = "LLVM Code Obfuscator";

    [ObservableProperty]
    private object _currentView = null!;

    [ObservableProperty]
    private bool _isFileSelectionVisible = true;

    [ObservableProperty]
    private bool _isConfigurationVisible = false;

    [ObservableProperty]
    private bool _isResultsVisible = false;

    public FileSelectionViewModel FileSelectionViewModel { get; }
    public ConfigurationViewModel ConfigurationViewModel { get; }
    public ResultsViewModel ResultsViewModel { get; }
    
    // View instances
    private FileSelectionView? _fileSelectionView;
    private ConfigurationView? _configurationView;
    private ResultsView? _resultsView;
    
    public ObservableCollection<string> RecentFiles { get; } = new();
    public ObservableCollection<string> Notifications { get; } = new();

    // Window reference for minimize/close operations
    public Avalonia.Controls.Window? Window { get; set; }

    public MainWindowViewModel()
    {
        // Fallback constructor for design-time - get services from DI if available
        _logger = Program.ServiceProvider?.GetService<ILogger<MainWindowViewModel>>();
        _configService = Program.ServiceProvider?.GetService<IConfigurationService>();
        _dialogService = Program.ServiceProvider?.GetService<IDialogService>();
        _notificationService = Program.ServiceProvider?.GetService<INotificationService>();
        
        // If services are null, we're in design mode - use dummy implementations
        if (_logger == null || _configService == null || _dialogService == null || _notificationService == null)
        {
            FileSelectionViewModel = new FileSelectionViewModel();
            ConfigurationViewModel = new ConfigurationViewModel();
            ResultsViewModel = new ResultsViewModel();
            return;
        }
        
        // Initialize properly with services
        _logger.LogInformation("Initializing MainWindowViewModel (fallback constructor)");

        // Get ViewModels from DI
        FileSelectionViewModel = Program.ServiceProvider?.GetService<FileSelectionViewModel>() ?? new FileSelectionViewModel();
        ConfigurationViewModel = Program.ServiceProvider?.GetService<ConfigurationViewModel>() ?? new ConfigurationViewModel();
        ResultsViewModel = Program.ServiceProvider?.GetService<ResultsViewModel>() ?? new ResultsViewModel();

        // Connect view models
        FileSelectionViewModel.FileSelected += (sender, filePath) => OnFileSelected(filePath);
        ConfigurationViewModel.ObfuscationStarted += (sender, e) => OnObfuscationStarted();
        ConfigurationViewModel.ObfuscationCompleted += (sender, success) => OnObfuscationCompleted(success);
        ConfigurationViewModel.ProgressUpdated += (sender, progress) => OnProgressUpdated(progress);
        ResultsViewModel.ReturnToFileSelectionAction = ReturnToFileSelection;

        // Subscribe to notifications
        _notificationService.NotificationShown += OnNotificationShown;

        // Load recent files and theme
        LoadRecentFiles();
        LoadThemePreference();
        
        // Initialize current view to file selection
        SetCurrentView(FileSelectionViewModel);
    }

    public MainWindowViewModel(
        ILogger<MainWindowViewModel> logger,
        IConfigurationService configService,
        IDialogService dialogService,
        INotificationService notificationService)
    {
        _logger = logger;
        _configService = configService;
        _dialogService = dialogService;
        _notificationService = notificationService;

        _logger.LogInformation("Initializing MainWindowViewModel");

        // Get ViewModels from DI
        FileSelectionViewModel = Program.ServiceProvider?.GetService<FileSelectionViewModel>() ?? new FileSelectionViewModel();
        ConfigurationViewModel = Program.ServiceProvider?.GetService<ConfigurationViewModel>() ?? new ConfigurationViewModel();
        ResultsViewModel = Program.ServiceProvider?.GetService<ResultsViewModel>() ?? new ResultsViewModel();

        // Connect view models
        FileSelectionViewModel.FileSelected += (sender, filePath) => OnFileSelected(filePath);
        ConfigurationViewModel.ObfuscationStarted += (sender, e) => OnObfuscationStarted();
        ConfigurationViewModel.ObfuscationCompleted += (sender, success) => OnObfuscationCompleted(success);
        ConfigurationViewModel.ProgressUpdated += (sender, progress) => OnProgressUpdated(progress);
        ResultsViewModel.ReturnToFileSelectionAction = ReturnToFileSelection;

        // Subscribe to notifications
        _notificationService.NotificationShown += OnNotificationShown;

        // Load recent files
        LoadRecentFiles();

        // Load theme preference
        LoadThemePreference();
        
        // Initialize current view to file selection
        SetCurrentView(FileSelectionViewModel);
    }

    [RelayCommand]
    private void ToggleTheme()
    {
        IsDarkTheme = !IsDarkTheme;
        ThemeButtonText = IsDarkTheme ? "Light" : "Dark";
        
        // Apply theme
        if (Application.Current != null)
        {
            Application.Current.RequestedThemeVariant = IsDarkTheme 
                ? Avalonia.Styling.ThemeVariant.Dark 
                : Avalonia.Styling.ThemeVariant.Light;
        }
        
        // Save preference
        if (_configService != null)
        {
            _configService.Settings.Application.Theme = IsDarkTheme ? "Dark" : "Light";
            _ = _configService.SaveSettingsAsync();
        }

        StatusMessage = IsDarkTheme ? "Switched to dark theme" : "Switched to light theme";
        _logger?.LogInformation("Theme toggled to {Theme}", IsDarkTheme ? "Dark" : "Light");
        _notificationService?.ShowInformation($"Theme changed to {(IsDarkTheme ? "Dark" : "Light")}");
    }

    [RelayCommand]
    private async Task Help()
    {
        _logger?.LogInformation("Help requested");
        
        var helpMessage = @"LLVM Code Obfuscator Help

File Selection:
• Browse and select your LLVM IR file (.ll or .bc)
• View file information and size
• Recent files are saved for quick access

Configuration:
• Choose a preset (Basic, Standard, Aggressive, Custom)
• Enable specific obfuscation techniques
• Configure advanced options
• Save profiles for reuse

Results:
• Monitor obfuscation progress
• View detailed statistics
• Download output and reports
• Review transformation details

Keyboard Shortcuts:
• Ctrl+O: Open file
• Ctrl+S: Save profile
• Ctrl+R: Start obfuscation
• F1: Help
• F11: Toggle fullscreen";

        await _dialogService?.ShowInformationAsync("Help", helpMessage);
    }

    [RelayCommand]
    private async Task About()
    {
        _logger?.LogInformation("About dialog requested");
        
        var aboutMessage = @"LLVM Code Obfuscator
Version 4.0.0

Professional LLVM-based code obfuscation tool with enterprise-grade features.

Built with:
• Avalonia UI 11.0
• .NET 8
• Serilog Logging
• Dependency Injection
• FluentValidation

Features:
✓ 15+ obfuscation techniques
✓ Profile management
✓ Batch processing
✓ Comprehensive logging
✓ Cross-platform support

© 2025 LLVM Obfuscator Project
All rights reserved.";

        await _dialogService?.ShowInformationAsync("About", aboutMessage);
    }

    [RelayCommand]
    private async Task OpenRecentFile(string filePath)
    {
        _logger?.LogInformation("Opening recent file: {FilePath}", filePath);
        
        if (System.IO.File.Exists(filePath))
        {
            await FileSelectionViewModel.LoadFile(filePath);
            SetCurrentView(FileSelectionViewModel); // Switch to file selection view
            _notificationService?.ShowSuccess($"Loaded file: {System.IO.Path.GetFileName(filePath)}");
        }
        else
        {
            _notificationService?.ShowWarning("File not found", "The selected file no longer exists");
            RecentFiles.Remove(filePath);
        }
    }

    [RelayCommand]
    private async Task ClearRecentFiles()
    {
        _logger?.LogInformation("Clearing recent files");
        
        var confirmed = _dialogService != null ? await _dialogService.ShowConfirmationAsync(
            "Clear Recent Files",
            "Are you sure you want to clear all recent files?"
        ) : false;

        if (confirmed)
        {
            _configService?.ClearRecentFiles();
            RecentFiles.Clear();
            _notificationService?.ShowInformation("Recent files cleared");
        }
    }

    [RelayCommand]
    private async Task ExportSettings()
    {
        _logger?.LogInformation("Exporting settings");
        
        var filePath = await _dialogService?.ShowSaveFileDialogAsync(
            "Export Settings",
            "settings.json",
            new[] { "json" }
        );

        if (!string.IsNullOrEmpty(filePath))
        {
            try
            {
                await _configService?.ExportSettingsAsync(filePath);
                _notificationService?.ShowSuccess("Settings exported successfully");
            }
            catch (Exception ex)
            {
                _logger?.LogError(ex, "Error exporting settings");
                await _dialogService?.ShowErrorAsync("Export Error", $"Failed to export settings: {ex.Message}");
            }
        }
    }

    [RelayCommand]
    private async Task ImportSettings()
    {
        _logger?.LogInformation("Importing settings");
        
        var filePath = await _dialogService?.ShowOpenFileDialogAsync(
            "Import Settings",
            new[] { "json" }
        );

        if (!string.IsNullOrEmpty(filePath))
        {
            try
            {
                await _configService?.ImportSettingsAsync(filePath);
                LoadRecentFiles();
                LoadThemePreference();
                _notificationService?.ShowSuccess("Settings imported successfully");
            }
            catch (Exception ex)
            {
                _logger?.LogError(ex, "Error importing settings");
                await _dialogService?.ShowErrorAsync("Import Error", $"Failed to import settings: {ex.Message}");
            }
        }
    }

    [RelayCommand]
    private async Task ResetSettings()
    {
        _logger?.LogInformation("Resetting settings");
        
        var confirmed = _dialogService != null ? await _dialogService.ShowConfirmationAsync(
            "Reset Settings",
            "Are you sure you want to reset all settings to default values? This cannot be undone."
        ) : false;

        if (confirmed)
        {
            await _configService?.ResetToDefaultsAsync();
            LoadRecentFiles();
            LoadThemePreference();
            _notificationService?.ShowInformation("Settings reset to defaults");
        }
    }

    [RelayCommand]
    private void Minimize()
    {
        _logger?.LogInformation("Minimizing window");
        if (Window != null)
        {
            Window.WindowState = Avalonia.Controls.WindowState.Minimized;
        }
    }

    [RelayCommand]
    private async Task Close()
    {
        _logger?.LogInformation("Closing application");
        
        var confirmed = _dialogService != null ? await _dialogService.ShowConfirmationAsync(
            "Exit Application",
            "Are you sure you want to exit?"
        ) : false;

        if (confirmed)
        {
            // Save settings before closing
            await _configService?.SaveSettingsAsync();
            _logger?.LogInformation("Application closing - settings saved");
            
            Window?.Close();
        }
    }

    [RelayCommand]
    private void ReturnToFileSelectionCommand()
    {
        try
        {
            System.Diagnostics.Debug.WriteLine("ReturnToFileSelectionCommand called directly");
            StatusMessage = "Button clicked! Testing direct command...";
            System.Diagnostics.Debug.WriteLine("Direct command executed successfully");
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Error in ReturnToFileSelectionCommand: {ex.Message}");
            StatusMessage = "Error in button command";
        }
    }

    private void OnFileSelected(string filePath)
    {
        StatusMessage = $"File selected: {System.IO.Path.GetFileName(filePath)}";
        ConfigurationViewModel.SelectedFilePath = filePath;
        
        // Add to recent files
        _configService?.AddRecentFile(filePath);
        LoadRecentFiles();
        
        // Auto-switch to configuration view
        SetCurrentView(ConfigurationViewModel);
        
        _notificationService?.ShowSuccess($"Loaded: {System.IO.Path.GetFileName(filePath)}");
    }

    private void OnObfuscationStarted()
    {
        StatusMessage = "Obfuscation in progress...";
        SetCurrentView(ResultsViewModel); // Switch to results view
        _notificationService?.ShowInformation("Obfuscation started");
        
        // Initialize progress
        ResultsViewModel.UpdateProgress(0, "Starting obfuscation...", "Preparing to obfuscate your code");
    }

    private void OnObfuscationCompleted(bool success)
    {
        if (success)
        {
            StatusMessage = "✅ Obfuscation completed successfully!";
            _notificationService?.ShowSuccess("Obfuscation completed successfully!");
            ResultsViewModel.CompleteObfuscation();
        }
        else
        {
            StatusMessage = "❌ Obfuscation failed";
            _notificationService?.ShowError("Obfuscation failed. Check logs for details.");
        }
    }

    private void OnProgressUpdated((int progress, string text, string details) progressData)
    {
        ResultsViewModel.UpdateProgress(progressData.progress, progressData.text, progressData.details);
    }

    public void ReturnToFileSelection()
    {
        try
        {
            System.Diagnostics.Debug.WriteLine("ReturnToFileSelection called");
            StatusMessage = "Returning to file selection...";
            
            // Switch back to file selection view
            SetCurrentView(FileSelectionViewModel);
            
            // Show notification
            _notificationService?.ShowInformation("Returned to file selection. You can now select another file.");
            
            System.Diagnostics.Debug.WriteLine("ReturnToFileSelection completed successfully");
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Error in ReturnToFileSelection: {ex.Message}");
            _logger?.LogError(ex, "Error in ReturnToFileSelection");
            StatusMessage = "Error returning to file selection";
        }
    }

    private void OnNotificationShown(object? sender, NotificationEventArgs e)
    {
        var icon = e.Type switch
        {
            NotificationType.Success => "✓",
            NotificationType.Information => "ℹ",
            NotificationType.Warning => "⚠",
            NotificationType.Error => "✗",
            _ => ""
        };

        var notification = $"{icon} {e.Message}";
        Notifications.Insert(0, notification);
        
        // Keep only last 10 notifications
        while (Notifications.Count > 10)
        {
            Notifications.RemoveAt(Notifications.Count - 1);
        }
    }

    private void LoadRecentFiles()
    {
        RecentFiles.Clear();
        var recentFiles = _configService?.GetRecentFiles() ?? new System.Collections.Generic.List<string>();
        foreach (var file in recentFiles.Take(10))
        {
            RecentFiles.Add(file);
        }
    }

    private void LoadThemePreference()
    {
        if (_configService?.Settings?.Application != null)
        {
            IsDarkTheme = _configService.Settings.Application.Theme == "Dark";
            ThemeButtonText = IsDarkTheme ? "Light" : "Dark";
            
            // Apply theme
            if (Application.Current != null)
            {
                Application.Current.RequestedThemeVariant = IsDarkTheme 
                    ? Avalonia.Styling.ThemeVariant.Dark 
                    : Avalonia.Styling.ThemeVariant.Light;
            }
        }
    }

    private void SetCurrentView(object viewModel)
    {
        try
        {
            System.Diagnostics.Debug.WriteLine($"SetCurrentView called with: {viewModel.GetType().Name}");
            
            // Create views only once and reuse them
            if (viewModel == FileSelectionViewModel)
            {
                _fileSelectionView ??= new FileSelectionView { DataContext = FileSelectionViewModel };
                CurrentView = _fileSelectionView;
            }
            else if (viewModel == ConfigurationViewModel)
            {
                _configurationView ??= new ConfigurationView { DataContext = ConfigurationViewModel };
                CurrentView = _configurationView;
            }
            else if (viewModel == ResultsViewModel)
            {
                _resultsView ??= new ResultsView { DataContext = ResultsViewModel };
                CurrentView = _resultsView;
            }
            else
            {
                _fileSelectionView ??= new FileSelectionView { DataContext = FileSelectionViewModel };
                CurrentView = _fileSelectionView;
            }
            
            // Update visibility properties
            IsFileSelectionVisible = viewModel == FileSelectionViewModel;
            IsConfigurationVisible = viewModel == ConfigurationViewModel;
            IsResultsVisible = viewModel == ResultsViewModel;
            
            System.Diagnostics.Debug.WriteLine($"SetCurrentView completed successfully");
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Error in SetCurrentView: {ex.Message}");
            throw;
        }
    }
}
