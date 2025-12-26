import { runCLIPipeline } from '../cli-runner.js';
import { runWebAppPipeline } from '../webapp-runner.js';
import { compareAllOutputs } from '../comparison.js';
import { PRESET_CONFIGS } from '../config.js';

const config = PRESET_CONFIGS.basic;

export async function testBasicPreset() {
  console.log(`Testing: ${config.name}`);

  try {
    const cliPaths = await runCLIPipeline('basic-preset', config.cliFlags);
    const { paths: webappPaths } = await runWebAppPipeline('basic-preset', config.webConfig);
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


