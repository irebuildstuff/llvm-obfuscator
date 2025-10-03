using System;
using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Markup.Xaml;
using LLVMObfuscatorAvalonia.ViewModels;
using LLVMObfuscatorAvalonia.Views;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;

namespace LLVMObfuscatorAvalonia;

/// <summary>
/// Main application class with dependency injection integration
/// </summary>
public partial class App : Application
{
    private ILogger<App>? _logger;

    public override void Initialize()
    {
        AvaloniaXamlLoader.Load(this);
    }

    public override async void OnFrameworkInitializationCompleted()
    {
        if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
        {
            // Get logger
            _logger = Program.ServiceProvider?.GetService<ILogger<App>>();
            _logger?.LogInformation("Application framework initialization completed");

            // Load configuration
            var configService = Program.ServiceProvider?.GetService<Services.IConfigurationService>();
            if (configService != null)
            {
                await configService.LoadSettingsAsync();
                _logger?.LogInformation("Configuration loaded");
            }

            // Create main window with DI
            try
            {
                var viewModel = Program.ServiceProvider?.GetService<MainWindowViewModel>();
                _logger?.LogInformation("ViewModel retrieved from DI: {IsNull}", viewModel == null ? "NULL" : "OK");

                var mainWindow = new MainWindow
                {
                    DataContext = viewModel ?? new MainWindowViewModel(),
                };

                // Set window reference in ViewModel for minimize/close commands
                if (viewModel != null)
                {
                    viewModel.Window = mainWindow;
                }

                desktop.MainWindow = mainWindow;

                _logger?.LogInformation("Main window created and assigned to desktop");
                _logger?.LogInformation("Window DataContext: {DataContext}", mainWindow.DataContext?.GetType().Name ?? "NULL");

                // Explicitly show and activate the window (important for Avalonia)
                mainWindow.Show();
                mainWindow.Activate();
                _logger?.LogInformation("Window shown and activated");
            }
            catch (Exception ex)
            {
                _logger?.LogError(ex, "Error creating main window");
                throw;
            }

            // Handle application exit
            desktop.Exit += OnExit;
        }

        base.OnFrameworkInitializationCompleted();
    }

    private async void OnExit(object? sender, ControlledApplicationLifetimeExitEventArgs e)
    {
        _logger?.LogInformation("Application exiting");
        
        // Save configuration
        var configService = Program.ServiceProvider?.GetService<Services.IConfigurationService>();
        if (configService != null)
        {
            await configService.SaveSettingsAsync();
            _logger?.LogInformation("Configuration saved");
        }
    }
}

