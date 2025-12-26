import { readFile } from 'fs/promises';
import { join } from 'path';
import { WEBAPP_API_URL, WEBAPP_TIMEOUT, TEST_INPUT_FILE, WEBAPP_OUTPUT_DIR } from './config.js';
import { getTestPaths, ensureDir, writeFileContent } from './utils/file-utils.js';
import fetch from 'node-fetch';
import FormData from 'form-data';

/**
 * Upload file and start obfuscation job
 */
export async function startObfuscationJob(filePath, config) {
  const formData = new FormData();
  const fileBuffer = await readFile(filePath);
  formData.append('file', fileBuffer, { filename: filePath.split(/[/\\]/).pop() });
  
  // If config has a preset, use it; otherwise use 'custom' and send all options
  if (config.preset) {
    formData.append('preset', config.preset);
    if (config.options && Object.keys(config.options).length > 0) {
      formData.append('options', JSON.stringify(config.options));
    }
  } else {
    // For technique tests, send as 'custom' with all options
    formData.append('preset', 'custom');
    formData.append('options', JSON.stringify(config));
  }

  const response = await fetch(`${WEBAPP_API_URL}/api/obfuscate`, {
    method: 'POST',
    body: formData,
    headers: formData.getHeaders()
  });

  if (!response.ok) {
    const error = await response.text();
    throw new Error(`Failed to start obfuscation: ${response.status} ${error}`);
  }

  const data = await response.json();
  return data.jobId;
}

/**
 * Poll job status until completion
 */
export async function waitForJobCompletion(jobId, timeout = WEBAPP_TIMEOUT) {
  const startTime = Date.now();
  const pollInterval = 2000; // 2 seconds

  while (Date.now() - startTime < timeout) {
    const response = await fetch(`${WEBAPP_API_URL}/api/status/${jobId}`);

    if (!response.ok) {
      throw new Error(`Failed to get job status: ${response.status}`);
    }

    const status = await response.json();

    if (status.status === 'completed') {
      return status;
    }

    if (status.status === 'failed') {
      throw new Error(`Job failed: ${status.error || 'Unknown error'}`);
    }

    // Wait before next poll
    await new Promise(resolve => setTimeout(resolve, pollInterval));
  }

  throw new Error(`Job timeout after ${timeout}ms`);
}

/**
 * Download file from web app
 */
export async function downloadFile(jobId, fileType) {
  const response = await fetch(`${WEBAPP_API_URL}/api/download/${jobId}/${fileType}`);

  if (!response.ok) {
    throw new Error(`Failed to download ${fileType}: ${response.status}`);
  }

  return await response.text();
}

/**
 * Download IR file content
 */
export async function downloadIR(jobId) {
  try {
    return await downloadFile(jobId, 'ir');
  } catch (error) {
    // IR might not be available for download, try viewing it
    const response = await fetch(`${WEBAPP_API_URL}/api/view/${jobId}/ir`);
    if (response.ok) {
      const data = await response.json();
      return data.content;
    }
    throw error;
  }
}

/**
 * Run complete web app obfuscation pipeline
 */
export async function runWebAppPipeline(testName, config) {
  const outputDir = join(WEBAPP_OUTPUT_DIR, testName);
  await ensureDir(outputDir);

  const paths = getTestPaths(testName, outputDir);

  // Step 1: Start obfuscation job
  const jobId = await startObfuscationJob(TEST_INPUT_FILE, config);

  // Step 2: Wait for completion
  const status = await waitForJobCompletion(jobId);

  // Step 3: Download outputs
  if (status.obfuscatedIrPath) {
    try {
      const irContent = await downloadIR(jobId);
      await writeFileContent(paths.obfuscatedIr, irContent);
    } catch (error) {
      console.warn(`Failed to download IR: ${error.message}`);
    }
  }

  if (status.reportPath) {
    try {
      const reportContent = await downloadFile(jobId, 'report');
      await writeFileContent(paths.report, reportContent);
    } catch (error) {
      console.warn(`Failed to download report: ${error.message}`);
    }
  }

  if (status.executablePath) {
    try {
      const response = await fetch(`${WEBAPP_API_URL}/api/download/${jobId}/executable`);
      if (response.ok) {
        const arrayBuffer = await response.arrayBuffer();
        const buffer = Buffer.from(arrayBuffer);
        const { writeFile } = await import('fs/promises');
        await writeFile(paths.executable, buffer);
      }
    } catch (error) {
      console.warn(`Failed to download executable: ${error.message}`);
    }
  }

  return { paths, jobId, status };
}

