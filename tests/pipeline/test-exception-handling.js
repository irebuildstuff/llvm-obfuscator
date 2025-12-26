import { runCLIPipeline } from '../cli-runner.js';
import { runWebAppPipeline } from '../webapp-runner.js';
import { compareAllOutputs } from '../comparison.js';
import { fileExists } from '../utils/file-utils.js';

export async function testExceptionHandling() {
  console.log('Testing: Exception Handling');

  try {
    // Test with -fno-exceptions flag (both CLI and Web App should handle this)
    // Use basic preset for both to ensure same configuration
    const cliFlags = ['--cf'];
    const webConfig = { preset: 'basic', options: {} };

    // Run CLI pipeline
    const cliPaths = await runCLIPipeline('exception-handling', cliFlags);

    // Run Web App pipeline
    const { paths: webappPaths } = await runWebAppPipeline('exception-handling', webConfig);

    // Compare outputs
    const comparison = await compareAllOutputs(cliPaths, webappPaths);

    // Verify executables were created (exception handling should not prevent compilation)
    const cliExeExists = fileExists(cliPaths.executable);
    const webappExeExists = fileExists(webappPaths.executable);

    return {
      name: 'Exception Handling',
      success: comparison.overall && cliExeExists && webappExeExists,
      comparison,
      cliExecutableCreated: cliExeExists,
      webappExecutableCreated: webappExeExists,
      cliPaths,
      webappPaths
    };
  } catch (error) {
    return {
      name: 'Exception Handling',
      success: false,
      error: error.message
    };
  }
}


