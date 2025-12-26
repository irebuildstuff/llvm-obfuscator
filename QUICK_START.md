# Quick Start Guide

## Prerequisites

1. **Node.js 20+** installed
2. **LLVM/Clang** toolchain installed and in PATH
3. **Built obfuscator binary** (`llvm-obfuscator.exe` on Windows or `llvm-obfuscator` on Linux/Mac)
   - Should be located at: `output/llvm-obfuscator.exe` (Windows) or `output/llvm-obfuscator` (Linux/Mac)

## Installation

### 1. Install Dependencies

```bash
# Backend
cd backend
npm install

# Frontend
cd ../frontend
npm install
```

### 2. Configure Environment

Create `backend/.env` file:
```env
PORT=3001
CLANG_PATH=clang
OBFUSCATOR_PATH=../output/llvm-obfuscator.exe
MAX_FILE_SIZE=10485760
```

## Running Locally

### Option 1: Using Helper Scripts

**Windows:**
```bash
start-dev.bat
```

**Linux/Mac:**
```bash
chmod +x start-dev.sh
./start-dev.sh
```

### Option 2: Manual Start

**Terminal 1 - Backend:**
```bash
cd backend
npm start
```

**Terminal 2 - Frontend:**
```bash
cd frontend
npm run dev
```

### Option 3: Docker Compose

```bash
docker-compose up
```

## Access the Application

- **Frontend**: http://localhost:3000
- **Backend API**: http://localhost:3001
- **Health Check**: http://localhost:3001/health

## Testing

1. Open http://localhost:3000 in your browser
2. Upload a C/C++ source file (.c, .cpp, .cc, .cxx, .c++)
3. Select an obfuscation preset (Light/Medium/Heavy/Custom)
4. Click "Start Obfuscation"
5. Wait for processing to complete
6. Download the obfuscated executable
7. View the detailed obfuscation report

## Troubleshooting

### Backend won't start
- Check that Node.js 20+ is installed: `node --version`
- Verify dependencies are installed: `cd backend && npm install`
- Check that port 3001 is not in use
- Ensure `uploads`, `output`, and `jobs` directories exist in backend folder

### Frontend won't start
- Check that Node.js 20+ is installed
- Verify dependencies are installed: `cd frontend && npm install`
- Check that port 3000 is not in use
- Ensure backend is running first

### Obfuscation fails
- Verify clang is installed and in PATH: `clang --version`
- Check that obfuscator binary exists at the configured path
- Verify the input C/C++ file compiles correctly
- Check backend logs for detailed error messages

### File upload fails
- Ensure file size is under 10MB
- Verify file extension is .c, .cpp, .cc, .cxx, or .c++
- Check that `backend/uploads` directory exists and is writable

## Next Steps

- See [README.md](README.md) for full documentation
- See [deployment/gcp-setup.md](deployment/gcp-setup.md) for GCP deployment
- Check backend logs for detailed error information


