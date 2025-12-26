# GCP Deployment Guide for LLVM Obfuscator

Complete guide to deploy `obfuscator.live` to Google Cloud Platform.

## Prerequisites

1. **Google Cloud Account** with billing enabled
2. **gcloud CLI** installed ([Download](https://cloud.google.com/sdk/docs/install))
3. **Docker** installed (for local testing, optional)
4. **Domain**: `obfuscator.live` (already purchased from GoDaddy)

## Quick Start (Windows PowerShell)

```powershell
# 1. Install gcloud CLI if not already installed
# Download from: https://cloud.google.com/sdk/docs/install

# 2. Authenticate with Google Cloud
gcloud auth login

# 3. Set your project ID (replace with your actual project ID)
$PROJECT_ID = "your-gcp-project-id"
$REGION = "us-central1"

# 4. Run the deployment script
.\deployment\deploy.ps1 -ProjectId $PROJECT_ID -Region $REGION
```

## Step-by-Step Manual Deployment

### Step 1: Set Up GCP Project

```powershell
# Set your project ID
$PROJECT_ID = "your-gcp-project-id"
$REGION = "us-central1"

# Create a new project (if needed)
gcloud projects create $PROJECT_ID

# Set the project
gcloud config set project $PROJECT_ID

# Enable billing (do this in GCP Console)
# https://console.cloud.google.com/billing
```

### Step 2: Enable Required APIs

```powershell
gcloud services enable `
    cloudbuild.googleapis.com `
    run.googleapis.com `
    artifactregistry.googleapis.com `
    storage.googleapis.com `
    cloudresourcemanager.googleapis.com
```

### Step 3: Create Artifact Registry

```powershell
gcloud artifacts repositories create llvm-obfuscator `
    --repository-format=docker `
    --location=$REGION `
    --description="Docker repository for LLVM Obfuscator"
```

### Step 4: Create Cloud Storage Bucket

```powershell
$BUCKET_NAME = "$PROJECT_ID-obfuscator-temp"
gsutil mb -p $PROJECT_ID -c STANDARD -l $REGION "gs://$BUCKET_NAME"

# Set lifecycle policy (auto-delete after 24 hours)
@"
{
  "lifecycle": {
    "rule": [
      {
        "action": {"type": "Delete"},
        "condition": {"age": 1}
      }
    ]
  }
}
"@ | Out-File -FilePath "$env:TEMP\lifecycle.json" -Encoding utf8

gsutil lifecycle set "$env:TEMP\lifecycle.json" "gs://$BUCKET_NAME"
```

### Step 5: Build and Deploy

#### Option A: Using Deployment Script (Recommended)

```powershell
.\deployment\deploy.ps1 -ProjectId $PROJECT_ID -Region $REGION
```

#### Option B: Using Cloud Build

```powershell
gcloud builds submit --config=deployment/cloudbuild.yaml `
    --substitutions="_REGION=$REGION,_REPO_NAME=llvm-obfuscator"
```

### Step 6: Configure Custom Domain

After deployment, map your domain `obfuscator.live`:

```powershell
# Get frontend service URL
$FRONTEND_URL = gcloud run services describe llvm-obfuscator-frontend `
    --region $REGION `
    --format 'value(status.url)'

# Create domain mapping
gcloud run domain-mappings create `
    --service llvm-obfuscator-frontend `
    --domain obfuscator.live `
    --region $REGION
```

### Step 7: Update DNS in GoDaddy

1. Log in to GoDaddy
2. Go to DNS Management for `obfuscator.live`
3. Add/Update CNAME record:
   - **Name**: `@` (or leave blank for root domain)
   - **Value**: The Cloud Run service URL (provided after domain mapping)
   - **TTL**: 600 (or default)

**Note**: For root domains, you may need to use an A record instead. Cloud Run will provide the IP addresses after domain mapping.

## Environment Variables

### Backend Service

```powershell
gcloud run services update llvm-obfuscator-backend `
    --region $REGION `
    --update-env-vars="CLANG_PATH=clang,OBFUSCATOR_PATH=/app/llvm-obfuscator,NODE_ENV=production"
```

### Frontend Service

The frontend automatically gets the backend URL. To update manually:

```powershell
$BACKEND_URL = gcloud run services describe llvm-obfuscator-backend `
    --region $REGION `
    --format 'value(status.url)'

gcloud run services update llvm-obfuscator-frontend `
    --region $REGION `
    --update-env-vars="NEXT_PUBLIC_API_URL=$BACKEND_URL,NODE_ENV=production"
```

## Monitoring and Logs

### View Logs

```powershell
# Backend logs
gcloud run services logs read llvm-obfuscator-backend --region $REGION --limit 50

# Frontend logs
gcloud run services logs read llvm-obfuscator-frontend --region $REGION --limit 50

# Follow logs in real-time
gcloud run services logs tail llvm-obfuscator-backend --region $REGION
```

### View Service Status

```powershell
# Backend status
gcloud run services describe llvm-obfuscator-backend --region $REGION

# Frontend status
gcloud run services describe llvm-obfuscator-frontend --region $REGION
```

## Updating Deployment

### Rebuild and Redeploy

```powershell
# Using Cloud Build
gcloud builds submit --config=deployment/cloudbuild.yaml `
    --substitutions="_REGION=$REGION,_REPO_NAME=llvm-obfuscator"
```

### Update Specific Service

```powershell
# Update backend
gcloud run deploy llvm-obfuscator-backend `
    --image ${REGION}-docker.pkg.dev/${PROJECT_ID}/llvm-obfuscator/backend:latest `
    --region $REGION

# Update frontend
gcloud run deploy llvm-obfuscator-frontend `
    --image ${REGION}-docker.pkg.dev/${PROJECT_ID}/llvm-obfuscator/frontend:latest `
    --region $REGION
```

## Troubleshooting

### Build Fails

1. Check Cloud Build logs:
   ```powershell
   gcloud builds list --limit=5
   gcloud builds log [BUILD_ID]
   ```

2. Verify Dockerfiles are correct
3. Check that all source files are present

### Service Won't Start

1. Check service logs (see Monitoring section)
2. Verify environment variables are set correctly
3. Check that the obfuscator binary exists in the container

### Domain Not Working

1. Verify domain mapping:
   ```powershell
   gcloud run domain-mappings describe obfuscator.live --region $REGION
   ```

2. Check DNS propagation:
   ```powershell
   nslookup obfuscator.live
   ```

3. Wait 24-48 hours for DNS propagation

### Obfuscation Fails

1. Check backend logs for errors
2. Verify `clang` and `llvm-obfuscator` are accessible
3. Check file permissions in the container

## Cost Optimization

1. **Set min-instances to 0** for development (saves costs when idle)
2. **Use Cloud Storage** for temporary files
3. **Enable request-based scaling** (default)
4. **Monitor usage** in Cloud Console

## Security

1. **Enable Cloud Armor** for DDoS protection
2. **Use Secret Manager** for sensitive data
3. **Enable audit logging**
4. **Set up IAM roles** with least privilege

## Support

- GCP Console: https://console.cloud.google.com
- Cloud Run Docs: https://cloud.google.com/run/docs
- Cloud Build Docs: https://cloud.google.com/build/docs

