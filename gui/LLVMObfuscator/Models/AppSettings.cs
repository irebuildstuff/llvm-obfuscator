using System.Collections.Generic;

namespace LLVMObfuscatorAvalonia.Models
{
    /// <summary>
    /// Application-wide settings and configuration
    /// </summary>
    public class AppSettings
    {
        public ApplicationSettings Application { get; set; } = new();
        public ObfuscationSettings Obfuscation { get; set; } = new();
        public UISettings UI { get; set; } = new();
        public List<string> RecentFiles { get; set; } = new();
        public List<ObfuscationProfile> SavedProfiles { get; set; } = new();
        public Dictionary<string, string> UserPreferences { get; set; } = new();
    }

    /// <summary>
    /// General application settings
    /// </summary>
    public class ApplicationSettings
    {
        public string Version { get; set; } = "4.0.0";
        public string Name { get; set; } = "LLVM Code Obfuscator";
        public int MaxRecentFiles { get; set; } = 10;
        public bool AutoSave { get; set; } = true;
        public int AutoSaveInterval { get; set; } = 300;
        public bool CheckForUpdates { get; set; } = true;
        public string Theme { get; set; } = "Light";
        public string Language { get; set; } = "en-US";
    }

    /// <summary>
    /// Obfuscation-specific settings
    /// </summary>
    public class ObfuscationSettings
    {
        public string DefaultOutputSuffix { get; set; } = "_obfuscated";
        public string DefaultReportSuffix { get; set; } = "_report";
        public long MaxFileSize { get; set; } = 52428800; // 50MB
        public int Timeout { get; set; } = 300; // seconds
        public string DefaultPreset { get; set; } = "Standard";
        public bool ShowAdvancedOptions { get; set; } = false;
        public bool ConfirmBeforeObfuscation { get; set; } = true;
    }

    /// <summary>
    /// UI-specific settings
    /// </summary>
    public class UISettings
    {
        public int WindowWidth { get; set; } = 1400;
        public int WindowHeight { get; set; } = 900;
        public int MinWindowWidth { get; set; } = 1200;
        public int MinWindowHeight { get; set; } = 800;
        public bool EnableAnimations { get; set; } = true;
        public bool ShowTooltips { get; set; } = true;
        public double WindowLeft { get; set; } = -1;
        public double WindowTop { get; set; } = -1;
        public bool IsMaximized { get; set; } = false;
    }

    /// <summary>
    /// Saved obfuscation profile
    /// </summary>
    public class ObfuscationProfile
    {
        public string Name { get; set; } = "";
        public string Description { get; set; } = "";
        public bool ControlFlow { get; set; }
        public bool StringEncryption { get; set; }
        public bool BogusCode { get; set; }
        public bool FakeLoops { get; set; }
        public bool InstructionSubstitution { get; set; }
        public bool ControlFlowFlattening { get; set; }
        public bool MixedBooleanArithmetic { get; set; }
        public bool AntiDebug { get; set; }
        public bool IndirectCalls { get; set; }
        public bool ConstantObfuscation { get; set; }
        public bool AntiTamper { get; set; }
        public bool Virtualization { get; set; }
        public bool Polymorphic { get; set; }
        public bool AntiAnalysis { get; set; }
        public bool Metamorphic { get; set; }
        public bool DynamicObfuscation { get; set; }
        public bool IsCustom { get; set; }
        public System.DateTime CreatedDate { get; set; }
        public System.DateTime LastUsedDate { get; set; }
    }
}

