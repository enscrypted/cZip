#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# --- Configuration ---
SKIP_APPIMAGE=false
DOCKER_IMAGE_NAME="czip-build-linux"

# Dynamically determine the cZip project directory based on this script's location
SCRIPT_DIR=$(dirname "$(realpath "$0")")
DEFAULT_CZIP_DIR=$(realpath "$SCRIPT_DIR/../..")
CZIP_DIR="$DEFAULT_CZIP_DIR"

# --- Argument Parsing ---
while [[ "$#" -gt 0 ]]; do
  case $1 in
    --skip-appimage) SKIP_APPIMAGE=true ;;
    --czip-directory) CZIP_DIR="$2"; shift ;;
    *) echo "Unknown parameter: $1"; exit 1 ;;
  esac
  shift
done

CZIP_DIR=$(realpath "$CZIP_DIR")

echo "================================================="
echo "cZip Linux Build"
echo "================================================="
echo "-> cZip Project Directory: $CZIP_DIR"
echo "-> Skip AppImage Creation: $SKIP_APPIMAGE"
echo "-> Docker Image: $DOCKER_IMAGE_NAME"
echo "-------------------------------------------------"

# --- Submodule Initialization ---
echo "⚙️ Initializing and updating Git submodules (AURA and Botan)..."
cd "$CZIP_DIR"
git submodule update --init --recursive
cd "$SCRIPT_DIR"

# --- Docker Build ---
echo "🏗️ Building Docker image '$DOCKER_IMAGE_NAME'..."
docker build -t "$DOCKER_IMAGE_NAME" "$SCRIPT_DIR"

# --- Cleanup ---
echo "🧹 Cleaning previous build directory..."
rm -rf "$CZIP_DIR/src/build"

# --- Docker Run ---
echo "🚀 Running build script inside the container..."
docker run --rm -it \
  --cap-add SYS_ADMIN \
  --device /dev/fuse \
  --security-opt apparmor:unconfined \
  -v "$CZIP_DIR":/project \
  -v "$SCRIPT_DIR/build.sh":/project/build.sh \
  -e "SKIP_APPIMAGE=${SKIP_APPIMAGE}" \
  "$DOCKER_IMAGE_NAME" \
  bash /project/build.sh

echo "✅ Done!"
echo ""
echo "Find your build artifacts in: $CZIP_DIR/src/build/"