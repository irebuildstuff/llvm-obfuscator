# Frontend Deployment Guide - Vercel

This guide will help you deploy the Next.js frontend to Vercel.

## Prerequisites

1. A GitHub account with the repository pushed
2. A Vercel account (free tier is fine)
3. Your backend URL: `https://llvm-obfuscator.onrender.com`

## Step-by-Step Deployment

### Option 1: Deploy via Vercel Dashboard (Recommended)

1. **Go to Vercel**
   - Visit https://vercel.com
   - Sign in with your GitHub account

2. **Import Your Project**
   - Click "Add New..." → "Project"
   - Select your GitHub repository: `irebuildstuff/llvm-obfuscator`
   - Vercel will auto-detect it's a Next.js project

3. **Configure Project Settings**
   - **Framework Preset**: Next.js (auto-detected)
   - **Root Directory**: `frontend` (IMPORTANT: Set this!)
   - **Build Command**: `npm run build` (default)
   - **Output Directory**: `.next` (default)
   - **Install Command**: `npm install` (default)

4. **Set Environment Variables**
   - Click "Environment Variables"
   - Add the following:
     ```
     Name: NEXT_PUBLIC_API_URL
     Value: https://llvm-obfuscator.onrender.com
     Environment: Production, Preview, Development (select all)
     ```
   - Click "Save"

5. **Deploy**
   - Click "Deploy"
   - Wait for the build to complete (usually 2-3 minutes)
   - Your site will be live at: `https://your-project-name.vercel.app`

6. **Custom Domain (Optional)**
   - Go to Project Settings → Domains
   - Add your domain: `obfuscator.live`
   - Follow Vercel's DNS configuration instructions

### Option 2: Deploy via Vercel CLI

1. **Install Vercel CLI**
   ```bash
   npm install -g vercel
   ```

2. **Login to Vercel**
   ```bash
   vercel login
   ```

3. **Navigate to Frontend Directory**
   ```bash
   cd frontend
   ```

4. **Deploy**
   ```bash
   vercel
   ```
   - Follow the prompts:
     - Set up and deploy? **Yes**
     - Which scope? (Select your account)
     - Link to existing project? **No** (first time) or **Yes** (updates)
     - What's your project's name? (Press Enter for default)
     - In which directory is your code located? `./` (current directory)
     - Want to override the settings? **No**

5. **Set Environment Variable**
   ```bash
   vercel env add NEXT_PUBLIC_API_URL
   ```
   - Enter value: `https://llvm-obfuscator.onrender.com`
   - Select environments: Production, Preview, Development

6. **Redeploy with Environment Variable**
   ```bash
   vercel --prod
   ```

## Configuration Files

### vercel.json
The project already includes a `vercel.json` file that configures API rewrites:

```json
{
  "rewrites": [
    {
      "source": "/api/:path*",
      "destination": "https://llvm-obfuscator.onrender.com/api/:path*"
    }
  ]
}
```

This allows the frontend to proxy API requests to the backend.

## Environment Variables

### Required
- `NEXT_PUBLIC_API_URL`: Your Render backend URL
  - Value: `https://llvm-obfuscator.onrender.com`

### Optional
- None required for basic functionality

## Post-Deployment Checklist

1. ✅ Verify the site loads: Visit your Vercel URL
2. ✅ Test file upload: Try uploading a C/C++ file
3. ✅ Test obfuscation: Verify the obfuscation process works
4. ✅ Test download: Verify `.ll` file download works
5. ✅ Test compilation instructions: Verify instructions display correctly

## Troubleshooting

### Build Fails
- **Error**: "Module not found"
  - **Solution**: Ensure Root Directory is set to `frontend` in Vercel settings

### API Calls Fail
- **Error**: "Failed to fetch"
  - **Solution**: 
    1. Verify `NEXT_PUBLIC_API_URL` is set correctly
    2. Check backend is running: `https://llvm-obfuscator.onrender.com/health`
    3. Check CORS settings in backend (should allow all origins)

### Environment Variable Not Working
- **Error**: API URL is undefined
  - **Solution**: 
    1. Environment variables starting with `NEXT_PUBLIC_` must be set in Vercel
    2. Redeploy after adding environment variables
    3. Clear browser cache

## Custom Domain Setup

1. **In Vercel Dashboard**
   - Go to Project Settings → Domains
   - Add domain: `obfuscator.live`
   - Add subdomain: `www.obfuscator.live` (optional)

2. **In GoDaddy (or your domain provider)**
   - Go to DNS Management
   - Add/Update DNS records as shown in Vercel:
     - Type: `A` or `CNAME`
     - Name: `@` or `www`
     - Value: (Vercel will provide this)

3. **Wait for DNS Propagation**
   - Usually takes 5-60 minutes
   - Check with: `nslookup obfuscator.live`

## Quick Deploy Command

If you've already set up the project once, you can redeploy with:

```bash
cd frontend
vercel --prod
```

## Support

- Vercel Docs: https://vercel.com/docs
- Next.js Deployment: https://nextjs.org/docs/deployment
- Render Backend: https://llvm-obfuscator.onrender.com

