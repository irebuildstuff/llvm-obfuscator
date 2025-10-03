using System;

namespace LLVMObfuscatorAvalonia.Services
{
    /// <summary>
    /// Service for displaying notifications and toast messages
    /// </summary>
    public interface INotificationService
    {
        /// <summary>
        /// Shows a success notification
        /// </summary>
        void ShowSuccess(string message, string? title = null);

        /// <summary>
        /// Shows an information notification
        /// </summary>
        void ShowInformation(string message, string? title = null);

        /// <summary>
        /// Shows a warning notification
        /// </summary>
        void ShowWarning(string message, string? title = null);

        /// <summary>
        /// Shows an error notification
        /// </summary>
        void ShowError(string message, string? title = null);

        /// <summary>
        /// Event fired when a notification is shown
        /// </summary>
        event EventHandler<NotificationEventArgs>? NotificationShown;
    }

    public class NotificationEventArgs : EventArgs
    {
        public string Message { get; set; } = "";
        public string? Title { get; set; }
        public NotificationType Type { get; set; }
        public DateTime Timestamp { get; set; } = DateTime.Now;
    }

    public enum NotificationType
    {
        Success,
        Information,
        Warning,
        Error
    }
}

