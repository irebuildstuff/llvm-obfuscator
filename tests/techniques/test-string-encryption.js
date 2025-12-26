import { runCLIPipeline } from '../cli-runner.js';
import { runWebAppPipeline } from '../webapp-runner.js';
import { compareAllOutputs } from '../comparison.js';
import { TECHNIQUE_CONFIGS } from '../config.js';

const config = TECHNIQUE_CONFIGS.stringEncryption;

export async function testStringEncryption() {
  console.log(`Testing: ${config.name}`);

  try {
    const cliPaths = await runCLIPipeline('string-encryption', config.cliFlags);
    const { paths: webappPaths } = await runWebAppPipeline('string-encryption', config.webConfig);
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


