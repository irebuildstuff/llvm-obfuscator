import { runCLIPipeline } from '../cli-runner.js';
import { runWebAppPipeline } from '../webapp-runner.js';
import { compareAllOutputs } from '../comparison.js';
import { TECHNIQUE_CONFIGS } from '../config.js';

const config = TECHNIQUE_CONFIGS.instructionSubstitution;

export async function testInstructionSubstitution() {
  console.log(`Testing: ${config.name}`);

  try {
    const cliPaths = await runCLIPipeline('instruction-sub', config.cliFlags);
    const { paths: webappPaths } = await runWebAppPipeline('instruction-sub', config.webConfig);
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


