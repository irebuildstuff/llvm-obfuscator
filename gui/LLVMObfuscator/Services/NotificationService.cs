using System;
using Microsoft.Extensions.Logging;

namespace LLVMObfuscatorAvalonia.Services
{
    /// <summary>
    /// Implementation of notification service
    /// </summary>
    public class NotificationService : INotificationService
    {
        private readonly ILogger<NotificationService> _logger;

        public event EventHandler<NotificationEventArgs>? NotificationShown;

        public NotificationService(ILogger<NotificationService> logger)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }

        public void ShowSuccess(string message, string? title = null)
        {
            _logger.LogInformation("Success notification: {Message}", message);
            RaiseNotification(message, title, NotificationType.Success);
        }

        public void ShowInformation(string message, string? title = null)
        {
            _logger.LogInformation("Information notification: {Message}", message);
            RaiseNotification(message, title, NotificationType.Information);
        }

        public void ShowWarning(string message, string? title = null)
        {
            _logger.LogWarning("Warning notification: {Message}", message);
            RaiseNotification(message, title, NotificationType.Warning);
        }

        public void ShowError(string message, string? title = null)
        {
            _logger.LogError("Error notification: {Message}", message);
            RaiseNotification(message, title, NotificationType.Error);
        }

        private void RaiseNotification(string message, string? title, NotificationType type)
        {
            NotificationShown?.Invoke(this, new NotificationEventArgs
            {
                Message = message,
                Title = title,
                Type = type,
                Timestamp = DateTime.Now
            });
        }
    }
}

