import { v4 as uuidv4 } from 'uuid';
import { readFile, writeFile, mkdir } from 'fs/promises';
import { existsSync } from 'fs';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const JOBS_DIR = join(__dirname, '../jobs');

// Ensure jobs directory exists
if (!existsSync(JOBS_DIR)) {
  mkdir(JOBS_DIR, { recursive: true });
}

/**
 * Create a new job
 */
export async function createJob(initialData = {}) {
  const jobId = uuidv4();
  const jobData = {
    jobId,
    status: 'processing',
    createdAt: new Date().toISOString(),
    ...initialData
  };

  const jobPath = join(JOBS_DIR, `${jobId}.json`);
  await writeFile(jobPath, JSON.stringify(jobData, null, 2));

  return jobId;
}

/**
 * Update job status
 */
export async function updateJob(jobId, updates) {
  const jobPath = join(JOBS_DIR, `${jobId}.json`);
  
  if (!existsSync(jobPath)) {
    throw new Error(`Job ${jobId} not found`);
  }

  const jobData = JSON.parse(await readFile(jobPath, 'utf-8'));
  const updatedJob = {
    ...jobData,
    ...updates,
    updatedAt: new Date().toISOString()
  };

  await writeFile(jobPath, JSON.stringify(updatedJob, null, 2));
  return updatedJob;
}

/**
 * Get job status
 */
export async function getJobStatus(jobId) {
  const jobPath = join(JOBS_DIR, `${jobId}.json`);
  
  if (!existsSync(jobPath)) {
    return null;
  }

  const jobData = JSON.parse(await readFile(jobPath, 'utf-8'));
  return jobData;
}

/**
 * Cleanup job files
 */
export async function cleanupJob(jobId) {
  const jobPath = join(JOBS_DIR, `${jobId}.json`);
  const job = await getJobStatus(jobId);

  if (job) {
    // Delete job file
    if (existsSync(jobPath)) {
      const { unlink } = await import('fs/promises');
      await unlink(jobPath);
    }

    // Cleanup associated files
    const { rm } = await import('fs/promises');
    
    if (job.inputPath && existsSync(job.inputPath)) {
      await rm(job.inputPath, { force: true });
    }
    if (job.irPath && existsSync(job.irPath)) {
      await rm(job.irPath, { force: true });
    }
    if (job.obfuscatedIrPath && existsSync(job.obfuscatedIrPath)) {
      await rm(job.obfuscatedIrPath, { force: true });
    }
    if (job.executablePath && existsSync(job.executablePath)) {
      await rm(job.executablePath, { force: true });
    }
    if (job.reportPath && existsSync(job.reportPath)) {
      await rm(job.reportPath, { force: true });
    }
  }
}


