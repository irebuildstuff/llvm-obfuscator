# Quick Free Deployment Guide

Deploy your LLVM Obfuscator app for **FREE** using Vercel (frontend) + Render (backend).

## 🚀 Quick Start (15 minutes)

### Step 1: Push to GitHub

```bash
git init
git add .
git commit -m "Initial commit"
git remote add origin https://github.com/YOUR_USERNAME/llvm-obfuscator.git
git push -u origin main
```

### Step 2: Deploy Backend (Render)

1. Go to https://dashboard.render.com
2. Click **"New +"** → **"Web Service"**
3. Connect GitHub → Select your repo
4. Configure:
   - **Name**: `llvm-obfuscator-backend`
   - **Environment**: `Docker`
   - **Dockerfile Path**: `backend/Dockerfile`
   - **Docker Context**: `.` (root)
   - **Plan**: `Free`
5. Add Environment Variables:
   ```
   NODE_ENV=production
   PORT=3001
   CLANG_PATH=clang
   OBFUSCATOR_PATH=/app/llvm-obfuscator
   ```
6. Click **"Create Web Service"**
7. Wait 10-15 minutes for first build
8. **Copy your backend URL** (e.g., `https://llvm-obfuscator-backend.onrender.com`)

### Step 3: Deploy Frontend (Vercel)

1. Go to https://vercel.com/new
2. Import GitHub repo
3. Configure:
   - **Root Directory**: `frontend`
   - **Framework**: Next.js (auto)
4. Add Environment Variable:
   ```
   NEXT_PUBLIC_API_URL=https://llvm-obfuscator-backend.onrender.com
   ```
   (Replace with your actual Render URL)
5. Click **"Deploy"**
6. Wait 2-3 minutes
7. **Copy your frontend URL** (e.g., `https://llvm-obfuscator.vercel.app`)

### Step 4: Add Custom Domain (GoDaddy)

#### In Vercel:
1. Go to Project → Settings → Domains
2. Add `obfuscator.live`
3. Vercel shows DNS instructions

#### In GoDaddy:
1. Go to DNS Management
2. Add CNAME record:
   - **Type**: CNAME
   - **Name**: `@` (or leave blank for root)
   - **Value**: `cname.vercel-dns.com` (check Vercel for exact value)
3. Add www subdomain:
   - **Type**: CNAME
   - **Name**: `www`
   - **Value**: `cname.vercel-dns.com`

4. Wait 1-2 hours for DNS propagation

## ✅ Done!

Your app is now live at `obfuscator.live` (after DNS propagates)

## 📝 Important Notes

### Render Free Tier:
- ⚠️ Spins down after 15 min inactivity (30 sec wake-up)
- ✅ 750 hours/month (enough for 24/7)
- ✅ 512MB RAM

**Keep it warm**: Use https://cron-job.org to ping your backend every 10 minutes:
- URL: `https://your-backend.onrender.com/health`
- Schedule: Every 10 minutes

### Vercel Free Tier:
- ✅ Unlimited deployments
- ✅ 100GB bandwidth/month
- ✅ Always on (no spin-down)

## 🔧 Troubleshooting

**Backend not responding?**
- Check Render logs
- Verify environment variables
- First request after spin-down takes ~30 seconds

**Frontend can't connect?**
- Verify `NEXT_PUBLIC_API_URL` in Vercel settings
- Check backend is running (visit backend URL)

**Domain not working?**
- Wait 24-48 hours for DNS
- Verify DNS records match Vercel's requirements

## 💰 Cost: $0/month

Both services are completely free for this use case!

