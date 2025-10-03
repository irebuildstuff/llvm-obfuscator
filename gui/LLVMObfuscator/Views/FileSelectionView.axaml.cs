using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using LLVMObfuscatorAvalonia.ViewModels;
using System.Linq;

namespace LLVMObfuscatorAvalonia.Views;

public partial class FileSelectionView : UserControl
{
    public FileSelectionView()
    {
        InitializeComponent();
        
        // Enable drag and drop
        this.AddHandler(DragDrop.DragOverEvent, OnDragOver);
        this.AddHandler(DragDrop.DropEvent, OnDrop);
    }

    private void OnDragOver(object? sender, DragEventArgs e)
    {
        // Check if the dragged data contains files
        if (e.Data.Contains(DataFormats.Files))
        {
            var files = e.Data.GetFiles()?.ToList();
            if (files?.Count == 1)
            {
                var file = files[0];
                var fileName = file.Name.ToLower();
                
                // Check if it's a supported file type
                if (fileName.EndsWith(".ll") || fileName.EndsWith(".bc"))
                {
                    e.DragEffects = DragDropEffects.Copy;
                    return;
                }
            }
        }
        
        e.DragEffects = DragDropEffects.None;
    }

    private async void OnDrop(object? sender, DragEventArgs e)
    {
        if (e.Data.Contains(DataFormats.Files))
        {
            var files = e.Data.GetFiles()?.ToList();
            if (files?.Count == 1)
            {
                var file = files[0];
                var fileName = file.Name.ToLower();
                
                // Check if it's a supported file type
                if (fileName.EndsWith(".ll") || fileName.EndsWith(".bc"))
                {
                    if (DataContext is FileSelectionViewModel viewModel)
                    {
                        await viewModel.LoadFile(file.Path.LocalPath);
                    }
                }
            }
        }
    }
}
