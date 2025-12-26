#!/usr/bin/env node

import { validateConfig, TEST_OUTPUT_DIR } from './config.js';
import { cleanDir } from './utils/file-utils.js';

// Import all technique tests
import { testControlFlow } from './techniques/test-control-flow.js';
import { testStringEncryption } from './techniques/test-string-encryption.js';
import { testBogusCode } from './techniques/test-bogus-code.js';
import { testFakeLoops } from './techniques/test-fake-loops.js';
import { testInstructionSubstitution } from './techniques/test-instruction-sub.js';
import { testFlattening } from './techniques/test-flattening.js';
import { testMBA } from './techniques/test-mba.js';
import { testAntiDebug } from './techniques/test-anti-debug.js';
import { testIndirectCalls } from './techniques/test-indirect-calls.js';
import { testConstantObfuscation } from './techniques/test-constant-obf.js';
import { testAntiTamper } from './techniques/test-anti-tamper.js';
import { testVirtualization } from './techniques/test-virtualization.js';
import { testPolymorphic } from './techniques/test-polymorphic.js';
import { testAntiAnalysis } from './techniques/test-anti-analysis.js';
import { testMetamorphic } from './techniques/test-metamorphic.js';
import { testDynamic } from './techniques/test-dynamic.js';

// Import preset tests
import { testBasicPreset } from './presets/test-basic-preset.js';
import { testMediumPreset } from './presets/test-medium-preset.js';
import { testHeavyPreset } from './presets/test-heavy-preset.js';

// Import pipeline tests
import { testFullPipeline } from './pipeline/test-full-pipeline.js';
import { testExceptionHandling } from './pipeline/test-exception-handling.js';

// All tests
const techniqueTests = [
  testControlFlow,
  testStringEncryption,
  testBogusCode,
  testFakeLoops,
  testInstructionSubstitution,
  testFlattening,
  testMBA,
  testAntiDebug,
  testIndirectCalls,
  testConstantObfuscation,
  testAntiTamper,
  testVirtualization,
  testPolymorphic,
  testAntiAnalysis,
  testMetamorphic,
  testDynamic
];

const presetTests = [
  testBasicPreset,
  testMediumPreset,
  testHeavyPreset
];

const pipelineTests = [
  testFullPipeline,
  testExceptionHandling
];

/**
 * Run a single test
 */
async function runTest(testFn, category) {
  try {
    const result = await testFn();
    return {
      ...result,
      category
    };
  } catch (error) {
    return {
      name: testFn.name || 'Unknown',
      success: false,
      error: error.message,
      category
    };
  }
}

/**
 * Print test result
 */
function printResult(result, index, total) {
  const status = result.success ? '✓' : '✗';
  const color = result.success ? '\x1b[32m' : '\x1b[31m';
  const reset = '\x1b[0m';
  
  console.log(`${color}${status}${reset} [${index + 1}/${total}] ${result.name}`);
  
  if (!result.success) {
    if (result.error) {
      console.log(`  Error: ${result.error}`);
    }
    if (result.comparison) {
      if (result.comparison.ir && result.comparison.ir.metrics && result.comparison.ir.metrics.instructionDiff > 0.8) {
        console.log(`  IR differences: ${result.comparison.ir.differences?.join(', ')}`);
      }
      if (result.comparison.report && !result.comparison.report.match) {
        // Only show significant report differences
        const significantDiffs = result.comparison.report.differences?.filter(d => 
          !d.includes('Techniques only in') || d.includes('100.0%')
        );
        if (significantDiffs && significantDiffs.length > 0) {
          console.log(`  Report differences: ${significantDiffs.join(', ')}`);
        }
      }
      if (result.comparison.executable && result.comparison.executable.error) {
        console.log(`  Executable error: ${result.comparison.executable.error}`);
      }
    }
  }
}

/**
 * Main test runner
 */
async function main() {
  console.log('='.repeat(80));
  console.log('CLI vs Web App Verification Test Suite');
  console.log('='.repeat(80));
  console.log('');

  // Validate configuration
  console.log('Validating configuration...');
  const configErrors = validateConfig();
  if (configErrors.length > 0) {
    console.error('Configuration errors:');
    configErrors.forEach(error => console.error(`  - ${error}`));
    process.exit(1);
  }
  console.log('Configuration valid.\n');

  // Clean output directory
  console.log('Cleaning test output directory...');
  await cleanDir(TEST_OUTPUT_DIR);
  console.log('');

  const results = {
    techniques: [],
    presets: [],
    pipeline: [],
    summary: {
      total: 0,
      passed: 0,
      failed: 0
    }
  };

  // Run technique tests
  console.log('Running technique tests...');
  console.log('-'.repeat(80));
  for (let i = 0; i < techniqueTests.length; i++) {
    const result = await runTest(techniqueTests[i], 'technique');
    results.techniques.push(result);
    printResult(result, i, techniqueTests.length);
    results.summary.total++;
    if (result.success) {
      results.summary.passed++;
    } else {
      results.summary.failed++;
    }
  }
  console.log('');

  // Run preset tests
  console.log('Running preset tests...');
  console.log('-'.repeat(80));
  for (let i = 0; i < presetTests.length; i++) {
    const result = await runTest(presetTests[i], 'preset');
    results.presets.push(result);
    printResult(result, i, presetTests.length);
    results.summary.total++;
    if (result.success) {
      results.summary.passed++;
    } else {
      results.summary.failed++;
    }
  }
  console.log('');

  // Run pipeline tests
  console.log('Running pipeline tests...');
  console.log('-'.repeat(80));
  for (let i = 0; i < pipelineTests.length; i++) {
    const result = await runTest(pipelineTests[i], 'pipeline');
    results.pipeline.push(result);
    printResult(result, i, pipelineTests.length);
    results.summary.total++;
    if (result.success) {
      results.summary.passed++;
    } else {
      results.summary.failed++;
    }
  }
  console.log('');

  // Print summary
  console.log('='.repeat(80));
  console.log('Test Summary');
  console.log('='.repeat(80));
  console.log(`Total tests: ${results.summary.total}`);
  console.log(`Passed: ${results.summary.passed}`);
  console.log(`Failed: ${results.summary.failed}`);
  console.log(`Success rate: ${((results.summary.passed / results.summary.total) * 100).toFixed(1)}%`);
  console.log('');

  // Detailed breakdown
  console.log('Breakdown:');
  console.log(`  Techniques: ${results.techniques.filter(r => r.success).length}/${results.techniques.length} passed`);
  console.log(`  Presets: ${results.presets.filter(r => r.success).length}/${results.presets.length} passed`);
  console.log(`  Pipeline: ${results.pipeline.filter(r => r.success).length}/${results.pipeline.length} passed`);
  console.log('');

  // Exit with appropriate code
  if (results.summary.failed > 0) {
    console.log('Some tests failed. Check the output above for details.');
    process.exit(1);
  } else {
    console.log('All tests passed!');
    process.exit(0);
  }
}

// Run tests
main().catch(error => {
  console.error('Fatal error:', error);
  process.exit(1);
});

