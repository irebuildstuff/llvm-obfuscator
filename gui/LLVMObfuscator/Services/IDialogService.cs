using System.Threading.Tasks;

namespace LLVMObfuscatorAvalonia.Services
{
    /// <summary>
    /// Service for displaying dialogs and user prompts
    /// </summary>
    public interface IDialogService
    {
        /// <summary>
        /// Shows an information dialog
        /// </summary>
        Task ShowInformationAsync(string title, string message);

        /// <summary>
        /// Shows a warning dialog
        /// </summary>
        Task ShowWarningAsync(string title, string message);

        /// <summary>
        /// Shows an error dialog
        /// </summary>
        Task ShowErrorAsync(string title, string message);

        /// <summary>
        /// Shows a confirmation dialog
        /// </summary>
        Task<bool> ShowConfirmationAsync(string title, string message);

        /// <summary>
        /// Shows a file picker dialog for opening files
        /// </summary>
        Task<string?> ShowOpenFileDialogAsync(string title, string[]? fileTypes = null);

        /// <summary>
        /// Shows a file picker dialog for saving files
        /// </summary>
        Task<string?> ShowSaveFileDialogAsync(string title, string? defaultFileName = null, string[]? fileTypes = null);

        /// <summary>
        /// Shows a folder picker dialog
        /// </summary>
        Task<string?> ShowFolderPickerDialogAsync(string title);

        /// <summary>
        /// Shows a progress dialog
        /// </summary>
        Task ShowProgressDialogAsync(string title, string message, System.Action<System.IProgress<int>> action);
    }
}

