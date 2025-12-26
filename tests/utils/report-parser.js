/**
 * Obfuscation Report Parser
 * Parses and compares obfuscation reports
 */

/**
 * Parse obfuscation report
 */
export function parseReport(content) {
  if (!content) {
    return { metrics: {}, techniques: [] };
  }

  const report = {
    metrics: {},
    techniques: [],
    raw: content
  };

  // Extract metrics
  const metricPatterns = {
    bogusInstructions: /bogus.*instructions?[:\s]+(\d+)/i,
    fakeLoops: /fake.*loops?[:\s]+(\d+)/i,
    stringEncryptions: /string.*encryption[:\s]+(\d+)/i,
    obfuscationCycles: /cycles?[:\s]+(\d+)/i,
    instructionSubstitutions: /instruction.*substitution[:\s]+(\d+)/i,
    flattenedFunctions: /flattened.*functions?[:\s]+(\d+)/i,
    mbaTransformations: /mba.*transformation[:\s]+(\d+)/i,
    antiDebugChecks: /anti.*debug.*checks?[:\s]+(\d+)/i,
    indirectCalls: /indirect.*calls?[:\s]+(\d+)/i,
    obfuscatedConstants: /obfuscated.*constants?[:\s]+(\d+)/i,
    virtualizedFunctions: /virtualized.*functions?[:\s]+(\d+)/i,
    polymorphicVariants: /polymorphic.*variants?[:\s]+(\d+)/i,
    antiAnalysisChecks: /anti.*analysis.*checks?[:\s]+(\d+)/i,
    metamorphicTransformations: /metamorphic.*transformation[:\s]+(\d+)/i,
    dynamicObfuscations: /dynamic.*obfuscation[:\s]+(\d+)/i
  };

  for (const [key, pattern] of Object.entries(metricPatterns)) {
    const match = content.match(pattern);
    if (match) {
      report.metrics[key] = parseInt(match[1], 10);
    } else {
      report.metrics[key] = 0;
    }
  }

  // Extract enabled techniques
  const techniquePatterns = [
    /control.*flow.*obfuscation/i,
    /string.*encryption/i,
    /bogus.*code/i,
    /fake.*loops?/i,
    /instruction.*substitution/i,
    /control.*flow.*flattening/i,
    /mixed.*boolean.*arithmetic|mba/i,
    /anti.*debug/i,
    /indirect.*calls?/i,
    /constant.*obfuscation/i,
    /anti.*tamper/i,
    /virtualization/i,
    /polymorphic/i,
    /anti.*analysis/i,
    /metamorphic/i,
    /dynamic.*obfuscation/i
  ];

  for (const pattern of techniquePatterns) {
    if (pattern.test(content)) {
      report.techniques.push(pattern.source);
    }
  }

  return report;
}

/**
 * Compare two reports
 */
export function compareReports(report1, report2, options = {}) {
  const {
    tolerance = 0.1, // 10% difference allowed for metrics
    strict = false
  } = options;

  const result = {
    match: true,
    differences: [],
    metrics: {}
  };

  // Compare metrics
  const allMetrics = new Set([
    ...Object.keys(report1.metrics || {}),
    ...Object.keys(report2.metrics || {})
  ]);

  for (const metric of allMetrics) {
    const val1 = report1.metrics?.[metric] || 0;
    const val2 = report2.metrics?.[metric] || 0;

    if (strict) {
      if (val1 !== val2) {
        result.match = false;
        result.differences.push(`${metric}: ${val1} vs ${val2}`);
      }
    } else {
      // Allow tolerance for metrics
      const maxVal = Math.max(val1, val2, 1);
      const diff = Math.abs(val1 - val2) / maxVal;

      if (diff > tolerance && maxVal > 0) {
        result.match = false;
        result.differences.push(`${metric}: ${val1} vs ${val2} (${(diff * 100).toFixed(1)}% difference)`);
      }
    }

    result.metrics[metric] = { report1: val1, report2: val2 };
  }

  // Compare techniques (should match exactly)
  const tech1 = new Set(report1.techniques || []);
  const tech2 = new Set(report2.techniques || []);

  const onlyIn1 = [...tech1].filter(t => !tech2.has(t));
  const onlyIn2 = [...tech2].filter(t => !tech1.has(t));

  if (onlyIn1.length > 0 || onlyIn2.length > 0) {
    result.match = false;
    if (onlyIn1.length > 0) {
      result.differences.push(`Techniques only in report1: ${onlyIn1.join(', ')}`);
    }
    if (onlyIn2.length > 0) {
      result.differences.push(`Techniques only in report2: ${onlyIn2.join(', ')}`);
    }
  }

  return result;
}


