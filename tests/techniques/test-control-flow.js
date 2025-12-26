import { runCLIPipeline } from '../cli-runner.js';
import { runWebAppPipeline } from '../webapp-runner.js';
import { compareAllOutputs } from '../comparison.js';
import { TECHNIQUE_CONFIGS } from '../config.js';

const config = TECHNIQUE_CONFIGS.controlFlow;

export async function testControlFlow() {
  console.log(`Testing: ${config.name}`);

  try {
    // Run CLI
    const cliPaths = await runCLIPipeline('control-flow', config.cliFlags);

    // Run Web App
    const { paths: webappPaths } = await runWebAppPipeline('control-flow', config.webConfig);

    // Compare outputs
    const comparison = await compareAllOutputs(cliPaths, webappPaths);

    return {
      name: config.name,
      success: comparison.overall,
      comparison,
      cliPaths,
      webappPaths
    };
  } catch (error) {
    return {
      name: config.name,
      success: false,
      error: error.message
    };
  }
}


