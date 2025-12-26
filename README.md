# LLVM Code Obfuscator Web Application

A modern, cloud-hosted web application that makes the LLVM Code Obfuscator accessible to users through a browser. Users can upload C/C++ source files, configure obfuscation settings, and receive ready-to-run executables plus detailed obfuscation reports.

## Features

- **User-Friendly Interface**: VirusTotal-inspired modern UI with drag-and-drop file upload
- **Multiple Obfuscation Presets**: Light, Medium, Heavy, and Custom configurations
- **Real-time Progress**: Live updates during obfuscation process
- **Detailed Reports**: Comprehensive obfuscation statistics displayed directly on the website
- **Ready-to-Run Executables**: Download obfuscated binaries immediately
- **Cloud-Ready**: Designed for deployment on Google Cloud Platform

## Architecture

```
Frontend (Next.js) → Backend API (Node.js) → Obfuscation Service (C++ binary)
```

## Project Structure

```
web-app/
├── frontend/          # Next.js frontend application
├── backend/           # Express.js backend API
├── deployment/        # GCP deployment configurations
└── docker-compose.yml # Local development setup
```

## Quick Start

### Prerequisites

- Node.js 20+
- Docker and Docker Compose
- Built LLVM obfuscator binary (`llvm-obfuscator.exe` or `llvm-obfuscator`)
- Clang/LLVM toolchain installed

### Local Development

1. **Start Backend**:
   ```bash
   cd backend
   npm install
   npm start
   ```

2. **Start Frontend**:
   ```bash
   cd frontend
   npm install
   npm run dev
   ```

3. **Using Docker Compose**:
   ```bash
   docker-compose up
   ```

### Access the Application

- Frontend: http://localhost:3000
- Backend API: http://localhost:3001

## Deployment

See [deployment/gcp-setup.md](deployment/gcp-setup.md) for detailed GCP deployment instructions.

### Quick Deploy to GCP

```bash
# Set environment variables
export PROJECT_ID="your-project-id"
export REGION="us-central1"

# Deploy using Cloud Build
gcloud builds submit --config=deployment/cloudbuild.yaml
```

## API Endpoints

### POST /api/obfuscate
Upload a C/C++ file and start obfuscation.

**Request**: Multipart form data with `file`, `preset`, and `options`

**Response**:
```json
{
  "jobId": "uuid",
  "status": "processing",
  "message": "Obfuscation job started"
}
```

### GET /api/status/:jobId
Get the status of an obfuscation job.

**Response**:
```json
{
  "status": "completed",
  "progress": 100,
  "executableUrl": "/api/download/jobId/executable",
  "report": {
    "raw": "...",
    "parsed": {
      "metrics": {...},
      "summary": {...}
    }
  }
}
```

### GET /api/download/:jobId/:fileType
Download obfuscated executable or report.

**File Types**: `executable`, `report`

## Configuration

### Backend Configuration

Edit `backend/.env`:
```env
PORT=3001
CLANG_PATH=clang
OBFUSCATOR_PATH=./llvm-obfuscator
MAX_FILE_SIZE=10485760
```

### Frontend Configuration

Edit `frontend/next.config.js`:
```javascript
env: {
  NEXT_PUBLIC_API_URL: 'http://localhost:3001'
}
```

## Obfuscation Presets

- **Light**: Basic protection with minimal overhead (5-10%)
- **Medium**: Balanced security and performance (15-25%)
- **Heavy**: Maximum protection with higher overhead (50-200%)
- **Custom**: Full control over all obfuscation techniques

## Security Considerations

- File size limits (10MB default)
- File type validation (C/C++ only)
- Timeout protection
- Sandboxed execution in Docker
- Automatic cleanup of temporary files
- HTTPS only in production

## License

MIT License

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues.

## Support

For issues or questions, please check:
- Backend logs: `gcloud run services logs read llvm-obfuscator-backend`
- Frontend logs: `gcloud run services logs read llvm-obfuscator-frontend`
- Health endpoint: `GET /health`


