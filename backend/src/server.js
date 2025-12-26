import express from 'express';
import cors from 'cors';
import multer from 'multer';
import { fileURLToPath } from 'url';
import { dirname, join } from 'path';
import { config } from 'dotenv';
import { obfuscateFile } from './obfuscation-service.js';
import { cleanupJob, getJobStatus } from './job-manager.js';
import { existsSync } from 'fs';

config();

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const app = express();
const PORT = process.env.PORT || 3001;
const MAX_FILE_SIZE = 10 * 1024 * 1024; // 10MB

// Middleware
app.use(cors({
  origin: process.env.FRONTEND_URL || '*', // Allow all origins in development, set specific in production
  credentials: true
}));
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// Configure multer for file uploads
const storage = multer.diskStorage({
  destination: (req, file, cb) => {
    const uploadDir = join(__dirname, '../uploads');
    cb(null, uploadDir);
  },
  filename: (req, file, cb) => {
    const uniqueSuffix = Date.now() + '-' + Math.round(Math.random() * 1E9);
    cb(null, uniqueSuffix + '-' + file.originalname);
  }
});

const upload = multer({
  storage: storage,
  limits: {
    fileSize: MAX_FILE_SIZE
  },
  fileFilter: (req, file, cb) => {
    // Only allow C/C++ source files
    const allowedExtensions = ['.c', '.cpp', '.cc', '.cxx', '.c++'];
    const ext = file.originalname.toLowerCase().substring(file.originalname.lastIndexOf('.'));
    
    if (allowedExtensions.includes(ext)) {
      cb(null, true);
    } else {
      cb(new Error(`Invalid file type. Only C/C++ source files are allowed. Got: ${ext}`), false);
    }
  }
});

// Health check endpoint
app.get('/health', (req, res) => {
  res.json({ status: 'ok', timestamp: new Date().toISOString() });
});

// Main obfuscation endpoint
app.post('/api/obfuscate', upload.single('file'), async (req, res) => {
  try {
    if (!req.file) {
      return res.status(400).json({ error: 'No file uploaded' });
    }

    let { preset, options } = req.body;
    
    // Parse options if it's a JSON string
    if (options && typeof options === 'string') {
      try {
        options = JSON.parse(options);
      } catch (e) {
        console.warn('Failed to parse options JSON:', e);
      }
    }
    
    const config = parseObfuscationConfig(preset, options);

    // Start obfuscation job
    const jobId = await obfuscateFile(req.file.path, req.file.originalname, config);

    res.json({
      jobId,
      status: 'processing',
      message: 'Obfuscation job started'
    });
  } catch (error) {
    console.error('Error starting obfuscation:', error);
    res.status(500).json({ 
      error: 'Failed to start obfuscation job',
      message: error.message 
    });
  }
});

// Get job status
app.get('/api/status/:jobId', async (req, res) => {
  try {
    const { jobId } = req.params;
    const status = await getJobStatus(jobId);

    if (!status) {
      return res.status(404).json({ error: 'Job not found' });
    }

    res.json(status);
  } catch (error) {
    console.error('Error getting job status:', error);
    res.status(500).json({ 
      error: 'Failed to get job status',
      message: error.message 
    });
  }
});

// View obfuscated IR code
app.get('/api/view/:jobId/ir', async (req, res) => {
  try {
    const { jobId } = req.params;
    const status = await getJobStatus(jobId);

    if (!status) {
      return res.status(404).json({ error: 'Job not found' });
    }

    if (status.status !== 'completed') {
      return res.status(400).json({ error: 'Job not completed yet' });
    }

    if (!status.obfuscatedIrPath) {
      console.error(`Job ${jobId} does not have obfuscatedIrPath in status`);
      return res.status(404).json({ 
        error: 'Obfuscated IR file path not found in job status',
        details: 'The obfuscation may not have completed successfully'
      });
    }

    if (!existsSync(status.obfuscatedIrPath)) {
      console.error(`Obfuscated IR file does not exist: ${status.obfuscatedIrPath}`);
      return res.status(404).json({ 
        error: 'Obfuscated IR file not found on disk',
        details: `Expected path: ${status.obfuscatedIrPath}`
      });
    }

    const { readFile } = await import('fs/promises');
    const irContent = await readFile(status.obfuscatedIrPath, 'utf-8');
    const filename = status.obfuscatedIrPath.split(/[/\\]/).pop();

    res.json({
      content: irContent,
      filename: filename || 'obfuscated.ll'
    });
  } catch (error) {
    console.error('Error reading obfuscated IR:', error);
    res.status(500).json({ 
      error: 'Failed to read obfuscated IR',
      message: error.message 
    });
  }
});

// Download results
app.get('/api/download/:jobId/:fileType', async (req, res) => {
  try {
    const { jobId, fileType } = req.params;
    const status = await getJobStatus(jobId);

    if (!status) {
      return res.status(404).json({ error: 'Job not found' });
    }

    if (status.status !== 'completed') {
      return res.status(400).json({ error: 'Job not completed yet' });
    }

    let filePath;
    let contentType;
    let filename;

    if (fileType === 'ir') {
      filePath = status.obfuscatedIrPath;
      contentType = 'text/plain';
      filename = filePath ? filePath.split(/[/\\]/).pop() : 'obfuscated.ll';
    } else if (fileType === 'report') {
      filePath = status.reportPath;
      contentType = 'text/plain';
      filename = 'obfuscation_report.txt';
    } else {
      return res.status(400).json({ error: 'Invalid file type' });
    }

    if (!filePath || !existsSync(filePath)) {
      return res.status(404).json({ error: 'File not found' });
    }

    res.setHeader('Content-Type', contentType);
    res.setHeader('Content-Disposition', `attachment; filename="${filename}"`);
    res.sendFile(filePath);
  } catch (error) {
    console.error('Error downloading file:', error);
    res.status(500).json({ 
      error: 'Failed to download file',
      message: error.message 
    });
  }
});

// Cleanup old jobs (optional endpoint for manual cleanup)
app.delete('/api/jobs/:jobId', async (req, res) => {
  try {
    const { jobId } = req.params;
    await cleanupJob(jobId);
    res.json({ message: 'Job cleaned up successfully' });
  } catch (error) {
    console.error('Error cleaning up job:', error);
    res.status(500).json({ 
      error: 'Failed to cleanup job',
      message: error.message 
    });
  }
});

// Parse obfuscation configuration from request
function parseObfuscationConfig(preset, options) {
  const config = {
    // Core obfuscation techniques
    enableControlFlowObfuscation: true,
    enableStringEncryption: true,
    enableBogusCode: false,
    enableFakeLoops: false,
    enableInstructionSubstitution: false,
    enableControlFlowFlattening: false,
    enableMBA: false,
    
    // Advanced protection features
    enableAntiDebug: false,
    enableIndirectCalls: false,
    enableConstantObfuscation: false,
    enableAntiTamper: false,
    enableVirtualization: false,
    enablePolymorphic: false,
    enableAntiAnalysis: false,
    enableMetamorphic: false,
    enableDynamicObf: false,
    
    // String encryption settings (NEW: World-Class Features)
    stringEncryptionMethod: 'RC4_PBKDF2', // 'XOR_ROTATING', 'RC4_SIMPLE', 'RC4_PBKDF2'
    pbkdf2Iterations: 1000,
    decryptStringsAtStartup: false, // Lazy decryption by default
    
    // Technique parameters
    obfuscationCycles: 3,
    bogusCodePercentage: 30,
    fakeLoopCount: 5,
    mbaComplexity: 3,
    flatteningProbability: 80,
    virtualizationLevel: 2,
    polymorphicVariants: 5
  };

  // Apply preset
  if (preset === 'basic') {
    // Basic preset: minimal protection, fast
    config.enableControlFlowObfuscation = true;
    config.enableStringEncryption = false;
    config.enableBogusCode = false;
    config.enableFakeLoops = false;
    config.enableAntiDebug = false;
    config.obfuscationCycles = 1;
    config.bogusCodePercentage = 10;
    config.stringEncryptionMethod = 'XOR_ROTATING'; // Legacy for speed
  } else if (preset === 'light') {
    // Light preset: control flow + basic string encryption
    config.enableControlFlowObfuscation = true;
    config.enableStringEncryption = true;
    config.enableBogusCode = false;
    config.enableFakeLoops = false;
    config.enableAntiDebug = false;
    config.obfuscationCycles = 1;
    config.bogusCodePercentage = 10;
    config.stringEncryptionMethod = 'RC4_SIMPLE'; // Medium security
    config.pbkdf2Iterations = 500;
  } else if (preset === 'medium') {
    // Medium preset: balanced security
    config.enableControlFlowObfuscation = true;
    config.enableStringEncryption = true;
    config.enableBogusCode = true;
    config.enableFakeLoops = true;
    config.enableAntiDebug = true;
    config.enableIndirectCalls = true; // Import hiding
    config.obfuscationCycles = 3;
    config.bogusCodePercentage = 30;
    config.stringEncryptionMethod = 'RC4_PBKDF2'; // Strong encryption
    config.pbkdf2Iterations = 1000;
  } else if (preset === 'heavy') {
    // Heavy preset: maximum protection (world-class)
    config.enableControlFlowObfuscation = true;
    config.enableStringEncryption = true;
    config.enableBogusCode = true;
    config.enableFakeLoops = true;
    config.enableInstructionSubstitution = true;
    config.enableControlFlowFlattening = true;
    config.enableMBA = true;
    config.enableAntiDebug = true;
    config.enableIndirectCalls = true; // Import hiding
    config.enableAntiTamper = true;
    config.enablePolymorphic = true;
    config.enableMetamorphic = true;
    config.obfuscationCycles = 5;
    config.bogusCodePercentage = 50;
    config.mbaComplexity = 5;
    config.stringEncryptionMethod = 'RC4_PBKDF2'; // Strongest encryption
    config.pbkdf2Iterations = 2000; // Higher iterations for heavy preset
    config.polymorphicVariants = 5;
  }

  // Override with custom options if provided
  if (options && typeof options === 'object') {
    Object.assign(config, options);
  }

  return config;
}

// Error handling middleware
app.use((error, req, res, next) => {
  if (error instanceof multer.MulterError) {
    if (error.code === 'LIMIT_FILE_SIZE') {
      return res.status(400).json({ error: 'File too large. Maximum size is 10MB' });
    }
  }
  
  res.status(500).json({ 
    error: 'Internal server error',
    message: error.message 
  });
});

// Start server
app.listen(PORT, () => {
  console.log(`Backend server running on port ${PORT}`);
  console.log(`Health check: http://localhost:${PORT}/health`);
});

