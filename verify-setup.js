#!/usr/bin/env node

/**
 * Setup Verification Script
 * Checks if all required components are in place
 */

import { existsSync } from 'fs';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const checks = [];
let allPassed = true;

function check(name, condition, message) {
  const passed = condition();
  checks.push({ name, passed, message });
  if (!passed) allPassed = false;
  
  const icon = passed ? '✓' : '✗';
  const status = passed ? 'PASS' : 'FAIL';
  console.log(`${icon} [${status}] ${name}: ${message}`);
}

console.log('🔍 Verifying LLVM Obfuscator Web App Setup...\n');

// Check directories
check('Backend directory', () => existsSync(join(__dirname, 'backend')), 'Backend folder exists');
check('Frontend directory', () => existsSync(join(__dirname, 'frontend')), 'Frontend folder exists');
check('Deployment directory', () => existsSync(join(__dirname, 'deployment')), 'Deployment folder exists');

// Check backend files
check('Backend package.json', () => existsSync(join(__dirname, 'backend/package.json')), 'Backend package.json exists');
check('Backend server.js', () => existsSync(join(__dirname, 'backend/src/server.js')), 'Backend server file exists');
check('Backend node_modules', () => existsSync(join(__dirname, 'backend/node_modules')), 'Backend dependencies installed');

// Check frontend files
check('Frontend package.json', () => existsSync(join(__dirname, 'frontend/package.json')), 'Frontend package.json exists');
check('Frontend app directory', () => existsSync(join(__dirname, 'frontend/app')), 'Frontend app directory exists');
check('Frontend node_modules', () => existsSync(join(__dirname, 'frontend/node_modules')), 'Frontend dependencies installed');

// Check required directories
check('Backend uploads directory', () => existsSync(join(__dirname, 'backend/uploads')), 'Uploads directory exists');
check('Backend output directory', () => existsSync(join(__dirname, 'backend/output')), 'Output directory exists');
check('Backend jobs directory', () => existsSync(join(__dirname, 'backend/jobs')), 'Jobs directory exists');

// Check Docker files
check('Docker Compose', () => existsSync(join(__dirname, 'docker-compose.yml')), 'Docker Compose file exists');
check('Backend Dockerfile', () => existsSync(join(__dirname, 'backend/Dockerfile')), 'Backend Dockerfile exists');
check('Frontend Dockerfile', () => existsSync(join(__dirname, 'frontend/Dockerfile')), 'Frontend Dockerfile exists');

// Check deployment files
check('Cloud Build config', () => existsSync(join(__dirname, 'deployment/cloudbuild.yaml')), 'Cloud Build config exists');
check('Cloud Run config', () => existsSync(join(__dirname, 'deployment/cloudrun.yaml')), 'Cloud Run config exists');
check('GCP Setup docs', () => existsSync(join(__dirname, 'deployment/gcp-setup.md')), 'GCP setup documentation exists');

// Check obfuscator binary (optional)
const obfuscatorWin = existsSync(join(__dirname, 'output/llvm-obfuscator.exe'));
const obfuscatorUnix = existsSync(join(__dirname, 'output/llvm-obfuscator'));
check('Obfuscator binary', () => obfuscatorWin || obfuscatorUnix, 
  obfuscatorWin ? 'Obfuscator binary found (Windows)' : 
  obfuscatorUnix ? 'Obfuscator binary found (Unix)' : 
  'Obfuscator binary not found - build it first');

console.log('\n' + '='.repeat(50));
if (allPassed) {
  console.log('✅ All checks passed! Setup is complete.');
  console.log('\nNext steps:');
  console.log('1. Ensure obfuscator binary is built (if not already)');
  console.log('2. Configure backend/.env with correct paths');
  console.log('3. Run: start-dev.bat (Windows) or ./start-dev.sh (Linux/Mac)');
  console.log('4. Or manually: cd backend && npm start (Terminal 1)');
  console.log('5. And: cd frontend && npm run dev (Terminal 2)');
} else {
  console.log('⚠️  Some checks failed. Please review the issues above.');
}
console.log('='.repeat(50));

process.exit(allPassed ? 0 : 1);


