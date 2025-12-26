/**
 * LLVM IR Parser and Normalizer
 * Parses IR files and normalizes them for comparison
 */

/**
 * Normalize IR content for comparison
 */
export function normalizeIR(content) {
  if (!content) return '';

  // Remove comments
  let normalized = content.replace(/;.*$/gm, '');

  // Normalize whitespace
  normalized = normalized.replace(/\s+/g, ' ');

  // Remove leading/trailing whitespace from lines
  normalized = normalized.split('\n')
    .map(line => line.trim())
    .filter(line => line.length > 0)
    .join('\n');

  // Normalize variable names (replace with placeholders)
  normalized = normalized.replace(/%\d+/g, '%VAR');
  normalized = normalized.replace(/@\w+/g, '@SYM');

  // Normalize numbers (optional - may want to keep for some comparisons)
  // normalized = normalized.replace(/\b\d+\b/g, 'NUM');

  return normalized.trim();
}

/**
 * Extract functions from IR
 */
export function extractFunctions(content) {
  const functions = [];
  const functionRegex = /define\s+(?:[^@]*@)?(\w+)\s*\([^)]*\)/g;
  let match;

  while ((match = functionRegex.exec(content)) !== null) {
    functions.push(match[1]);
  }

  return functions;
}

/**
 * Extract basic blocks from IR
 */
export function extractBasicBlocks(content) {
  const blocks = [];
  const blockRegex = /^(\w+):\s*$/gm;
  let match;

  while ((match = blockRegex.exec(content)) !== null) {
    blocks.push(match[1]);
  }

  return blocks;
}

/**
 * Count instructions in IR
 */
export function countInstructions(content) {
  // Count lines that contain actual instructions (not labels, not empty)
  const lines = content.split('\n')
    .map(line => line.trim())
    .filter(line => {
      // Filter out comments, labels, and empty lines
      if (!line || line.startsWith(';') || line.endsWith(':')) {
        return false;
      }
      // Check if it looks like an instruction
      return /^\s*(?:%|@|call|ret|br|switch|invoke|unreachable)/.test(line);
    });

  return lines.length;
}

/**
 * Compare two IR files
 */
export function compareIR(ir1, ir2, options = {}) {
  const {
    normalize = true,
    compareFunctions = true,
    compareInstructions = true,
    tolerance = 0.1 // 10% difference allowed
  } = options;

  const result = {
    match: true,
    differences: [],
    metrics: {}
  };

  // Normalize if requested
  const normalized1 = normalize ? normalizeIR(ir1) : ir1;
  const normalized2 = normalize ? normalizeIR(ir2) : ir2;

  // Compare normalized content
  if (normalized1 !== normalized2) {
    result.match = false;
    result.differences.push('IR content differs after normalization');
  }

  // Compare functions
  if (compareFunctions) {
    const funcs1 = extractFunctions(ir1);
    const funcs2 = extractFunctions(ir2);

    if (funcs1.length !== funcs2.length) {
      result.match = false;
      result.differences.push(`Function count differs: ${funcs1.length} vs ${funcs2.length}`);
    }

    result.metrics.functions1 = funcs1.length;
    result.metrics.functions2 = funcs2.length;
  }

  // Compare instruction counts
  if (compareInstructions) {
    const inst1 = countInstructions(ir1);
    const inst2 = countInstructions(ir2);
    const diff = Math.abs(inst1 - inst2) / Math.max(inst1, inst2);

    if (diff > tolerance) {
      result.match = false;
      result.differences.push(`Instruction count differs significantly: ${inst1} vs ${inst2} (${(diff * 100).toFixed(1)}% difference)`);
    }

    result.metrics.instructions1 = inst1;
    result.metrics.instructions2 = inst2;
    result.metrics.instructionDiff = diff;
  }

  return result;
}


