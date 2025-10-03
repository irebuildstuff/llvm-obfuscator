using Avalonia.Controls;
using Avalonia.Interactivity;

namespace LLVMObfuscatorAvalonia.Views;

public partial class ResultsView : UserControl
{
    public ResultsView()
    {
        InitializeComponent();
    }

    private void ReturnToFileSelection_Click(object? sender, RoutedEventArgs e)
    {
        System.Diagnostics.Debug.WriteLine("ReturnToFileSelection_Click called");
        
        // Try to get the MainWindowViewModel from the DataContext chain
        var mainWindowViewModel = FindMainWindowViewModel();
        if (mainWindowViewModel != null)
        {
            System.Diagnostics.Debug.WriteLine("Found MainWindowViewModel, calling ReturnToFileSelection");
            mainWindowViewModel.ReturnToFileSelection();
        }
        else
        {
            System.Diagnostics.Debug.WriteLine("Could not find MainWindowViewModel");
        }
    }

    private ViewModels.MainWindowViewModel? FindMainWindowViewModel()
    {
        // Walk up the visual tree to find the MainWindow
        var current = this.Parent;
        while (current != null)
        {
            if (current is MainWindow mainWindow)
            {
                return mainWindow.DataContext as ViewModels.MainWindowViewModel;
            }
            current = current.Parent;
        }
        return null;
    }
}
