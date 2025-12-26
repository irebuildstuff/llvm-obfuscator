import { exec } from 'child_process';
import { promisify } from 'util';
import { join, dirname, basename } from 'path';
import { fileURLToPath } from 'url';
import { existsSync, mkdirSync } from 'fs';
import { createJob, updateJob } from './job-manager.js';
import { parseReport } from './report-parser.js';

const execAsync = promisify(exec);
const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const UPLOADS_DIR = join(__dirname, '../uploads');
const OUTPUT_DIR = join(__dirname, '../output');
const OBFUSCATOR_PATH = process.env.OBFUSCATOR_PATH || join(__dirname, '../../output/llvm-obfuscator.exe');

/**
 * Find clang executable in common locations
 */
function findClang() {
  // Check environment variable first
  if (process.env.CLANG_PATH && existsSync(process.env.CLANG_PATH)) {
    return process.env.CLANG_PATH;
  }

  const isWindows = process.platform === 'win32';
  
  if (isWindows) {
    // Common Windows paths
    const possiblePaths = [
      'C:\\Program Files\\LLVM\\bin\\clang.exe',
      'C:\\Program Files (x86)\\LLVM\\bin\\clang.exe',
      'C:\\LLVM\\bin\\clang.exe',
      'clang.exe',
      'clang'
    ];

    for (const path of possiblePaths) {
      if (path === 'clang.exe' || path === 'clang') {
        // If it's just 'clang' or 'clang.exe', assume it's in PATH
        return path;
      } else if (existsSync(path)) {
        return path;
      }
    }
  } else {
    // Linux/Unix paths
    const possiblePaths = [
      '/usr/bin/clang',
      '/usr/local/bin/clang',
      'clang'  // Try PATH last
    ];

    for (const path of possiblePaths) {
      if (path === 'clang') {
        // Assume it's in PATH
        return path;
      } else if (existsSync(path)) {
        return path;
      }
    }
  }

  return null;
}

const CLANG_PATH = findClang() || 'clang';

// Log clang path for debugging
console.log('Clang path:', CLANG_PATH);
if (CLANG_PATH && CLANG_PATH !== 'clang' && CLANG_PATH !== 'clang.exe') {
  console.log('Clang found at:', CLANG_PATH);
  if (!existsSync(CLANG_PATH)) {
    console.warn('Warning: Clang path does not exist:', CLANG_PATH);
  }
} else {
  // On Linux, 'clang' in PATH is normal and expected
  if (process.platform === 'win32') {
    console.warn('Warning: Clang not found in common Windows paths. Using:', CLANG_PATH);
    console.warn('If obfuscation fails, set CLANG_PATH environment variable.');
  } else {
    console.log('Using clang from system PATH (expected on Linux)');
  }
}

// Ensure directories exist
[UPLOADS_DIR, OUTPUT_DIR].forEach(dir => {
  if (!existsSync(dir)) {
    mkdirSync(dir, { recursive: true });
  }
});

/**
 * Main obfuscation function
 */
export async function obfuscateFile(inputFilePath, originalFilename, config) {
  const jobId = await createJob({
    inputPath: inputFilePath,
    originalFilename,
    config
  });

  // Run obfuscation asynchronously
  runObfuscationPipeline(jobId, inputFilePath, originalFilename, config)
    .catch(error => {
      console.error(`Error in obfuscation pipeline for job ${jobId}:`, error);
      updateJob(jobId, {
        status: 'failed',
        error: error.message
      });
    });

  return jobId;
}

/**
 * Run the complete obfuscation pipeline
 */
async function runObfuscationPipeline(jobId, inputFilePath, originalFilename, config) {
  try {
    await updateJob(jobId, { progress: 10, message: 'Compiling C/C++ to LLVM IR...' });

    // Step 1: Compile C/C++ to LLVM IR
    const irFilename = basename(originalFilename, getExtension(originalFilename)) + '.ll';
    const irPath = join(OUTPUT_DIR, `${jobId}-${irFilename}`);

    await compileToLLVMIR(inputFilePath, irPath);

    await updateJob(jobId, { 
      irPath,
      progress: 25,
      message: 'Stripping exception handling from IR...' 
    });

    // Step 1.5: Strip exception handling before obfuscation to avoid breaking it
    const cleanedIrPath = await stripExceptionHandling(irPath, jobId);
    const irToObfuscate = cleanedIrPath || irPath;

    await updateJob(jobId, { 
      progress: 30,
      message: 'Running obfuscation pass...' 
    });

    // Step 2: Run obfuscator
    const obfuscatedIrFilename = basename(originalFilename, getExtension(originalFilename)) + '_obfuscated.ll';
    const obfuscatedIrPath = join(OUTPUT_DIR, `${jobId}-${obfuscatedIrFilename}`);
    const reportPath = join(OUTPUT_DIR, `${jobId}-obfuscation_report.txt`);

    await runObfuscator(irToObfuscate, obfuscatedIrPath, reportPath, config);

    await updateJob(jobId, { 
      obfuscatedIrPath,
      obfuscatedIrFilename,
      reportPath,
      progress: 80,
      message: 'Parsing obfuscation report...' 
    });

    // Step 2.5: Verify IR is valid (optional, but helps catch issues early)
    // Note: We skip this if opt is not available, as it's not critical

    await updateJob(jobId, { 
      progress: 80,
      message: 'Parsing obfuscation report...' 
    });

    // Step 4: Parse report
    let reportData = null;
    if (existsSync(reportPath)) {
      reportData = await parseReport(reportPath);
    }

    // Step 3: Mark as completed
    await updateJob(jobId, {
      status: 'completed',
      progress: 100,
      message: 'Obfuscation complete! Download the .ll file and compile it locally.',
      report: reportData,
      obfuscatedIrPath,
      obfuscatedIrFilename
    });

    console.log(`Obfuscation completed for job ${jobId}`);
  } catch (error) {
    console.error(`Obfuscation failed for job ${jobId}:`, error);
    await updateJob(jobId, {
      status: 'failed',
      error: error.message,
      stack: error.stack
    });
    throw error;
  }
}

/**
 * Strip exception handling from LLVM IR using opt
 */
async function stripExceptionHandling(irPath, jobId) {
  const isWindows = process.platform === 'win32';
  const optPath = process.env.OPT_PATH || (isWindows ? 'C:\\Program Files\\LLVM\\bin\\opt.exe' : 'opt');
  
  if (!existsSync(optPath)) {
    console.log('opt not found, skipping exception handling strip');
    return null;
  }

  try {
    const cleanedPath = join(OUTPUT_DIR, `${jobId}-${basename(irPath, '.ll')}_noeh.ll`);
    const command = isWindows
      ? `cmd /c ""${optPath}" -strip-eh "${irPath}" -o "${cleanedPath}"`
      : `"${optPath}" -strip-eh "${irPath}" -o "${cleanedPath}"`;
    
    console.log('Stripping exception handling:', command);
    const { stdout, stderr } = await execAsync(command);
    
    if (existsSync(cleanedPath)) {
      console.log('Exception handling stripped successfully');
      return cleanedPath;
    }
  } catch (error) {
    console.warn('Failed to strip exception handling, using original IR:', error.message);
  }
  
  return null;
}

/**
 * Compile C/C++ source to LLVM IR
 */
async function compileToLLVMIR(sourcePath, outputPath) {
  // Use the found clang path or try to find it again
  let clangPath = CLANG_PATH;
  if (!clangPath || clangPath === 'clang' || clangPath === 'clang.exe') {
    const foundClang = findClang();
    if (!foundClang) {
      throw new Error(
        'clang not found! Please install LLVM/Clang.\n' +
        'Download from: https://llvm.org/builds/\n' +
        'Or set CLANG_PATH environment variable to the clang.exe path.\n' +
        'Common locations: C:\\Program Files\\LLVM\\bin\\clang.exe'
      );
    }
    clangPath = foundClang;
  }

  // Verify the path exists
  if (!existsSync(clangPath) && clangPath !== 'clang' && clangPath !== 'clang.exe') {
    throw new Error(`Clang not found at: ${clangPath}`);
  }

  // On Windows, use cmd /c to properly handle paths with spaces
  // Use -fno-exceptions to avoid exception handling issues with obfuscation
  const isWindows = process.platform === 'win32';
  const command = isWindows
    ? `cmd /c ""${clangPath}" -S -emit-llvm -fno-exceptions "${sourcePath}" -o "${outputPath}""`
    : `"${clangPath}" -S -emit-llvm -fno-exceptions "${sourcePath}" -o "${outputPath}"`;
  
  console.log('Compiling to LLVM IR with command:', command);
  
  try {
    const { stdout, stderr } = await execAsync(command);
    if (stderr && !stderr.includes('warning') && !stderr.includes('note')) {
      throw new Error(`Clang compilation error: ${stderr}`);
    }
  } catch (error) {
    throw new Error(`Failed to compile to LLVM IR: ${error.message}`);
  }
}

/**
 * Run the obfuscator on LLVM IR
 */
async function runObfuscator(inputIrPath, outputIrPath, reportPath, config) {
  const args = buildObfuscatorArgs(inputIrPath, outputIrPath, reportPath, config);
  const command = `"${OBFUSCATOR_PATH}" ${args}`;

  try {
    const { stdout, stderr } = await execAsync(command, {
      maxBuffer: 10 * 1024 * 1024 // 10MB buffer
    });
    
    if (stderr && !stderr.includes('warning')) {
      console.warn('Obfuscator stderr:', stderr);
    }
  } catch (error) {
    throw new Error(`Obfuscation failed: ${error.message}`);
  }
}

/**
 * Build obfuscator command line arguments
 * 
 * Note: Some advanced options (stringEncryptionMethod, pbkdf2Iterations) are
 * compile-time settings in ObfuscationPass. The CLI tool may need updates
 * to support these via command-line flags. For now, they are stored in config
 * and may use defaults in the obfuscator.
 */
function buildObfuscatorArgs(inputPath, outputPath, reportPath, config) {
  const args = [
    `"${inputPath}"`,
    `-o "${outputPath}"`,
    `--report="${reportPath}"`
  ];

  // Add obfuscation technique flags
  if (config.enableControlFlowObfuscation) args.push('--cf');
  if (config.enableStringEncryption) args.push('--str');
  if (config.enableBogusCode) args.push('--bogus');
  if (config.enableFakeLoops) args.push('--loops');
  if (config.enableInstructionSubstitution) args.push('--subs');
  if (config.enableControlFlowFlattening) args.push('--flatten');
  if (config.enableMBA) args.push('--mba');
  if (config.enableAntiDebug) args.push('--anti-debug');
  if (config.enableIndirectCalls) args.push('--indirect');
  if (config.enableConstantObfuscation) args.push('--const-obf');
  if (config.enableAntiTamper) args.push('--anti-tamper');
  if (config.enableVirtualization) args.push('--virtualize');
  if (config.enablePolymorphic) args.push('--polymorphic');
  if (config.enableAntiAnalysis) args.push('--anti-analysis');
  if (config.enableMetamorphic) args.push('--metamorphic');
  if (config.enableDynamicObf) args.push('--dynamic');

  // Add numeric parameters
  if (config.obfuscationCycles) args.push(`--cycles=${config.obfuscationCycles}`);
  if (config.bogusCodePercentage) args.push(`--bogus-percent=${config.bogusCodePercentage}`);
  if (config.fakeLoopCount) args.push(`--fake-loops=${config.fakeLoopCount}`);
  if (config.mbaComplexity) args.push(`--mba-level=${config.mbaComplexity}`);
  if (config.flatteningProbability) args.push(`--flatten-prob=${config.flatteningProbability}`);
  if (config.virtualizationLevel) args.push(`--vm-level=${config.virtualizationLevel}`);
  if (config.polymorphicVariants) args.push(`--poly-variants=${config.polymorphicVariants}`);

  // String encryption settings (may need CLI tool updates to support)
  // For now, these are stored in config but may use defaults
  if (config.stringEncryptionMethod) {
    // TODO: Add CLI flag support for --string-method=XOR_ROTATING|RC4_SIMPLE|RC4_PBKDF2
    console.log(`String encryption method: ${config.stringEncryptionMethod} (may use default if CLI doesn't support)`);
  }
  if (config.pbkdf2Iterations) {
    // TODO: Add CLI flag support for --pbkdf2-iterations=N
    console.log(`PBKDF2 iterations: ${config.pbkdf2Iterations} (may use default if CLI doesn't support)`);
  }

  if (config.decryptStringsAtStartup) args.push('--decrypt-startup');

  return args.join(' ');
}

/**
 * Compile LLVM IR to executable
 */
async function compileToExecutable(irPath, outputPath) {
  // Use the found clang path or try to find it again
  let clangPath = CLANG_PATH;
  if (!clangPath || clangPath === 'clang' || clangPath === 'clang.exe') {
    const foundClang = findClang();
    if (!foundClang) {
      throw new Error(
        'clang not found! Please install LLVM/Clang.\n' +
        'Download from: https://llvm.org/builds/\n' +
        'Or set CLANG_PATH environment variable to the clang.exe path.\n' +
        'Common locations: C:\\Program Files\\LLVM\\bin\\clang.exe'
      );
    }
    clangPath = foundClang;
  }

  // Verify the path exists
  if (!existsSync(clangPath) && clangPath !== 'clang' && clangPath !== 'clang.exe') {
    throw new Error(`Clang not found at: ${clangPath}`);
  }

  // On Windows, use cmd /c to properly handle paths with spaces
  // Try to use opt to strip exception handling first, then compile
  const isWindows = process.platform === 'win32';
  const optPath = process.env.OPT_PATH || (isWindows ? 'C:\\Program Files\\LLVM\\bin\\opt.exe' : 'opt');
  
  // Try to use opt to strip exception handling if available
  let cleanedIrPath = irPath;
  if (existsSync(optPath)) {
    try {
      const cleanedPath = irPath.replace('.ll', '_cleaned.ll');
      const optCommand = isWindows
        ? `cmd /c ""${optPath}" -strip-eh "${irPath}" -o "${cleanedPath}"`
        : `"${optPath}" -strip-eh "${irPath}" -o "${cleanedPath}"`;
      
      console.log('Cleaning IR with opt:', optCommand);
      await execAsync(optCommand);
      cleanedIrPath = cleanedPath;
      console.log('IR cleaned successfully');
    } catch (optError) {
      console.warn('opt failed, using original IR:', optError.message);
      // Continue with original IR
    }
  }

  // Compile with -fno-exceptions to disable exception handling
  const command = isWindows
    ? `cmd /c ""${clangPath}" "${cleanedIrPath}" -o "${outputPath}" -fno-exceptions -lstdc++ -luser32 -lkernel32 -lntdll"`
    : `"${clangPath}" "${cleanedIrPath}" -o "${outputPath}" -fno-exceptions -lstdc++`;
  
  console.log('Compiling to executable with command:', command);
  
  try {
    const { stdout, stderr } = await execAsync(command);
    // Filter out exception handling warnings as they're expected
    const filteredStderr = stderr ? stderr.split('\n').filter(line => 
      !line.includes('warning') && 
      !line.includes('exception') &&
      !line.includes('note:')
    ).join('\n') : '';
    
    if (filteredStderr.trim()) {
      throw new Error(`Clang linking error: ${filteredStderr}`);
    }
  } catch (error) {
    // If compilation fails with -fno-exceptions, try using llvm-link or direct compilation
    console.log('Compilation with -fno-exceptions failed, trying alternative approach...');
    
    // Try using llc (LLVM compiler) instead
    const llcPath = process.env.LLC_PATH || (isWindows ? 'C:\\Program Files\\LLVM\\bin\\llc.exe' : 'llc');
    if (existsSync(llcPath)) {
      try {
        const objPath = outputPath.replace('.exe', '.o');
        const llcCommand = isWindows
          ? `cmd /c ""${llcPath}" -filetype=obj "${cleanedIrPath}" -o "${objPath}"`
          : `"${llcPath}" -filetype=obj "${cleanedIrPath}" -o "${objPath}"`;
        
        console.log('Using llc to compile:', llcCommand);
        await execAsync(llcCommand);
        
        // Link the object file
        const linkCommand = isWindows
          ? `cmd /c ""${clangPath}" "${objPath}" -o "${outputPath}" -lstdc++ -luser32 -lkernel32 -lntdll"`
          : `"${clangPath}" "${objPath}" -o "${outputPath}" -lstdc++`;
        
        await execAsync(linkCommand);
        console.log('Successfully compiled using llc');
        return;
      } catch (llcError) {
        console.warn('llc approach failed:', llcError.message);
      }
    }
    
    throw new Error(`Failed to compile to executable: ${error.message}`);
  }
}

/**
 * Get file extension
 */
function getExtension(filename) {
  const lastDot = filename.lastIndexOf('.');
  return lastDot !== -1 ? filename.substring(lastDot) : '';
}

