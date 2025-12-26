import { readFile, writeFile, mkdir, rm } from 'fs/promises';
import { existsSync } from 'fs';
import { join, dirname, basename, extname } from 'path';

/**
 * Ensure directory exists
 */
export async function ensureDir(dirPath) {
  if (!existsSync(dirPath)) {
    await mkdir(dirPath, { recursive: true });
  }
}

/**
 * Read file content
 */
export async function readFileContent(filePath) {
  try {
    return await readFile(filePath, 'utf-8');
  } catch (error) {
    throw new Error(`Failed to read file ${filePath}: ${error.message}`);
  }
}

/**
 * Write file content
 */
export async function writeFileContent(filePath, content) {
  await ensureDir(dirname(filePath));
  await writeFile(filePath, content, 'utf-8');
}

/**
 * Clean directory
 */
export async function cleanDir(dirPath) {
  if (existsSync(dirPath)) {
    await rm(dirPath, { recursive: true, force: true });
  }
  await ensureDir(dirPath);
}

/**
 * Generate test file paths
 */
export function getTestPaths(testName, outputDir) {
  const baseName = `test_${testName}`;
  return {
    ir: join(outputDir, `${baseName}.ll`),
    obfuscatedIr: join(outputDir, `${baseName}_obfuscated.ll`),
    executable: join(outputDir, `${baseName}_obfuscated.exe`),
    report: join(outputDir, `${baseName}_report.txt`)
  };
}

/**
 * Check if file exists
 */
export function fileExists(filePath) {
  return existsSync(filePath);
}

/**
 * Get file size
 */
export async function getFileSize(filePath) {
  if (!existsSync(filePath)) {
    return 0;
  }
  const { stat } = await import('fs/promises');
  const stats = await stat(filePath);
  return stats.size;
}


