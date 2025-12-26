import { runCLIPipeline } from '../cli-runner.js';
import { runWebAppPipeline } from '../webapp-runner.js';
import { compareAllOutputs } from '../comparison.js';
import { TECHNIQUE_CONFIGS } from '../config.js';

const config = TECHNIQUE_CONFIGS.constantObfuscation;

export async function testConstantObfuscation() {
  console.log(`Testing: ${config.name}`);

  try {
    const cliPaths = await runCLIPipeline('constant-obf', config.cliFlags);
    const { paths: webappPaths } = await runWebAppPipeline('constant-obf', config.webConfig);
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


