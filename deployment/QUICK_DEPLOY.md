# Quick Deployment Guide - obfuscator.live

## Prerequisites Checklist

- [ ] Google Cloud account with billing enabled
- [ ] gcloud CLI installed
- [ ] Domain `obfuscator.live` purchased from GoDaddy
- [ ] GCP project created

## One-Command Deployment (PowerShell)

```powershell
# Replace with your actual project ID
.\deployment\deploy.ps1 -ProjectId "your-gcp-project-id" -Region "us-central1"
```

## Manual Steps (If Script Fails)

### 1. Authenticate
```powershell
gcloud auth login
gcloud config set project YOUR_PROJECT_ID
```

### 2. Enable APIs
```powershell
gcloud services enable cloudbuild.googleapis.com run.googleapis.com artifactregistry.googleapis.com storage.googleapis.com
```

### 3. Deploy
```powershell
gcloud builds submit --config=deployment/cloudbuild.yaml --substitutions="_REGION=us-central1,_REPO_NAME=llvm-obfuscator"
```

### 4. Map Domain
```powershell
gcloud run domain-mappings create --service llvm-obfuscator-frontend --domain obfuscator.live --region us-central1
```

### 5. Update GoDaddy DNS
- Add CNAME record: `@` → Cloud Run URL (provided after domain mapping)

## Get Service URLs

```powershell
# Backend
gcloud run services describe llvm-obfuscator-backend --region us-central1 --format 'value(status.url)'

# Frontend  
gcloud run services describe llvm-obfuscator-frontend --region us-central1 --format 'value(status.url)'
```

## Troubleshooting

**Build fails?**
- Check: `gcloud builds list --limit=5`
- View logs: `gcloud builds log [BUILD_ID]`

**Service won't start?**
- Check logs: `gcloud run services logs read llvm-obfuscator-backend --region us-central1`

**Domain not working?**
- Wait 24-48 hours for DNS propagation
- Verify: `gcloud run domain-mappings describe obfuscator.live --region us-central1`

