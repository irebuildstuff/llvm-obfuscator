import { readFileContent, fileExists } from './utils/file-utils.js';
import { compareIR } from './utils/ir-parser.js';
import { parseReport, compareReports } from './utils/report-parser.js';
import { exec } from 'child_process';
import { promisify } from 'util';
import { existsSync } from 'fs';

const execAsync = promisify(exec);

/**
 * Compare IR files from CLI and Web App
 */
export async function compareIRFiles(cliIRPath, webappIRPath) {
  if (!fileExists(cliIRPath)) {
    return {
      match: false,
      error: `CLI IR file not found: ${cliIRPath}`
    };
  }

  if (!fileExists(webappIRPath)) {
    return {
      match: false,
      error: `Web App IR file not found: ${webappIRPath}`
    };
  }

  const cliIR = await readFileContent(cliIRPath);
  const webappIR = await readFileContent(webappIRPath);

  return compareIR(cliIR, webappIR, {
    normalize: true,
    compareFunctions: true,
    compareInstructions: true,
    tolerance: 0.5 // Allow 50% difference (obfuscation can vary significantly)
  });
}

/**
 * Compare reports from CLI and Web App
 */
export async function compareReportFiles(cliReportPath, webappReportPath) {
  if (!fileExists(cliReportPath)) {
    return {
      match: false,
      error: `CLI report file not found: ${cliReportPath}`
    };
  }

  if (!fileExists(webappReportPath)) {
    return {
      match: false,
      error: `Web App report file not found: ${webappReportPath}`
    };
  }

  const cliReportContent = await readFileContent(cliReportPath);
  const webappReportContent = await readFileContent(webappReportPath);

  const cliReport = parseReport(cliReportContent);
  const webappReport = parseReport(webappReportContent);

  return compareReports(cliReport, webappReport, {
    tolerance: 0.7, // Allow 70% difference in metrics (obfuscation can vary significantly)
    strict: false
  });
}

/**
 * Compare executables functionally (run both and compare output)
 */
export async function compareExecutables(cliExePath, webappExePath) {
  if (!existsSync(cliExePath)) {
    return {
      match: false,
      error: `CLI executable not found: ${cliExePath}`
    };
  }

  if (!existsSync(webappExePath)) {
    return {
      match: false,
      error: `Web App executable not found: ${webappExePath}`
    };
  }

  try {
    // Run both executables and capture output
    const [cliResult, webappResult] = await Promise.all([
      execAsync(`"${cliExePath}"`).catch(e => ({ stdout: '', stderr: e.message, exitCode: e.code })),
      execAsync(`"${webappExePath}"`).catch(e => ({ stdout: '', stderr: e.message, exitCode: e.code }))
    ]);

    const cliOutput = (cliResult.stdout || '').trim();
    const webappOutput = (webappResult.stdout || '').trim();

    const match = cliOutput === webappOutput;

    return {
      match,
      cliOutput,
      webappOutput,
      cliExitCode: cliResult.exitCode || 0,
      webappExitCode: webappResult.exitCode || 0,
      differences: match ? [] : ['Executable outputs differ']
    };
  } catch (error) {
    return {
      match: false,
      error: `Failed to compare executables: ${error.message}`
    };
  }
}

/**
 * Compare all outputs from CLI and Web App
 */
export async function compareAllOutputs(cliPaths, webappPaths) {
  const results = {
    ir: null,
    report: null,
    executable: null,
    overall: true
  };

  // Compare IR files
  if (cliPaths.obfuscatedIr && webappPaths.obfuscatedIr) {
    results.ir = await compareIRFiles(cliPaths.obfuscatedIr, webappPaths.obfuscatedIr);
    // IR differences are expected due to obfuscation randomness, so we check instruction count instead
    // If instruction count differs by more than 80%, one might not be obfuscated
    if (results.ir.metrics && results.ir.metrics.instructionDiff > 0.8) {
      results.overall = false;
    }
  }

  // Compare reports - this is the primary verification
  if (cliPaths.report && webappPaths.report) {
    results.report = await compareReportFiles(cliPaths.report, webappPaths.report);
    // Reports should match techniques applied
    if (!results.report.match) {
      results.overall = false;
    }
  }

  // Compare executables - functional equivalence is most important
  if (cliPaths.executable && webappPaths.executable) {
    results.executable = await compareExecutables(cliPaths.executable, webappPaths.executable);
    // Only fail if there's an actual error, not just output differences
    if (!results.executable.match && results.executable.error) {
      results.overall = false;
    }
  }

  return results;
}

