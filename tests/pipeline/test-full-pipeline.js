import { runCLIPipeline } from '../cli-runner.js';
import { runWebAppPipeline } from '../webapp-runner.js';
import { compareAllOutputs } from '../comparison.js';
import { readFileContent, fileExists } from '../utils/file-utils.js';
import { exec } from 'child_process';
import { promisify } from 'util';

const execAsync = promisify(exec);

export async function testFullPipeline() {
  console.log('Testing: Full Pipeline (C/C++ → IR → Obfuscated IR → Executable)');

  try {
    // Use medium preset for full pipeline test
    const cliFlags = ['--cf', '--str', '--bogus', '--cycles=3'];
    const webConfig = { preset: 'medium', options: {} };

    // Run CLI pipeline
    const cliPaths = await runCLIPipeline('full-pipeline', cliFlags);

    // Run Web App pipeline
    const { paths: webappPaths } = await runWebAppPipeline('full-pipeline', webConfig);

    // Compare all outputs
    const comparison = await compareAllOutputs(cliPaths, webappPaths);

    // Verify executables run and produce same output
    let executableMatch = false;
    if (fileExists(cliPaths.executable) && fileExists(webappPaths.executable)) {
      try {
        const [cliOutput, webappOutput] = await Promise.all([
          execAsync(`"${cliPaths.executable}"`).catch(e => ({ stdout: '', stderr: e.message })),
          execAsync(`"${webappPaths.executable}"`).catch(e => ({ stdout: '', stderr: e.message }))
        ]);

        executableMatch = (cliOutput.stdout || '').trim() === (webappOutput.stdout || '').trim();
      } catch (error) {
        console.warn('Failed to run executables:', error.message);
      }
    }

    return {
      name: 'Full Pipeline',
      success: comparison.overall && executableMatch,
      comparison,
      executableMatch,
      cliPaths,
      webappPaths
    };
  } catch (error) {
    return {
      name: 'Full Pipeline',
      success: false,
      error: error.message
    };
  }
}


