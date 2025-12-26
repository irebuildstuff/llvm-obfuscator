# Render Backend Setup Checklist

## Current Configuration ✅

- **Name**: `llvm-obfuscator` (or `llvm-obfuscator-backend` - either works)
- **Language**: `Docker` ✅
- **Branch**: `master` ✅
- **Region**: `Oregon (US West)` ✅
- **Root Directory**: (empty) ✅
- **Dockerfile Path**: `backend/Dockerfile` ✅

## Next Steps to Complete

### 1. Instance Type
- Select: **"Free"** plan
- This gives you 512MB RAM and 0.5GB disk (enough for this app)

### 2. Environment Variables (IMPORTANT!)
Click "Advanced" or scroll down to find "Environment Variables" section.

Add these 4 variables:

```
NODE_ENV=production
PORT=3001
CLANG_PATH=clang
OBFUSCATOR_PATH=/app/llvm-obfuscator
```

**How to add:**
- Click "Add Environment Variable"
- Enter name and value for each one
- Repeat for all 4 variables

### 3. Health Check Path (Optional but Recommended)
- **Health Check Path**: `/health`
- This helps Render know if your service is running

### 4. Auto-Deploy
- ✅ **Auto-Deploy**: Enabled (default)
- This automatically redeploys when you push to GitHub

### 5. Final Step
- Click **"Create Web Service"** button at the bottom
- Wait 10-15 minutes for first build
- Watch the build logs to see progress

## What Happens Next

1. **Build Phase** (10-15 min):
   - Clones your GitHub repo
   - Builds Docker image
   - Compiles LLVM/Clang (this takes time)
   - Installs Node.js dependencies
   - Builds the obfuscator binary

2. **Deploy Phase** (1-2 min):
   - Starts the container
   - Runs health checks
   - Service goes "Live"

3. **Get Your URL**:
   - Once "Live", you'll see:
   ```
   Your service is live at:
   https://llvm-obfuscator.onrender.com
   ```
   - Copy this URL for Vercel!

## Troubleshooting

### Build Fails?
- Check build logs in Render dashboard
- Common issues:
  - Dockerfile path incorrect
  - Missing environment variables
  - Build timeout (free tier has limits)

### Service Won't Start?
- Check logs tab
- Verify environment variables are set
- Check health endpoint: `https://your-service.onrender.com/health`

### Need to Update?
- Just push to GitHub, Render auto-deploys
- Or manually trigger from dashboard

## Quick Reference

**Render Dashboard**: https://dashboard.render.com
**Your Service URL**: `https://llvm-obfuscator.onrender.com` (or your service name)
**Health Check**: `https://llvm-obfuscator.onrender.com/health`

