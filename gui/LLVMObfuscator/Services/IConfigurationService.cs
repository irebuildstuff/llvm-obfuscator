using System;
using System.Threading.Tasks;
using LLVMObfuscatorAvalonia.Models;

namespace LLVMObfuscatorAvalonia.Services
{
    /// <summary>
    /// Service for managing application configuration and settings
    /// </summary>
    public interface IConfigurationService
    {
        /// <summary>
        /// Gets the current application settings
        /// </summary>
        AppSettings Settings { get; }

        /// <summary>
        /// Loads settings from persistent storage
        /// </summary>
        Task LoadSettingsAsync();

        /// <summary>
        /// Saves settings to persistent storage
        /// </summary>
        Task SaveSettingsAsync();

        /// <summary>
        /// Adds a file to the recent files list
        /// </summary>
        void AddRecentFile(string filePath);

        /// <summary>
        /// Gets the recent files list
        /// </summary>
        System.Collections.Generic.List<string> GetRecentFiles();

        /// <summary>
        /// Clears the recent files list
        /// </summary>
        void ClearRecentFiles();

        /// <summary>
        /// Saves a new obfuscation profile
        /// </summary>
        void SaveProfile(ObfuscationProfile profile);

        /// <summary>
        /// Loads a saved profile
        /// </summary>
        ObfuscationProfile? LoadProfile(string name);

        /// <summary>
        /// Deletes a saved profile
        /// </summary>
        void DeleteProfile(string name);

        /// <summary>
        /// Gets all saved profiles
        /// </summary>
        System.Collections.Generic.List<ObfuscationProfile> GetAllProfiles();

        /// <summary>
        /// Exports settings to a file
        /// </summary>
        Task ExportSettingsAsync(string filePath);

        /// <summary>
        /// Imports settings from a file
        /// </summary>
        Task ImportSettingsAsync(string filePath);

        /// <summary>
        /// Resets settings to default values
        /// </summary>
        Task ResetToDefaultsAsync();

        /// <summary>
        /// Event fired when settings are changed
        /// </summary>
        event EventHandler? SettingsChanged;
    }
}

