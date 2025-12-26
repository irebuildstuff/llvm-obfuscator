# GCP Project Setup Script
# This script will authenticate and create a new GCP project

param(
    [string]$Email = "akashbashir244i2@gmail.com",
    [string]$ProjectName = "llvm-obfuscator",
    [string]$Region = "us-central1"
)

Write-Host "🔐 Setting up GCP Project" -ForegroundColor Cyan
Write-Host "Email: $Email" -ForegroundColor Green
Write-Host "Project Name: $ProjectName" -ForegroundColor Green
Write-Host ""

# Check if gcloud is installed
if (-not (Get-Command gcloud -ErrorAction SilentlyContinue)) {
    Write-Host "❌ Error: gcloud CLI is not installed" -ForegroundColor Red
    Write-Host "Please install from: https://cloud.google.com/sdk/docs/install" -ForegroundColor Yellow
    exit 1
}

# Authenticate with the specified email
Write-Host "🔑 Authenticating with Google Cloud..." -ForegroundColor Cyan
gcloud auth login $Email

# Generate a unique project ID (GCP project IDs must be globally unique)
$timestamp = Get-Date -Format "yyyyMMddHHmmss"
$ProjectId = "$ProjectName-$timestamp"

Write-Host ""
Write-Host "📦 Creating new GCP project..." -ForegroundColor Cyan
Write-Host "Project ID: $ProjectId" -ForegroundColor Yellow

# Create the project
gcloud projects create $ProjectId --name=$ProjectName

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Failed to create project. It may already exist or the ID is taken." -ForegroundColor Red
    Write-Host "Try a different project name or use an existing project." -ForegroundColor Yellow
    exit 1
}

# Set the project as active
Write-Host "⚙️  Setting active project..." -ForegroundColor Cyan
gcloud config set project $ProjectId

# Enable billing (user needs to do this manually)
Write-Host ""
Write-Host "💰 IMPORTANT: Enable Billing" -ForegroundColor Yellow
Write-Host "1. Go to: https://console.cloud.google.com/billing?project=$ProjectId" -ForegroundColor White
Write-Host "2. Link a billing account to this project" -ForegroundColor White
Write-Host "3. Press Enter once billing is enabled..." -ForegroundColor White
Read-Host

# Enable required APIs
Write-Host ""
Write-Host "🔧 Enabling required APIs..." -ForegroundColor Cyan
gcloud services enable `
    cloudbuild.googleapis.com `
    run.googleapis.com `
    artifactregistry.googleapis.com `
    storage.googleapis.com `
    cloudresourcemanager.googleapis.com `
    --quiet

Write-Host ""
Write-Host "✅ Project setup complete!" -ForegroundColor Green
Write-Host ""
Write-Host "📊 Project Details:" -ForegroundColor Cyan
Write-Host "   Project ID: $ProjectId" -ForegroundColor White
Write-Host "   Project Name: $ProjectName" -ForegroundColor White
Write-Host "   Region: $Region" -ForegroundColor White
Write-Host ""
Write-Host "🚀 Ready to deploy! Run:" -ForegroundColor Cyan
Write-Host "   .\deployment\deploy.ps1 -ProjectId `"$ProjectId`" -Region `"$Region`"" -ForegroundColor Yellow

