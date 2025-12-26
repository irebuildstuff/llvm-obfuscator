# GCP Billing Setup Guide

## Current Status

- **Project ID**: `llvm-obfuscator-20251226144255`
- **Billing Account**: `01677E-DBC196-B83E3E` (Free Trial Account)
- **Status**: Linked but may need payment method activation

## Free Trial Account Requirements

GCP Free Trial accounts require:

1. **Valid Payment Method**: Even though you get $300 free credit, you must add a credit card
2. **Account Activation**: The account must be in "ACTIVE" state, not "delinquent"
3. **Propagation Time**: After adding payment method, wait 5-10 minutes for activation

## Steps to Fix Billing Issues

### 1. Check Billing Account Status

Visit: https://console.cloud.google.com/billing/01677E-DBC196-B83E3E

### 2. Add Payment Method (if needed)

1. Go to: https://console.cloud.google.com/billing/01677E-DBC196-B83E3E/payment
2. Click "Add Payment Method"
3. Enter your credit card details
4. **Note**: You won't be charged unless you exceed the free tier limits

### 3. Verify Account is Active

- Status should show "ACTIVE" (not "delinquent" or "closed")
- Payment method should show as "Valid"

### 4. Wait for Propagation

After adding/updating payment method:
- Wait 5-10 minutes for changes to propagate
- Then retry deployment

## Retry Deployment

Once billing is confirmed active:

```powershell
.\deployment\deploy.ps1 -ProjectId "llvm-obfuscator-20251226144255" -Region "us-central1"
```

## Free Trial Limits

- **$300 credit** for 90 days
- **Cloud Run**: 2 million requests/month free
- **Cloud Build**: 120 build-minutes/day free
- **Artifact Registry**: 0.5 GB storage free
- **Cloud Storage**: 5 GB free

These limits should be sufficient for initial deployment and testing.

## Troubleshooting

If you continue to see "delinquent" errors:

1. **Check payment method**: Ensure credit card is not expired
2. **Check account status**: Visit billing console to see exact status
3. **Contact support**: If issues persist, contact GCP support
4. **Try different billing account**: If you have another active billing account, link it instead

## Alternative: Use Existing Active Billing Account

If you have another billing account that's already active:

```powershell
gcloud billing projects link llvm-obfuscator-20251226144255 --billing-account=YOUR_ACTIVE_BILLING_ACCOUNT_ID
```

