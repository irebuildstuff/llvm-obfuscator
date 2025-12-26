# GitHub Authentication Fix

## Issue
You're authenticated as `akashmir` but need to push to `irebuildstuff` repository.

## Solution: Use Personal Access Token

### Step 1: Create Personal Access Token

1. Go to: https://github.com/settings/tokens
2. Click **"Generate new token"** → **"Generate new token (classic)"**
3. Name: `llvm-obfuscator-deploy`
4. Expiration: Choose your preference (90 days recommended)
5. Select scopes:
   - ✅ `repo` (Full control of private repositories)
6. Click **"Generate token"**
7. **COPY THE TOKEN** (you won't see it again!)

### Step 2: Update Git Remote

Run these commands:

```powershell
# Switch back to HTTPS
git remote set-url origin https://github.com/irebuildstuff/llvm-obfuscator.git

# Push (it will prompt for username and password)
# Username: irebuildstuff
# Password: <paste your Personal Access Token>
git push -u origin master
```

### Alternative: Use Git Credential Manager

If you have Windows Credential Manager:

```powershell
# Clear old credentials
git credential-manager-core erase
host=github.com
protocol=https

# Then push (will prompt for new credentials)
git push -u origin master
```

### Alternative: Use SSH Key

If you prefer SSH:

1. Generate SSH key (if you don't have one):
   ```powershell
   ssh-keygen -t ed25519 -C "your_email@example.com"
   ```

2. Add SSH key to GitHub:
   - Copy public key: `cat ~/.ssh/id_ed25519.pub`
   - Go to: https://github.com/settings/keys
   - Click **"New SSH key"**
   - Paste your public key

3. Test connection:
   ```powershell
   ssh -T git@github.com
   ```

4. Push:
   ```powershell
   git push -u origin master
   ```

## Quick Fix (Recommended)

The easiest way is to use Personal Access Token:

1. Create token at: https://github.com/settings/tokens
2. Run:
   ```powershell
   git remote set-url origin https://github.com/irebuildstuff/llvm-obfuscator.git
   git push -u origin master
   ```
3. When prompted:
   - Username: `irebuildstuff`
   - Password: `<paste your token>`

