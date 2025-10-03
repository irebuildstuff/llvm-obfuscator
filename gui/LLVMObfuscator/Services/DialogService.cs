using System;
using System.Linq;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Platform.Storage;
using Microsoft.Extensions.Logging;

namespace LLVMObfuscatorAvalonia.Services
{
    /// <summary>
    /// Implementation of dialog service using Avalonia dialogs
    /// </summary>
    public class DialogService : IDialogService
    {
        private readonly ILogger<DialogService> _logger;

        public DialogService(ILogger<DialogService> logger)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }

        private Window? GetMainWindow()
        {
            if (Application.Current?.ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
            {
                return desktop.MainWindow;
            }
            return null;
        }

        public async Task ShowInformationAsync(string title, string message)
        {
            _logger.LogInformation("Showing information dialog: {Title}", title);
            
            var window = GetMainWindow();
            if (window == null)
            {
                _logger.LogWarning("Main window not available for dialog");
                return;
            }

            var dialog = new Window
            {
                Title = title,
                Width = 500,
                Height = 250,
                WindowStartupLocation = WindowStartupLocation.CenterOwner,
                CanResize = false
            };

            var stack = new StackPanel { Margin = new Thickness(20), Spacing = 15 };
            stack.Children.Add(new TextBlock 
            { 
                Text = "ℹ️ " + message, 
                TextWrapping = Avalonia.Media.TextWrapping.Wrap,
                FontSize = 14
            });

            var button = new Button 
            { 
                Content = "OK", 
                HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Right,
                Padding = new Thickness(30, 10)
            };
            button.Click += (_, _) => dialog.Close();
            stack.Children.Add(button);

            dialog.Content = stack;
            await dialog.ShowDialog(window);
        }

        public async Task ShowWarningAsync(string title, string message)
        {
            _logger.LogWarning("Showing warning dialog: {Title}", title);
            
            var window = GetMainWindow();
            if (window == null) return;

            var dialog = new Window
            {
                Title = title,
                Width = 500,
                Height = 250,
                WindowStartupLocation = WindowStartupLocation.CenterOwner,
                CanResize = false
            };

            var stack = new StackPanel { Margin = new Thickness(20), Spacing = 15 };
            stack.Children.Add(new TextBlock 
            { 
                Text = "⚠️ " + message, 
                TextWrapping = Avalonia.Media.TextWrapping.Wrap,
                FontSize = 14,
                Foreground = Avalonia.Media.Brushes.Orange
            });

            var button = new Button 
            { 
                Content = "OK", 
                HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Right,
                Padding = new Thickness(30, 10)
            };
            button.Click += (_, _) => dialog.Close();
            stack.Children.Add(button);

            dialog.Content = stack;
            await dialog.ShowDialog(window);
        }

        public async Task ShowErrorAsync(string title, string message)
        {
            _logger.LogError("Showing error dialog: {Title} - {Message}", title, message);
            
            var window = GetMainWindow();
            if (window == null) return;

            var dialog = new Window
            {
                Title = title,
                Width = 500,
                Height = 250,
                WindowStartupLocation = WindowStartupLocation.CenterOwner,
                CanResize = false
            };

            var stack = new StackPanel { Margin = new Thickness(20), Spacing = 15 };
            stack.Children.Add(new TextBlock 
            { 
                Text = "❌ " + message, 
                TextWrapping = Avalonia.Media.TextWrapping.Wrap,
                FontSize = 14,
                Foreground = Avalonia.Media.Brushes.Red
            });

            var button = new Button 
            { 
                Content = "OK", 
                HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Right,
                Padding = new Thickness(30, 10)
            };
            button.Click += (_, _) => dialog.Close();
            stack.Children.Add(button);

            dialog.Content = stack;
            await dialog.ShowDialog(window);
        }

        public async Task<bool> ShowConfirmationAsync(string title, string message)
        {
            _logger.LogInformation("Showing confirmation dialog: {Title}", title);
            
            var window = GetMainWindow();
            if (window == null) return false;

            bool result = false;

            var dialog = new Window
            {
                Title = title,
                Width = 500,
                Height = 250,
                WindowStartupLocation = WindowStartupLocation.CenterOwner,
                CanResize = false
            };

            var stack = new StackPanel { Margin = new Thickness(20), Spacing = 15 };
            stack.Children.Add(new TextBlock 
            { 
                Text = "❓ " + message, 
                TextWrapping = Avalonia.Media.TextWrapping.Wrap,
                FontSize = 14
            });

            var buttonPanel = new StackPanel 
            { 
                Orientation = Avalonia.Layout.Orientation.Horizontal,
                HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Right,
                Spacing = 10
            };

            var yesButton = new Button 
            { 
                Content = "Yes", 
                Padding = new Thickness(30, 10)
            };
            yesButton.Click += (_, _) => 
            { 
                result = true; 
                dialog.Close(); 
            };

            var noButton = new Button 
            { 
                Content = "No", 
                Padding = new Thickness(30, 10)
            };
            noButton.Click += (_, _) => 
            { 
                result = false; 
                dialog.Close(); 
            };

            buttonPanel.Children.Add(yesButton);
            buttonPanel.Children.Add(noButton);
            stack.Children.Add(buttonPanel);

            dialog.Content = stack;
            await dialog.ShowDialog(window);

            return result;
        }

        public async Task<string?> ShowOpenFileDialogAsync(string title, string[]? fileTypes = null)
        {
            _logger.LogInformation("Showing open file dialog: {Title}", title);
            
            var window = GetMainWindow();
            if (window?.StorageProvider == null) return null;

            var fileTypeFilters = fileTypes?.Select(ft => new FilePickerFileType(ft)
            {
                Patterns = new[] { $"*.{ft}" }
            }).ToArray();

            var options = new FilePickerOpenOptions
            {
                Title = title,
                AllowMultiple = false,
                FileTypeFilter = fileTypeFilters
            };

            var files = await window.StorageProvider.OpenFilePickerAsync(options);
            return files.Count > 0 ? files[0].Path.LocalPath : null;
        }

        public async Task<string?> ShowSaveFileDialogAsync(string title, string? defaultFileName = null, string[]? fileTypes = null)
        {
            _logger.LogInformation("Showing save file dialog: {Title}", title);
            
            var window = GetMainWindow();
            if (window?.StorageProvider == null) return null;

            var fileTypeFilters = fileTypes?.Select(ft => new FilePickerFileType(ft)
            {
                Patterns = new[] { $"*.{ft}" }
            }).ToArray();

            var options = new FilePickerSaveOptions
            {
                Title = title,
                SuggestedFileName = defaultFileName,
                FileTypeChoices = fileTypeFilters
            };

            var file = await window.StorageProvider.SaveFilePickerAsync(options);
            return file?.Path.LocalPath;
        }

        public async Task<string?> ShowFolderPickerDialogAsync(string title)
        {
            _logger.LogInformation("Showing folder picker dialog: {Title}", title);
            
            var window = GetMainWindow();
            if (window?.StorageProvider == null) return null;

            var options = new FolderPickerOpenOptions
            {
                Title = title,
                AllowMultiple = false
            };

            var folders = await window.StorageProvider.OpenFolderPickerAsync(options);
            return folders.Count > 0 ? folders[0].Path.LocalPath : null;
        }

        public async Task ShowProgressDialogAsync(string title, string message, Action<IProgress<int>> action)
        {
            _logger.LogInformation("Showing progress dialog: {Title}", title);
            
            // This is a simplified implementation
            // In production, you'd use a proper progress dialog control
            await Task.Run(() => 
            {
                var progress = new Progress<int>();
                action(progress);
            });
        }
    }
}

