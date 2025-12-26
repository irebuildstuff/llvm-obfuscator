import { exec } from 'child_process';
import { promisify } from 'util';
import { join } from 'path';
import { existsSync } from 'fs';
import { CLI_TOOL_PATH, CLANG_PATH, TEST_INPUT_FILE, CLI_OUTPUT_DIR } from './config.js';
import { ensureDir, getTestPaths } from './utils/file-utils.js';

const execAsync = promisify(exec);

/**
 * Compile C/C++ source to LLVM IR using clang
 */
export async function compileToIR(sourcePath, outputIRPath) {
  const isWindows = process.platform === 'win32';
  const command = isWindows
    ? `cmd /c ""${CLANG_PATH}" -S -emit-llvm -fno-exceptions "${sourcePath}" -o "${outputIRPath}""`
    : `"${CLANG_PATH}" -S -emit-llvm -fno-exceptions "${sourcePath}" -o "${outputIRPath}"`;

  try {
    const { stdout, stderr } = await execAsync(command);
    if (stderr && !stderr.includes('warning') && !stderr.includes('note')) {
      throw new Error(`Clang compilation error: ${stderr}`);
    }
    return true;
  } catch (error) {
    throw new Error(`Failed to compile to IR: ${error.message}`);
  }
}

/**
 * Run CLI obfuscator with specific flags
 */
export async function runCLIObfuscator(inputIRPath, outputIRPath, reportPath, flags = []) {
  if (!existsSync(CLI_TOOL_PATH)) {
    throw new Error(`CLI tool not found: ${CLI_TOOL_PATH}`);
  }

  if (!existsSync(inputIRPath)) {
    throw new Error(`Input IR file not found: ${inputIRPath}`);
  }

  const isWindows = process.platform === 'win32';
  const flagsStr = flags.join(' ');
  const command = isWindows
    ? `cmd /c ""${CLI_TOOL_PATH}" "${inputIRPath}" -o "${outputIRPath}" --report="${reportPath}" ${flagsStr}"`
    : `"${CLI_TOOL_PATH}" "${inputIRPath}" -o "${outputIRPath}" --report="${reportPath}" ${flags.join(' ')}`;

  try {
    const { stdout, stderr } = await execAsync(command, {
      maxBuffer: 10 * 1024 * 1024 // 10MB
    });

    if (stderr && !stderr.includes('warning')) {
      console.warn('CLI stderr:', stderr);
    }

    return { stdout, stderr };
  } catch (error) {
    throw new Error(`CLI obfuscation failed: ${error.message}`);
  }
}

/**
 * Compile obfuscated IR to executable
 */
export async function compileIRToExecutable(irPath, executablePath) {
  const isWindows = process.platform === 'win32';
  const command = isWindows
    ? `cmd /c ""${CLANG_PATH}" "${irPath}" -o "${executablePath}" -fno-exceptions -lstdc++ -luser32 -lkernel32 -lntdll"`
    : `"${CLANG_PATH}" "${irPath}" -o "${executablePath}" -fno-exceptions -lstdc++`;

  try {
    const { stdout, stderr } = await execAsync(command);
    if (stderr && !stderr.includes('warning') && !stderr.includes('exception')) {
      throw new Error(`Clang linking error: ${stderr}`);
    }
    return true;
  } catch (error) {
    throw new Error(`Failed to compile to executable: ${error.message}`);
  }
}

/**
 * Run complete CLI obfuscation pipeline
 */
export async function runCLIPipeline(testName, flags = []) {
  const outputDir = join(CLI_OUTPUT_DIR, testName);
  await ensureDir(outputDir);

  const paths = getTestPaths(testName, outputDir);

  // Step 1: Compile source to IR
  await compileToIR(TEST_INPUT_FILE, paths.ir);

  // Step 2: Run obfuscator
  await runCLIObfuscator(paths.ir, paths.obfuscatedIr, paths.report, flags);

  // Step 3: Compile to executable
  try {
    await compileIRToExecutable(paths.obfuscatedIr, paths.executable);
  } catch (error) {
    console.warn(`Failed to compile executable (may be expected): ${error.message}`);
  }

  return paths;
}


