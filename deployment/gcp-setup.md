# GCP Deployment Guide for LLVM Code Obfuscator

This guide provides step-by-step instructions for deploying the LLVM Code Obfuscator web application to Google Cloud Platform.

## Prerequisites

1. **Google Cloud Account** with billing enabled
2. **gcloud CLI** installed and configured
3. **Docker** installed locally
4. **Built obfuscator binary** (`llvm-obfuscator.exe` or `llvm-obfuscator`)

## Step 1: Set Up GCP Project

```bash
# Set your project ID
export PROJECT_ID="your-project-id"
export REGION="us-central1"

# Create a new project (optional)
gcloud projects create $PROJECT_ID

# Set the project
gcloud config set project $PROJECT_ID

# Enable required APIs
gcloud services enable \
  cloudbuild.googleapis.com \
  run.googleapis.com \
  artifactregistry.googleapis.com \
  storage.googleapis.com
```

## Step 2: Create Artifact Registry

```bash
# Create Artifact Registry repository
gcloud artifacts repositories create llvm-obfuscator \
  --repository-format=docker \
  --location=$REGION \
  --description="Docker repository for LLVM Obfuscator"

# Configure Docker authentication
gcloud auth configure-docker ${REGION}-docker.pkg.dev
```

## Step 3: Create Cloud Storage Bucket

```bash
# Create bucket for temporary files
gsutil mb -p $PROJECT_ID -c STANDARD -l $REGION gs://${PROJECT_ID}-obfuscator-temp

# Set lifecycle policy to auto-delete files after 24 hours
cat > lifecycle.json <<EOF
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
EOF
gsutil lifecycle set lifecycle.json gs://${PROJECT_ID}-obfuscator-temp
```

## Step 4: Build and Push Docker Images

### Option A: Using Cloud Build (Recommended)

```bash
# Submit build to Cloud Build
gcloud builds submit --config=deployment/cloudbuild.yaml \
  --substitutions=_REGION=$REGION,_REPO_NAME=llvm-obfuscator
```

### Option B: Manual Build and Push

```bash
# Build backend image
docker build -t ${REGION}-docker.pkg.dev/${PROJECT_ID}/llvm-obfuscator/backend:latest ./backend

# Build frontend image
docker build -t ${REGION}-docker.pkg.dev/${PROJECT_ID}/llvm-obfuscator/frontend:latest ./frontend

# Push images
docker push ${REGION}-docker.pkg.dev/${PROJECT_ID}/llvm-obfuscator/backend:latest
docker push ${REGION}-docker.pkg.dev/${PROJECT_ID}/llvm-obfuscator/frontend:latest
```

## Step 5: Deploy to Cloud Run

### Deploy Backend

```bash
gcloud run deploy llvm-obfuscator-backend \
  --image ${REGION}-docker.pkg.dev/${PROJECT_ID}/llvm-obfuscator/backend:latest \
  --region $REGION \
  --platform managed \
  --allow-unauthenticated \
  --memory 2Gi \
  --cpu 2 \
  --timeout 300 \
  --min-instances 1 \
  --max-instances 10 \
  --set-env-vars="CLANG_PATH=clang,OBFUSCATOR_PATH=/app/llvm-obfuscator"
```

### Deploy Frontend

```bash
# Get backend URL
BACKEND_URL=$(gcloud run services describe llvm-obfuscator-backend \
  --region $REGION \
  --format 'value(status.url)')

# Deploy frontend
gcloud run deploy llvm-obfuscator-frontend \
  --image ${REGION}-docker.pkg.dev/${PROJECT_ID}/llvm-obfuscator/frontend:latest \
  --region $REGION \
  --platform managed \
  --allow-unauthenticated \
  --memory 512Mi \
  --cpu 1 \
  --set-env-vars="NEXT_PUBLIC_API_URL=${BACKEND_URL}"
```

## Step 6: Set Up IAM Permissions

```bash
# Allow unauthenticated access (for public web app)
gcloud run services add-iam-policy-binding llvm-obfuscator-backend \
  --region $REGION \
  --member="allUsers" \
  --role="roles/run.invoker"

gcloud run services add-iam-policy-binding llvm-obfuscator-frontend \
  --region $REGION \
  --member="allUsers" \
  --role="roles/run.invoker"
```

## Step 7: Configure Custom Domain (Optional)

```bash
# Map custom domain to Cloud Run service
gcloud run domain-mappings create \
  --service llvm-obfuscator-frontend \
  --domain your-domain.com \
  --region $REGION
```

## Step 8: Set Up CI/CD (Optional)

### Create Cloud Build Trigger

```bash
# Connect repository (GitHub, GitLab, etc.)
gcloud builds triggers create github \
  --name="llvm-obfuscator-deploy" \
  --repo-name="your-repo" \
  --repo-owner="your-username" \
  --branch-pattern="^main$" \
  --build-config="deployment/cloudbuild.yaml"
```

## Environment Variables

### Backend Environment Variables

- `PORT`: Server port (default: 3001)
- `CLANG_PATH`: Path to clang executable (default: clang)
- `OBFUSCATOR_PATH`: Path to obfuscator binary
- `MAX_FILE_SIZE`: Maximum file upload size in bytes (default: 10485760)
- `JOB_CLEANUP_INTERVAL`: Job cleanup interval in milliseconds
- `JOB_EXPIRY_TIME`: Job expiry time in milliseconds

### Frontend Environment Variables

- `NEXT_PUBLIC_API_URL`: Backend API URL
- `NODE_ENV`: Environment (production)

## Monitoring and Logging

```bash
# View logs
gcloud run services logs read llvm-obfuscator-backend --region $REGION
gcloud run services logs read llvm-obfuscator-frontend --region $REGION

# View metrics in Cloud Console
# Navigate to Cloud Run > Services > Metrics
```

## Troubleshooting

### Common Issues

1. **Build fails**: Check that all dependencies are installed in Dockerfile
2. **Service won't start**: Check logs for errors
3. **File upload fails**: Verify file size limits and permissions
4. **Obfuscation fails**: Ensure obfuscator binary is correctly mounted/copied

### Debug Commands

```bash
# Check service status
gcloud run services describe llvm-obfuscator-backend --region $REGION

# View recent logs
gcloud run services logs read llvm-obfuscator-backend --region $REGION --limit 50

# Test health endpoint
curl https://llvm-obfuscator-backend-REGION-PROJECT_ID.a.run.app/health
```

## Cost Optimization

1. **Set min-instances to 0** for development (saves costs)
2. **Use Cloud Storage** for temporary files instead of container storage
3. **Enable request-based scaling** to handle traffic spikes
4. **Use Cloud CDN** for frontend static assets

## Security Considerations

1. **Enable Cloud Armor** for DDoS protection
2. **Set up VPC** for private networking (if needed)
3. **Use Secret Manager** for sensitive configuration
4. **Enable audit logging** for compliance
5. **Set up Cloud IAM** roles with least privilege

## Scaling Configuration

- **Backend**: 1-10 instances, 2 CPU, 2GB memory
- **Frontend**: 1-5 instances, 1 CPU, 512MB memory
- **Auto-scaling**: Based on request count and CPU utilization

## Maintenance

### Update Deployment

```bash
# Rebuild and redeploy
gcloud builds submit --config=deployment/cloudbuild.yaml
```

### Rollback

```bash
# Rollback to previous revision
gcloud run services update-traffic llvm-obfuscator-backend \
  --to-revisions=REVISION_NAME=100 \
  --region $REGION
```

## Support

For issues or questions:
- Check Cloud Run logs
- Review Cloud Build logs
- Check service health endpoints
- Review GCP documentation


