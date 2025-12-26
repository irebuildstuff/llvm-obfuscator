#!/bin/bash

# GCP Deployment Script for LLVM Obfuscator
# Usage: ./deployment/deploy.sh [project-id] [region]

set -e

# Configuration
PROJECT_ID=${1:-"your-project-id"}
REGION=${2:-"us-central1"}
REPO_NAME="llvm-obfuscator"
BACKEND_SERVICE="llvm-obfuscator-backend"
FRONTEND_SERVICE="llvm-obfuscator-frontend"
DOMAIN="obfuscator.live"

echo "🚀 Starting GCP Deployment for LLVM Obfuscator"
echo "Project ID: $PROJECT_ID"
echo "Region: $REGION"
echo "Domain: $DOMAIN"
echo ""

# Check if gcloud is installed
if ! command -v gcloud &> /dev/null; then
    echo "❌ Error: gcloud CLI is not installed"
    echo "Install it from: https://cloud.google.com/sdk/docs/install"
    exit 1
fi

# Set project
echo "📋 Setting GCP project..."
gcloud config set project $PROJECT_ID

# Enable required APIs
echo "🔧 Enabling required APIs..."
gcloud services enable \
    cloudbuild.googleapis.com \
    run.googleapis.com \
    artifactregistry.googleapis.com \
    storage.googleapis.com \
    cloudresourcemanager.googleapis.com \
    --quiet

# Create Artifact Registry repository
echo "📦 Creating Artifact Registry repository..."
if ! gcloud artifacts repositories describe $REPO_NAME --location=$REGION &> /dev/null; then
    gcloud artifacts repositories create $REPO_NAME \
        --repository-format=docker \
        --location=$REGION \
        --description="Docker repository for LLVM Obfuscator" \
        --quiet
    echo "✅ Artifact Registry repository created"
else
    echo "✅ Artifact Registry repository already exists"
fi

# Configure Docker authentication
echo "🔐 Configuring Docker authentication..."
gcloud auth configure-docker ${REGION}-docker.pkg.dev --quiet

# Create Cloud Storage bucket for temporary files
BUCKET_NAME="${PROJECT_ID}-obfuscator-temp"
echo "🗄️  Creating Cloud Storage bucket..."
if ! gsutil ls -b gs://$BUCKET_NAME &> /dev/null; then
    gsutil mb -p $PROJECT_ID -c STANDARD -l $REGION gs://$BUCKET_NAME
    echo "✅ Bucket created"
else
    echo "✅ Bucket already exists"
fi

# Set lifecycle policy
echo "⚙️  Setting bucket lifecycle policy..."
cat > /tmp/lifecycle.json <<EOF
{
  "lifecycle": {
    "rule": [
      {
        "action": {"type": "Delete"},
        "condition": {"age": 1}
      }
    ]
  }
}
EOF
gsutil lifecycle set /tmp/lifecycle.json gs://$BUCKET_NAME
rm /tmp/lifecycle.json

# Build and push images using Cloud Build
echo "🏗️  Building and pushing Docker images..."
gcloud builds submit --config=deployment/cloudbuild.yaml \
    --substitutions=_REGION=$REGION,_REPO_NAME=$REPO_NAME \
    --quiet

# Get image URLs
BACKEND_IMAGE="${REGION}-docker.pkg.dev/${PROJECT_ID}/${REPO_NAME}/backend:latest"
FRONTEND_IMAGE="${REGION}-docker.pkg.dev/${PROJECT_ID}/${REPO_NAME}/frontend:latest"

# Deploy backend
echo "🚀 Deploying backend service..."
gcloud run deploy $BACKEND_SERVICE \
    --image $BACKEND_IMAGE \
    --region $REGION \
    --platform managed \
    --allow-unauthenticated \
    --memory 2Gi \
    --cpu 2 \
    --timeout 300 \
    --min-instances 1 \
    --max-instances 10 \
    --set-env-vars="CLANG_PATH=clang,OBFUSCATOR_PATH=/app/llvm-obfuscator,NODE_ENV=production" \
    --quiet

# Get backend URL
BACKEND_URL=$(gcloud run services describe $BACKEND_SERVICE \
    --region $REGION \
    --format 'value(status.url)')

echo "✅ Backend deployed at: $BACKEND_URL"

# Deploy frontend
echo "🚀 Deploying frontend service..."
gcloud run deploy $FRONTEND_SERVICE \
    --image $FRONTEND_IMAGE \
    --region $REGION \
    --platform managed \
    --allow-unauthenticated \
    --memory 512Mi \
    --cpu 1 \
    --timeout 60 \
    --min-instances 1 \
    --max-instances 5 \
    --set-env-vars="NEXT_PUBLIC_API_URL=${BACKEND_URL},NODE_ENV=production" \
    --quiet

# Get frontend URL
FRONTEND_URL=$(gcloud run services describe $FRONTEND_SERVICE \
    --region $REGION \
    --format 'value(status.url)')

echo "✅ Frontend deployed at: $FRONTEND_URL"

# Set up IAM permissions
echo "🔐 Setting up IAM permissions..."
gcloud run services add-iam-policy-binding $BACKEND_SERVICE \
    --region $REGION \
    --member="allUsers" \
    --role="roles/run.invoker" \
    --quiet

gcloud run services add-iam-policy-binding $FRONTEND_SERVICE \
    --region $REGION \
    --member="allUsers" \
    --role="roles/run.invoker" \
    --quiet

echo ""
echo "✅ Deployment completed successfully!"
echo ""
echo "📊 Service URLs:"
echo "   Backend:  $BACKEND_URL"
echo "   Frontend: $FRONTEND_URL"
echo ""
echo "🌐 Next steps:"
echo "   1. Map your domain ($DOMAIN) to the frontend service:"
echo "      gcloud run domain-mappings create \\"
echo "        --service $FRONTEND_SERVICE \\"
echo "        --domain $DOMAIN \\"
echo "        --region $REGION"
echo ""
echo "   2. Update DNS records in GoDaddy:"
echo "      Add a CNAME record pointing $DOMAIN to: $FRONTEND_URL"
echo ""
echo "   3. View logs:"
echo "      gcloud run services logs read $FRONTEND_SERVICE --region $REGION"
echo "      gcloud run services logs read $BACKEND_SERVICE --region $REGION"

