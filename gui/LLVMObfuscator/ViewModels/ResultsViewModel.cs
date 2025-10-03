using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using System.Windows.Input;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

namespace LLVMObfuscatorAvalonia.ViewModels;

public partial class ResultsViewModel : ObservableObject
{
    [ObservableProperty]
    private int _progressValue = 25;

    [ObservableProperty]
    private string _progressText = "Ready to start obfuscation";

    [ObservableProperty]
    private string _progressDetails = "Select a file and configure obfuscation settings to begin";

    [ObservableProperty]
    private bool _isObfuscationComplete = false;

    public ObservableCollection<ResultItem> Results { get; } = new();

    // Action to call when returning to file selection
    public Action? ReturnToFileSelectionAction { get; set; }

    [RelayCommand]
    private async Task DownloadOutput()
    {
        // Implement download functionality
        await Task.Delay(100);
    }

    [RelayCommand]
    private async Task DownloadReport()
    {
        // Implement download functionality
        await Task.Delay(100);
    }

    [RelayCommand]
    private void ReturnToFileSelectionCommand()
    {
        System.Diagnostics.Debug.WriteLine("ReturnToFileSelectionCommand called - SIMPLE VERSION");
        // Just change a property to test if the command works
        ProgressText = "Button clicked! Testing...";
        System.Diagnostics.Debug.WriteLine("ProgressText updated");
    }

    public void UpdateProgress(int value, string text, string details)
    {
        ProgressValue = value;
        ProgressText = text;
        ProgressDetails = details;
    }

    public void CompleteObfuscation()
    {
        IsObfuscationComplete = true;
        UpdateProgress(100, "✅ Obfuscation completed!", "All transformations applied successfully");
        
        // Populate results
        Results.Clear();
        Results.Add(new ResultItem("Control Flow Obfuscations", "54", "Opaque predicates inserted"));
        Results.Add(new ResultItem("String Encryptions", "3", "Strings encrypted with XOR"));
        Results.Add(new ResultItem("Bogus Instructions", "948", "Complexity added"));
        Results.Add(new ResultItem("Processing Time", "2.3s", "Total obfuscation time"));
    }
}

public class ResultItem
{
    public string Metric { get; }
    public string Value { get; }
    public string Description { get; }

    public ResultItem(string metric, string value, string description)
    {
        Metric = metric;
        Value = value;
        Description = description;
    }
}
