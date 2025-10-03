using Avalonia;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Serilog;
using Serilog.Events;
using System;
using System.IO;

namespace LLVMObfuscatorAvalonia;

/// <summary>
/// Application entry point with dependency injection setup
/// </summary>
public class Program
{
    public static IServiceProvider? ServiceProvider { get; private set; }

    [STAThread]
    public static void Main(string[] args)
    {
        try
        {
            // Configure Serilog
            ConfigureLogging();

            // Build and configure host with DI
            var host = Host.CreateDefaultBuilder(args)
                .ConfigureServices(ConfigureServices)
                .UseSerilog()
                .Build();

            ServiceProvider = host.Services;

            // Start Avalonia app
            BuildAvaloniaApp()
                .StartWithClassicDesktopLifetime(args);
        }
        catch (Exception ex)
        {
            Log.Fatal(ex, "Application terminated unexpectedly");
        }
        finally
        {
            Log.CloseAndFlush();
        }
    }

    public static AppBuilder BuildAvaloniaApp()
        => AppBuilder.Configure<App>()
            .UsePlatformDetect()
            .WithInterFont()
            .LogToTrace();

    private static void ConfigureServices(IServiceCollection services)
    {
        // Configuration
        services.AddSingleton<Services.IConfigurationService, Services.ConfigurationService>();

        // Core Services
        services.AddSingleton<Services.IDialogService, Services.DialogService>();
        services.AddSingleton<Services.INotificationService, Services.NotificationService>();
        services.AddTransient<Services.ObfuscationService>();

        // ViewModels
        services.AddTransient<ViewModels.MainWindowViewModel>();
        services.AddTransient<ViewModels.FileSelectionViewModel>(provider => 
            new ViewModels.FileSelectionViewModel(provider.GetRequiredService<Services.IDialogService>()));
        services.AddTransient<ViewModels.ConfigurationViewModel>(provider => 
            new ViewModels.ConfigurationViewModel(provider.GetRequiredService<Services.ObfuscationService>()));
        services.AddTransient<ViewModels.ResultsViewModel>();

        // Validators
        services.AddTransient<Validators.ObfuscationOptionsValidator>();
    }

    private static void ConfigureLogging()
    {
        var logsDirectory = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
            "LLVMObfuscator",
            "Logs"
        );

        if (!Directory.Exists(logsDirectory))
        {
            Directory.CreateDirectory(logsDirectory);
        }

        Log.Logger = new LoggerConfiguration()
            .MinimumLevel.Debug()
            .MinimumLevel.Override("Microsoft", LogEventLevel.Information)
            .Enrich.FromLogContext()
            .Enrich.WithProperty("Application", "LLVMObfuscator")
            .WriteTo.Debug()
            .WriteTo.Console()
            .WriteTo.File(
                Path.Combine(logsDirectory, "llvm-obfuscator-.log"),
                rollingInterval: RollingInterval.Day,
                retainedFileCountLimit: 30,
                fileSizeLimitBytes: 10485760,
                outputTemplate: "{Timestamp:yyyy-MM-dd HH:mm:ss.fff zzz} [{Level:u3}] {Message:lj}{NewLine}{Exception}"
            )
            .CreateLogger();

        Log.Information("LLVM Obfuscator Application Starting");
        Log.Information("Logs Directory: {LogsDirectory}", logsDirectory);
    }
}

