import { join, dirname } from 'path';
import { fileURLToPath } from 'url';
import { existsSync } from 'fs';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

// Paths
export const PROJECT_ROOT = join(__dirname, '..');
export const EXAMPLES_DIR = join(PROJECT_ROOT, 'examples');
export const TEST_OUTPUT_DIR = join(__dirname, 'output');
export const CLI_OUTPUT_DIR = join(TEST_OUTPUT_DIR, 'cli');
export const WEBAPP_OUTPUT_DIR = join(TEST_OUTPUT_DIR, 'webapp');

// Test files
export const TEST_INPUT_FILE = join(EXAMPLES_DIR, 'simple_example.c');

// Tool paths
export const CLI_TOOL_PATH = process.env.OBFUSCATOR_PATH || join(PROJECT_ROOT, 'output', 'llvm-obfuscator.exe');
export const CLANG_PATH = process.env.CLANG_PATH || 'C:\\Program Files\\LLVM\\bin\\clang.exe';

// Web app configuration
export const WEBAPP_API_URL = process.env.API_URL || 'http://localhost:3001';
export const WEBAPP_TIMEOUT = 300000; // 5 minutes

// Test configurations for each technique
export const TECHNIQUE_CONFIGS = {
  controlFlow: {
    name: 'Control Flow Obfuscation',
    cliFlags: ['--cf'],
    webConfig: { enableControlFlowObfuscation: true }
  },
  stringEncryption: {
    name: 'String Encryption',
    cliFlags: ['--str'],
    webConfig: { enableStringEncryption: true }
  },
  bogusCode: {
    name: 'Bogus Code Insertion',
    cliFlags: ['--bogus', '--bogus-percent=30'],
    webConfig: { enableBogusCode: true, bogusCodePercentage: 30 }
  },
  fakeLoops: {
    name: 'Fake Loops',
    cliFlags: ['--loops', '--fake-loops=5'],
    webConfig: { enableFakeLoops: true, fakeLoopCount: 5 }
  },
  instructionSubstitution: {
    name: 'Instruction Substitution',
    cliFlags: ['--subs'],
    webConfig: { enableInstructionSubstitution: true }
  },
  controlFlowFlattening: {
    name: 'Control Flow Flattening',
    cliFlags: ['--flatten', '--flatten-prob=80'],
    webConfig: { enableControlFlowFlattening: true, flatteningProbability: 80 }
  },
  mba: {
    name: 'Mixed Boolean Arithmetic',
    cliFlags: ['--mba', '--mba-level=3'],
    webConfig: { enableMBA: true, mbaComplexity: 3 }
  },
  antiDebug: {
    name: 'Anti-Debugging',
    cliFlags: ['--anti-debug'],
    webConfig: { enableAntiDebug: true }
  },
  indirectCalls: {
    name: 'Indirect Calls',
    cliFlags: ['--indirect'],
    webConfig: { enableIndirectCalls: true }
  },
  constantObfuscation: {
    name: 'Constant Obfuscation',
    cliFlags: ['--const-obf'],
    webConfig: { enableConstantObfuscation: true }
  },
  antiTamper: {
    name: 'Anti-Tampering',
    cliFlags: ['--anti-tamper'],
    webConfig: { enableAntiTamper: true }
  },
  virtualization: {
    name: 'Virtualization',
    cliFlags: ['--virtualize', '--vm-level=2'],
    webConfig: { enableVirtualization: true, virtualizationLevel: 2 }
  },
  polymorphic: {
    name: 'Polymorphic Code',
    cliFlags: ['--polymorphic', '--poly-variants=5'],
    webConfig: { enablePolymorphic: true, polymorphicVariants: 5 }
  },
  antiAnalysis: {
    name: 'Anti-Analysis',
    cliFlags: ['--anti-analysis'],
    webConfig: { enableAntiAnalysis: true }
  },
  metamorphic: {
    name: 'Metamorphic',
    cliFlags: ['--metamorphic'],
    webConfig: { enableMetamorphic: true }
  },
  dynamic: {
    name: 'Dynamic Obfuscation',
    cliFlags: ['--dynamic'],
    webConfig: { enableDynamicObf: true }
  }
};

// Preset configurations
export const PRESET_CONFIGS = {
  basic: {
    name: 'Basic Preset',
    cliFlags: ['--cf'],
    webConfig: { preset: 'basic', options: {} }
  },
  medium: {
    name: 'Medium Preset',
    cliFlags: ['--cf', '--str', '--bogus', '--cycles=3'],
    webConfig: { preset: 'medium', options: {} }
  },
  heavy: {
    name: 'Heavy Preset',
    cliFlags: [
      '--cf', '--str', '--bogus', '--loops', '--subs', '--flatten',
      '--mba', '--anti-debug', '--indirect', '--const-obf', '--anti-tamper',
      '--virtualize', '--polymorphic', '--anti-analysis', '--metamorphic', '--dynamic',
      '--cycles=5', '--bogus-percent=50'
    ],
    webConfig: { preset: 'heavy', options: {} }
  }
};

// Validate configuration
export function validateConfig() {
  const errors = [];

  if (!existsSync(CLI_TOOL_PATH)) {
    errors.push(`CLI tool not found: ${CLI_TOOL_PATH}`);
  }

  if (!existsSync(TEST_INPUT_FILE)) {
    errors.push(`Test input file not found: ${TEST_INPUT_FILE}`);
  }

  if (!existsSync(CLANG_PATH)) {
    errors.push(`Clang not found: ${CLANG_PATH}. Set CLANG_PATH environment variable.`);
  }

  return errors;
}


