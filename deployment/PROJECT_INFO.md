# GCP Project Information

## Project Details

- **Project ID**: `llvm-obfuscator-20251226144255`
- **Project Name**: LLVM Obfuscator
- **Email**: akashbashir244i2@gmail.com
- **Region**: us-central1 (recommended)

## Next Steps

### 1. Enable Billing (REQUIRED)

**IMPORTANT**: You must enable billing before deploying!

1. Go to: https://console.cloud.google.com/billing?project=llvm-obfuscator-20251226144255
2. Link a billing account to this project
3. If you don't have a billing account, create one first

### 2. Deploy the Application

Once billing is enabled, run:

```powershell
.\deployment\deploy.ps1 -ProjectId "llvm-obfuscator-20251226144255" -Region "us-central1"
```

### 3. Map Your Domain

After deployment completes:

```powershell
gcloud run domain-mappings create `
    --service llvm-obfuscator-frontend `
    --domain obfuscator.live `
    --region us-central1
```

### 4. Update GoDaddy DNS

Follow the DNS instructions provided after domain mapping.

## Quick Commands

```powershell
# Set project
gcloud config set project llvm-obfuscator-20251226144255

# View project info
gcloud projects describe llvm-obfuscator-20251226144255

# Check billing status
gcloud billing projects describe llvm-obfuscator-20251226144255

# View services
gcloud run services list --region us-central1
```

