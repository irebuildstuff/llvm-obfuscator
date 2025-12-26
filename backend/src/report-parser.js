import { readFile } from 'fs/promises';
import { existsSync } from 'fs';

/**
 * Parse obfuscation report file
 */
export async function parseReport(reportPath) {
  if (!existsSync(reportPath)) {
    return null;
  }

  const content = await readFile(reportPath, 'utf-8');
  
  return {
    raw: content,
    parsed: parseReportContent(content)
  };
}

/**
 * Parse report content into structured data
 */
function parseReportContent(content) {
  const parsed = {
    configuration: {},
    metrics: {},
    summary: {}
  };

  // Extract configuration
  const configMatch = content.match(/Configuration:[\s\S]*?(?=\n\n|\n===|$)/);
  if (configMatch) {
    parsed.configuration = parseConfiguration(configMatch[0]);
  }

  // Extract metrics
  parsed.metrics = extractMetrics(content);

  // Extract summary
  parsed.summary = extractSummary(content, parsed.metrics);

  return parsed;
}

/**
 * Parse configuration section
 */
function parseConfiguration(configText) {
  const config = {};
  
  // Extract enabled/disabled flags
  const flags = {
    'Control Flow Obfuscation': 'enableControlFlowObfuscation',
    'String Encryption': 'enableStringEncryption',
    'Bogus Code': 'enableBogusCode',
    'Fake Loops': 'enableFakeLoops',
    'Instruction Substitution': 'enableInstructionSubstitution',
    'Control Flow Flattening': 'enableControlFlowFlattening',
    'Mixed Boolean Arithmetic': 'enableMBA',
    'Anti-Debugging': 'enableAntiDebug',
    'Indirect Calls': 'enableIndirectCalls',
    'Constant Obfuscation': 'enableConstantObfuscation',
    'Anti-Tampering': 'enableAntiTamper',
    'Code Virtualization': 'enableVirtualization',
    'Polymorphic Code': 'enablePolymorphic',
    'Anti-Analysis': 'enableAntiAnalysis',
    'Metamorphic Transforms': 'enableMetamorphic',
    'Dynamic Obfuscation': 'enableDynamicObf'
  };

  for (const [label, key] of Object.entries(flags)) {
    const regex = new RegExp(`${label}:\\s*(Enabled|Disabled)`, 'i');
    const match = configText.match(regex);
    if (match) {
      config[key] = match[1].toLowerCase() === 'enabled';
    }
  }

  // Extract numeric values
  const cyclesMatch = configText.match(/Obfuscation Cycles:\s*(\d+)/i);
  if (cyclesMatch) {
    config.obfuscationCycles = parseInt(cyclesMatch[1]);
  }

  const bogusMatch = configText.match(/Bogus Code Percentage:\s*(\d+)/i);
  if (bogusMatch) {
    config.bogusCodePercentage = parseInt(bogusMatch[1]);
  }

  return config;
}

/**
 * Extract metrics from report
 */
function extractMetrics(content) {
  const metrics = {};

  // Common metric patterns
  const metricPatterns = {
    obfuscationCycles: /(?:Total\s+)?Obfuscation\s+cycles?:\s*(\d+)/i,
    bogusInstructions: /(?:Total\s+)?Bogus\s+instructions?:\s*(\d+)/i,
    fakeLoops: /(?:Total\s+)?Fake\s+loops?:\s*(\d+)/i,
    stringEncryptions: /(?:Total\s+)?String\s+encryptions?:\s*(\d+)/i,
    instructionSubstitutions: /(?:Total\s+)?Instruction\s+substitutions?:\s*(\d+)/i,
    flattenedFunctions: /(?:Total\s+)?Flattened\s+functions?:\s*(\d+)/i,
    mbaTransformations: /(?:Total\s+)?MBA\s+transformations?:\s*(\d+)/i,
    antiDebugChecks: /(?:Total\s+)?Anti-debug\s+checks?:\s*(\d+)/i,
    virtualizedFunctions: /(?:Total\s+)?Virtualized\s+functions?:\s*(\d+)/i,
    polymorphicVariants: /(?:Total\s+)?Polymorphic\s+variants?:\s*(\d+)/i,
    antiAnalysisChecks: /(?:Total\s+)?Anti-analysis\s+checks?:\s*(\d+)/i,
    metamorphicTransformations: /(?:Total\s+)?Metamorphic\s+transformations?:\s*(\d+)/i,
    dynamicObfuscations: /(?:Total\s+)?Dynamic\s+obfuscations?:\s*(\d+)/i
  };

  for (const [key, pattern] of Object.entries(metricPatterns)) {
    const match = content.match(pattern);
    if (match) {
      metrics[key] = parseInt(match[1]) || 0;
    } else {
      metrics[key] = 0;
    }
  }

  return metrics;
}

/**
 * Extract summary information
 */
function extractSummary(content, metrics) {
  const totalTransformations = Object.values(metrics).reduce((sum, val) => sum + (val || 0), 0);

  // Estimate overhead based on transformations
  let estimatedOverhead = '5-10%';
  if (totalTransformations > 500) {
    estimatedOverhead = '50-200%';
  } else if (totalTransformations > 200) {
    estimatedOverhead = '20-40%';
  } else if (totalTransformations > 100) {
    estimatedOverhead = '15-25%';
  }

  // Determine security level
  let securityLevel = 'Low';
  const advancedTechniques = (metrics.virtualizedFunctions || 0) + 
                             (metrics.polymorphicVariants || 0) + 
                             (metrics.metamorphicTransformations || 0);
  
  if (advancedTechniques > 0 || totalTransformations > 300) {
    securityLevel = 'Very High';
  } else if (totalTransformations > 150) {
    securityLevel = 'High';
  } else if (totalTransformations > 50) {
    securityLevel = 'Medium';
  }

  return {
    totalTransformations,
    estimatedOverhead,
    securityLevel
  };
}


