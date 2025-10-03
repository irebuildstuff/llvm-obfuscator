using System.IO;
using FluentValidation;
using LLVMObfuscatorAvalonia.Services;

namespace LLVMObfuscatorAvalonia.Validators
{
    /// <summary>
    /// Validator for obfuscation options
    /// </summary>
    public class ObfuscationOptionsValidator : AbstractValidator<ObfuscationOptions>
    {
        public ObfuscationOptionsValidator()
        {
            // Input file validation
            RuleFor(x => x.InputFile)
                .NotEmpty()
                .WithMessage("Input file is required");

            RuleFor(x => x.InputFile)
                .Must(File.Exists)
                .When(x => !string.IsNullOrEmpty(x.InputFile))
                .WithMessage("Input file does not exist");

            RuleFor(x => x.InputFile)
                .Must(BeValidFileExtension)
                .When(x => !string.IsNullOrEmpty(x.InputFile) && File.Exists(x.InputFile))
                .WithMessage("Input file must be .ll or .bc file");

            RuleFor(x => x.InputFile)
                .Must(BeValidFileSize)
                .When(x => !string.IsNullOrEmpty(x.InputFile) && File.Exists(x.InputFile))
                .WithMessage("Input file is too large (max 50MB)");

            // Output file validation
            RuleFor(x => x.OutputFile)
                .NotEmpty()
                .WithMessage("Output file is required");

            RuleFor(x => x.OutputFile)
                .Must(BeValidOutputPath)
                .When(x => !string.IsNullOrEmpty(x.OutputFile))
                .WithMessage("Output file path is invalid");

            // Report file validation
            RuleFor(x => x.ReportFile)
                .NotEmpty()
                .WithMessage("Report file is required");

            // Advanced options validation
            RuleFor(x => x.ObfuscationCycles)
                .GreaterThanOrEqualTo(0)
                .LessThanOrEqualTo(10)
                .WithMessage("Obfuscation cycles must be between 0 and 10");

            RuleFor(x => x.MBAComplexity)
                .GreaterThanOrEqualTo(0)
                .LessThanOrEqualTo(5)
                .WithMessage("MBA complexity must be between 0 and 5");

            RuleFor(x => x.FlattenProbability)
                .GreaterThanOrEqualTo(0)
                .LessThanOrEqualTo(100)
                .WithMessage("Flatten probability must be between 0 and 100");

            RuleFor(x => x.VirtualizationLevel)
                .GreaterThanOrEqualTo(0)
                .LessThanOrEqualTo(3)
                .WithMessage("Virtualization level must be between 0 and 3");

            RuleFor(x => x.PolymorphicVariants)
                .GreaterThanOrEqualTo(0)
                .LessThanOrEqualTo(10)
                .WithMessage("Polymorphic variants must be between 0 and 10");

            RuleFor(x => x.BogusCodePercentage)
                .GreaterThanOrEqualTo(0)
                .LessThanOrEqualTo(100)
                .WithMessage("Bogus code percentage must be between 0 and 100");

            RuleFor(x => x.FakeLoopCount)
                .GreaterThanOrEqualTo(0)
                .LessThanOrEqualTo(20)
                .WithMessage("Fake loop count must be between 0 and 20");

            // At least one technique must be selected
            RuleFor(x => x)
                .Must(HaveAtLeastOneTechnique)
                .WithMessage("At least one obfuscation technique must be enabled");
        }

        private bool BeValidFileExtension(string filePath)
        {
            var extension = Path.GetExtension(filePath).ToLowerInvariant();
            return extension == ".ll" || extension == ".bc";
        }

        private bool BeValidFileSize(string filePath)
        {
            var fileInfo = new FileInfo(filePath);
            const long maxSize = 50 * 1024 * 1024; // 50MB
            return fileInfo.Length <= maxSize;
        }

        private bool BeValidOutputPath(string outputPath)
        {
            try
            {
                var directory = Path.GetDirectoryName(outputPath);
                return string.IsNullOrEmpty(directory) || Directory.Exists(directory);
            }
            catch
            {
                return false;
            }
        }

        private bool HaveAtLeastOneTechnique(ObfuscationOptions options)
        {
            return options.ControlFlow ||
                   options.StringEncryption ||
                   options.BogusCode ||
                   options.FakeLoops ||
                   options.InstructionSubstitution ||
                   options.ControlFlowFlattening ||
                   options.MixedBooleanArithmetic ||
                   options.AntiDebug ||
                   options.IndirectCalls ||
                   options.ConstantObfuscation ||
                   options.AntiTamper ||
                   options.Virtualization ||
                   options.Polymorphic ||
                   options.AntiAnalysis ||
                   options.Metamorphic ||
                   options.DynamicObfuscation;
        }
    }
}

