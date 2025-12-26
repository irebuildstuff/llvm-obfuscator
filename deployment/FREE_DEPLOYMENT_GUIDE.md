# Free Deployment Guide - Vercel + Render

This guide will help you deploy your LLVM Obfuscator app completely free using:
- **Vercel** (Frontend) - Free tier, perfect for Next.js
- **Render** (Backend) - Free tier with Docker support

## Prerequisites

1. GitHub account (free)
2. Vercel account (free) - Sign up at https://vercel.com
3. Render account (free) - Sign up at https://render.com
4. GoDaddy domain: `obfuscator.live`

## Step 1: Push Code to GitHub

1. Create a new repository on GitHub
2. Push your code:

```bash
git init
git add .
git commit -m "Initial commit"
git branch -M main
git remote add origin https://github.com/YOUR_USERNAME/llvm-obfuscator.git
git push -u origin main
```

## Step 2: Deploy Backend to Render

### 2.1 Create Render Service

1. Go to https://dashboard.render.com
2. Click "New +" → "Web Service"
3. Connect your GitHub repository
4. Configure:
   - **Name**: `llvm-obfuscator-backend`
   - **Region**: `Oregon (US West)`
   - **Branch**: `main`
   - **Root Directory**: Leave empty
   - **Environment**: `Docker`
   - **Dockerfile Path**: `backend/Dockerfile`
   - **Docker Context**: `.` (root directory)
   - **Plan**: `Free`

### 2.2 Environment Variables

Add these environment variables in Render dashboard:

```
NODE_ENV=production
PORT=3001
CLANG_PATH=clang
OBFUSCATOR_PATH=/app/llvm-obfuscator
```

### 2.3 Deploy

Click "Create Web Service" and wait for deployment (5-10 minutes for first build).

### 2.4 Get Backend URL

After deployment, copy your backend URL (e.g., `https://llvm-obfuscator-backend.onrender.com`)

## Step 3: Deploy Frontend to Vercel

### 3.1 Import Project

1. Go to https://vercel.com/new
2. Import your GitHub repository
3. Configure:
   - **Framework Preset**: Next.js (auto-detected)
   - **Root Directory**: `frontend`
   - **Build Command**: `npm run build` (auto-detected)
   - **Output Directory**: `.next` (auto-detected)

### 3.2 Environment Variables

Add this environment variable:

```
NEXT_PUBLIC_API_URL=https://llvm-obfuscator-backend.onrender.com
```

(Replace with your actual Render backend URL)

### 3.3 Deploy

Click "Deploy" and wait for build (2-3 minutes).

### 3.4 Get Frontend URL

After deployment, copy your frontend URL (e.g., `https://llvm-obfuscator.vercel.app`)

## Step 4: Configure Custom Domain

### 4.1 Vercel Domain Setup

1. Go to your project settings → Domains
2. Add `obfuscator.live`
3. Vercel will provide DNS records

### 4.2 GoDaddy DNS Configuration

In GoDaddy DNS settings, add:

**For Root Domain (obfuscator.live):**
- Type: `A`
- Name: `@`
- Value: `76.76.21.21` (Vercel's IP - check Vercel dashboard for current IP)

**OR use CNAME:**
- Type: `CNAME`
- Name: `@`
- Value: `cname.vercel-dns.com` (check Vercel dashboard for exact value)

**For www subdomain:**
- Type: `CNAME`
- Name: `www`
- Value: `cname.vercel-dns.com`

### 4.3 Wait for DNS Propagation

DNS changes can take 24-48 hours, but usually work within 1-2 hours.

## Step 5: Update Frontend API URL

After backend is deployed, update the frontend environment variable:

1. Go to Vercel dashboard → Your project → Settings → Environment Variables
2. Update `NEXT_PUBLIC_API_URL` to your Render backend URL
3. Redeploy (or it will auto-redeploy)

## Free Tier Limits

### Vercel (Frontend)
- ✅ Unlimited deployments
- ✅ 100GB bandwidth/month
- ✅ Automatic HTTPS
- ✅ Custom domains
- ✅ No credit card required

### Render (Backend)
- ✅ 750 hours/month (enough for 24/7)
- ✅ 512MB RAM
- ✅ 0.5GB disk space
- ⚠️ Spins down after 15 minutes of inactivity (takes ~30 seconds to wake up)
- ✅ Automatic HTTPS
- ✅ Custom domains (paid feature, but you can use Render's free subdomain)

## Important Notes

### Render Free Tier Limitations

1. **Cold Starts**: Free tier services spin down after 15 minutes of inactivity. First request after spin-down takes ~30 seconds.

2. **Build Time**: First build takes 10-15 minutes (compiling LLVM/Clang). Subsequent builds are faster.

3. **Memory**: 512MB RAM might be tight for heavy obfuscation. Consider upgrading if needed.

### Optimization Tips

1. **Keep Backend Warm**: Use a cron job (like cron-job.org) to ping your backend every 10 minutes to prevent spin-down.

2. **Monitor Usage**: Check Render dashboard for resource usage.

3. **Upgrade if Needed**: If you exceed free tier, Render's paid plans start at $7/month.

## Troubleshooting

### Backend Not Responding

- Check Render logs: Dashboard → Your service → Logs
- Verify environment variables are set correctly
- Check if service is running (not crashed)

### Frontend Can't Connect to Backend

- Verify `NEXT_PUBLIC_API_URL` is set correctly in Vercel
- Check CORS settings in backend (should allow Vercel domain)
- Verify backend is accessible (visit backend URL directly)

### Build Failures

- **Backend**: Check Docker build logs in Render
- **Frontend**: Check build logs in Vercel
- Ensure all dependencies are in `package.json`

### Domain Not Working

- Wait 24-48 hours for DNS propagation
- Verify DNS records in GoDaddy match Vercel's requirements
- Check domain status in Vercel dashboard

## Alternative: Railway (Backend)

If Render doesn't work, you can use Railway instead:

1. Sign up at https://railway.app
2. Create new project from GitHub
3. Add Docker service
4. Use `backend/Dockerfile`
5. Set environment variables
6. Deploy

Railway free tier:
- $5 credit/month
- Pay-as-you-go after credit
- No spin-down (always on)

## Support

- Vercel Docs: https://vercel.com/docs
- Render Docs: https://render.com/docs
- Railway Docs: https://docs.railway.app

