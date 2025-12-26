# How to Get Your Render Backend URL

## Step-by-Step Guide

### Step 1: Deploy Backend to Render

1. Go to https://dashboard.render.com
2. Click **"New +"** → **"Web Service"**
3. Connect your GitHub account (if not already connected)
4. Select your repository: `irebuildstuff/llvm-obfuscator`
5. Configure the service:
   - **Name**: `llvm-obfuscator-backend`
   - **Region**: `Oregon (US West)` or any region
   - **Branch**: `master` (or `main`)
   - **Root Directory**: Leave empty (or `.`)
   - **Environment**: Select **"Docker"**
   - **Dockerfile Path**: `backend/Dockerfile`
   - **Docker Context**: `.` (root directory)
   - **Plan**: **"Free"**

6. Add Environment Variables (click "Advanced"):
   ```
   NODE_ENV=production
   PORT=3001
   CLANG_PATH=clang
   OBFUSCATOR_PATH=/app/llvm-obfuscator
   ```

7. Click **"Create Web Service"**

### Step 2: Wait for Deployment

- First build takes **10-15 minutes** (compiling LLVM/Clang)
- You'll see build logs in real-time
- Status will show "Building..." then "Live" when done

### Step 3: Get Your Backend URL

Once deployment is complete:

1. In your Render dashboard, click on your service: `llvm-obfuscator-backend`
2. At the top of the page, you'll see:
   ```
   Your service is live at:
   https://llvm-obfuscator-backend.onrender.com
   ```
   (Your URL will be similar, but with your service name)

3. **Copy this URL** - this is your backend URL!

### Step 4: Use It in Vercel

When deploying frontend to Vercel:

1. Go to your Vercel project settings
2. Navigate to **"Environment Variables"**
3. Add new variable:
   - **Name**: `NEXT_PUBLIC_API_URL`
   - **Value**: `https://llvm-obfuscator-backend.onrender.com` (your actual Render URL)
   - **Environment**: All (Production, Preview, Development)
4. Save and redeploy

## Example

If your Render service is named `llvm-obfuscator-backend`, your URL will be:
```
https://llvm-obfuscator-backend.onrender.com
```

Then in Vercel, set:
```
NEXT_PUBLIC_API_URL=https://llvm-obfuscator-backend.onrender.com
```

## Important Notes

- ⚠️ **Free tier services spin down** after 15 minutes of inactivity
- First request after spin-down takes ~30 seconds to wake up
- To keep it warm, use a cron job (see deployment guide)

## Quick Reference

- Render Dashboard: https://dashboard.render.com
- Your service URL will be: `https://[your-service-name].onrender.com`
- Check service status in Render dashboard

