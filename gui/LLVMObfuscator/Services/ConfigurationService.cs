using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using LLVMObfuscatorAvalonia.Models;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;

namespace LLVMObfuscatorAvalonia.Services
{
    /// <summary>
    /// Implementation of configuration service for managing app settings
    /// </summary>
    public class ConfigurationService : IConfigurationService
    {
        private readonly ILogger<ConfigurationService> _logger;
        private readonly string _settingsFilePath;
        private AppSettings _settings;

        public event EventHandler? SettingsChanged;

        public AppSettings Settings => _settings;

        public ConfigurationService(ILogger<ConfigurationService> logger)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
            _settingsFilePath = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
                "LLVMObfuscator",
                "settings.json"
            );
            _settings = new AppSettings();
        }

        public async Task LoadSettingsAsync()
        {
            try
            {
                _logger.LogInformation("Loading settings from {Path}", _settingsFilePath);

                if (File.Exists(_settingsFilePath))
                {
                    var json = await File.ReadAllTextAsync(_settingsFilePath);
                    var loadedSettings = JsonConvert.DeserializeObject<AppSettings>(json);
                    
                    if (loadedSettings != null)
                    {
                        _settings = loadedSettings;
                        _logger.LogInformation("Settings loaded successfully");
                        SettingsChanged?.Invoke(this, EventArgs.Empty);
                    }
                }
                else
                {
                    _logger.LogInformation("Settings file not found, using defaults");
                    await SaveSettingsAsync(); // Create default settings file
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error loading settings");
                _settings = new AppSettings(); // Fall back to defaults
            }
        }

        public async Task SaveSettingsAsync()
        {
            try
            {
                var directory = Path.GetDirectoryName(_settingsFilePath);
                if (!string.IsNullOrEmpty(directory) && !Directory.Exists(directory))
                {
                    Directory.CreateDirectory(directory);
                }

                var json = JsonConvert.SerializeObject(_settings, Formatting.Indented);
                await File.WriteAllTextAsync(_settingsFilePath, json);
                
                _logger.LogInformation("Settings saved successfully");
                SettingsChanged?.Invoke(this, EventArgs.Empty);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error saving settings");
            }
        }

        public void AddRecentFile(string filePath)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(filePath) || !File.Exists(filePath))
                    return;

                // Remove if already exists
                _settings.RecentFiles.Remove(filePath);
                
                // Add to front
                _settings.RecentFiles.Insert(0, filePath);
                
                // Trim to max size
                while (_settings.RecentFiles.Count > _settings.Application.MaxRecentFiles)
                {
                    _settings.RecentFiles.RemoveAt(_settings.RecentFiles.Count - 1);
                }

                _ = SaveSettingsAsync(); // Fire and forget
                _logger.LogInformation("Added recent file: {FilePath}", filePath);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error adding recent file");
            }
        }

        public List<string> GetRecentFiles()
        {
            // Filter out files that no longer exist
            var existingFiles = _settings.RecentFiles.Where(File.Exists).ToList();
            
            if (existingFiles.Count != _settings.RecentFiles.Count)
            {
                _settings.RecentFiles = existingFiles;
                _ = SaveSettingsAsync();
            }

            return new List<string>(_settings.RecentFiles);
        }

        public void ClearRecentFiles()
        {
            _settings.RecentFiles.Clear();
            _ = SaveSettingsAsync();
            _logger.LogInformation("Recent files cleared");
        }

        public void SaveProfile(ObfuscationProfile profile)
        {
            try
            {
                // Remove existing profile with same name
                _settings.SavedProfiles.RemoveAll(p => p.Name == profile.Name);
                
                profile.LastUsedDate = DateTime.Now;
                if (profile.CreatedDate == default)
                {
                    profile.CreatedDate = DateTime.Now;
                }

                _settings.SavedProfiles.Add(profile);
                _ = SaveSettingsAsync();
                
                _logger.LogInformation("Saved profile: {ProfileName}", profile.Name);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error saving profile");
            }
        }

        public ObfuscationProfile? LoadProfile(string name)
        {
            var profile = _settings.SavedProfiles.FirstOrDefault(p => p.Name == name);
            
            if (profile != null)
            {
                profile.LastUsedDate = DateTime.Now;
                _ = SaveSettingsAsync();
                _logger.LogInformation("Loaded profile: {ProfileName}", name);
            }

            return profile;
        }

        public void DeleteProfile(string name)
        {
            _settings.SavedProfiles.RemoveAll(p => p.Name == name);
            _ = SaveSettingsAsync();
            _logger.LogInformation("Deleted profile: {ProfileName}", name);
        }

        public List<ObfuscationProfile> GetAllProfiles()
        {
            return new List<ObfuscationProfile>(_settings.SavedProfiles);
        }

        public async Task ExportSettingsAsync(string filePath)
        {
            try
            {
                var json = JsonConvert.SerializeObject(_settings, Formatting.Indented);
                await File.WriteAllTextAsync(filePath, json);
                _logger.LogInformation("Settings exported to {FilePath}", filePath);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error exporting settings");
                throw;
            }
        }

        public async Task ImportSettingsAsync(string filePath)
        {
            try
            {
                var json = await File.ReadAllTextAsync(filePath);
                var importedSettings = JsonConvert.DeserializeObject<AppSettings>(json);
                
                if (importedSettings != null)
                {
                    _settings = importedSettings;
                    await SaveSettingsAsync();
                    _logger.LogInformation("Settings imported from {FilePath}", filePath);
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error importing settings");
                throw;
            }
        }

        public async Task ResetToDefaultsAsync()
        {
            _settings = new AppSettings();
            await SaveSettingsAsync();
            _logger.LogInformation("Settings reset to defaults");
        }
    }
}

