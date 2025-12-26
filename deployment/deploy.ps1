# GCP Deployment Script for LLVM Obfuscator (PowerShell)
# Usage: .\deployment\deploy.ps1 -ProjectId "your-project-id" -Region "us-central1"

param(
    [string]$ProjectId = "your-project-id",
    [string]$Region = "us-central1",
    [string]$Domain = "obfuscator.live"
)

$ErrorActionPreference = "Stop"

$RepoName = "llvm-obfuscator"
$BackendService = "llvm-obfuscator-backend"
$FrontendService = "llvm-obfuscator-frontend"
$BucketName = "$ProjectId-obfuscator-temp"

Write-Host "🚀 Starting GCP Deployment for LLVM Obfuscator" -ForegroundColor Cyan
Write-Host "Project ID: $ProjectId" -ForegroundColor Green
Write-Host "Region: $Region" -ForegroundColor Green
Write-Host "Domain: $Domain" -ForegroundColor Green
Write-Host ""

# Check if gcloud is installed
if (-not (Get-Command gcloud -ErrorAction SilentlyContinue)) {
    Write-Host "❌ Error: gcloud CLI is not installed" -ForegroundColor Red
    Write-Host "Install it from: https://cloud.google.com/sdk/docs/install" -ForegroundColor Yellow
    exit 1
}

# Set project
Write-Host "📋 Setting GCP project..." -ForegroundColor Cyan
gcloud config set project $ProjectId

# Enable required APIs
Write-Host "🔧 Enabling required APIs..." -ForegroundColor Cyan
gcloud services enable `
    cloudbuild.googleapis.com `
    run.googleapis.com `
    artifactregistry.googleapis.com `
    storage.googleapis.com `
    cloudresourcemanager.googleapis.com `
    --quiet

# Create Artifact Registry repository
Write-Host "📦 Creating Artifact Registry repository..." -ForegroundColor Cyan
$repoExists = gcloud artifacts repositories describe $RepoName --location=$Region 2>&1
if ($LASTEXITCODE -ne 0) {
    gcloud artifacts repositories create $RepoName `
        --repository-format=docker `
        --location=$Region `
        --description="Docker repository for LLVM Obfuscator" `
        --quiet
    Write-Host "✅ Artifact Registry repository created" -ForegroundColor Green
} else {
    Write-Host "✅ Artifact Registry repository already exists" -ForegroundColor Green
}

# Configure Docker authentication
Write-Host "🔐 Configuring Docker authentication..." -ForegroundColor Cyan
gcloud auth configure-docker "${Region}-docker.pkg.dev" --quiet

# Create Cloud Storage bucket
Write-Host "🗄️  Creating Cloud Storage bucket..." -ForegroundColor Cyan
$bucketExists = gsutil ls -b "gs://$BucketName" 2>&1
if ($LASTEXITCODE -ne 0) {
    gsutil mb -p $ProjectId -c STANDARD -l $Region "gs://$BucketName"
    Write-Host "✅ Bucket created" -ForegroundColor Green
} else {
    Write-Host "✅ Bucket already exists" -ForegroundColor Green
}

# Set lifecycle policy
Write-Host "⚙️  Setting bucket lifecycle policy..." -ForegroundColor Cyan
$lifecycleJson = @"
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
"@
$lifecycleJson | Out-File -FilePath "$env:TEMP\lifecycle.json" -Encoding utf8
gsutil lifecycle set "$env:TEMP\lifecycle.json" "gs://$BucketName"
Remove-Item "$env:TEMP\lifecycle.json"

# Build and push images using Cloud Build
Write-Host "🏗️  Building and pushing Docker images..." -ForegroundColor Cyan
gcloud builds submit --config=deployment/cloudbuild.yaml `
    --substitutions="_REGION=$Region,_REPO_NAME=$RepoName" `
    --quiet

# Get image URLs
$BackendImage = "${Region}-docker.pkg.dev/${ProjectId}/${RepoName}/backend:latest"
$FrontendImage = "${Region}-docker.pkg.dev/${ProjectId}/${RepoName}/frontend:latest"

# Deploy backend
Write-Host "🚀 Deploying backend service..." -ForegroundColor Cyan
gcloud run deploy $BackendService `
    --image $BackendImage `
    --region $Region `
    --platform managed `
    --allow-unauthenticated `
    --memory 2Gi `
    --cpu 2 `
    --timeout 300 `
    --min-instances 1 `
    --max-instances 10 `
    --set-env-vars="CLANG_PATH=clang,OBFUSCATOR_PATH=/app/llvm-obfuscator,NODE_ENV=production" `
    --quiet

# Get backend URL
$BackendUrl = gcloud run services describe $BackendService `
    --region $Region `
    --format 'value(status.url)'

Write-Host "✅ Backend deployed at: $BackendUrl" -ForegroundColor Green

# Deploy frontend
Write-Host "🚀 Deploying frontend service..." -ForegroundColor Cyan
gcloud run deploy $FrontendService `
    --image $FrontendImage `
    --region $Region `
    --platform managed `
    --allow-unauthenticated `
    --memory 512Mi `
    --cpu 1 `
    --timeout 60 `
    --min-instances 1 `
    --max-instances 5 `
    --set-env-vars="NEXT_PUBLIC_API_URL=${BackendUrl},NODE_ENV=production" `
    --quiet

# Get frontend URL
$FrontendUrl = gcloud run services describe $FrontendService `
    --region $Region `
    --format 'value(status.url)'

Write-Host "✅ Frontend deployed at: $FrontendUrl" -ForegroundColor Green

# Set up IAM permissions
Write-Host "🔐 Setting up IAM permissions..." -ForegroundColor Cyan
gcloud run services add-iam-policy-binding $BackendService `
    --region $Region `
    --member="allUsers" `
    --role="roles/run.invoker" `
    --quiet

gcloud run services add-iam-policy-binding $FrontendService `
    --region $Region `
    --member="allUsers" `
    --role="roles/run.invoker" `
    --quiet

Write-Host ""
Write-Host "✅ Deployment completed successfully!" -ForegroundColor Green
Write-Host ""
Write-Host "📊 Service URLs:" -ForegroundColor Cyan
Write-Host "   Backend:  $BackendUrl" -ForegroundColor White
Write-Host "   Frontend: $FrontendUrl" -ForegroundColor White
Write-Host ""
Write-Host "🌐 Next steps:" -ForegroundColor Cyan
Write-Host "   1. Map your domain ($Domain) to the frontend service:" -ForegroundColor Yellow
$domainCmd = "gcloud run domain-mappings create --service $FrontendService --domain $Domain --region $Region"
Write-Host "      $domainCmd" -ForegroundColor Gray
Write-Host ""
Write-Host "   2. Update DNS records in GoDaddy:" -ForegroundColor Yellow
Write-Host "      Add a CNAME record pointing $Domain to: $FrontendUrl" -ForegroundColor Gray
Write-Host ""
Write-Host "   3. View logs:" -ForegroundColor Yellow
$frontendLogCmd = "gcloud run services logs read $FrontendService --region $Region"
$backendLogCmd = "gcloud run services logs read $BackendService --region $Region"
Write-Host "      $frontendLogCmd" -ForegroundColor Gray
Write-Host "      $backendLogCmd" -ForegroundColor Gray

