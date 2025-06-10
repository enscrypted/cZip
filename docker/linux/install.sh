#!/bin/bash

set -e

# Default values
SKIP_APPIMAGE=false
# CZIP_DIR=~/repos/cZip # Original hardcoded path

# Dynamically determine CZIP_DIR based on script location.
# This assumes install.sh is located in czip/docker/linux.
# realpath resolves symlinks and gives the absolute path.
SCRIPT_DIR=$(dirname "$(realpath "$0")")
DEFAULT_CZIP_DIR=$(realpath "$SCRIPT_DIR/../..")
CZIP_DIR="$DEFAULT_CZIP_DIR"

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
  case $1 in
    --skip-appimage) SKIP_APPIMAGE=true ;;
    --czip-directory) CZIP_DIR="$2"; shift ;;
    *) echo "Unknown parameter: $1"; exit 1 ;;
  esac
  shift
done

# Resolve full path
CZIP_DIR=$(realpath "$CZIP_DIR")

echo "Using cZip directory: $CZIP_DIR"
echo "Skip AppImage: $SKIP_APPIMAGE"

# Build Docker image
echo "Building Docker image 'qt5.14-qca'..."
docker build -t qt5.14-qca "$(dirname "$0")"

# Clean previous build
echo "Cleaning previous build directory..."
rm -rf "$CZIP_DIR/src/build"

# Run build
echo "Running build..."
docker run --rm -it \
  -v "$CZIP_DIR/src":/src \
  -w /src \
  qt5.14-qca \
  bash -c "\
    export PKG_CONFIG_PATH=/usr/lib/pkgconfig:/opt/Qt/5.15.2/gcc_64/lib/pkgconfig && \
    export LD_LIBRARY_PATH=/usr/lib:/opt/Qt/5.15.2/gcc_64/lib && \
    export QMAKEFEATURES=/usr/mkspecs/features && \
    mkdir -p build && cd build && \
    /opt/Qt/5.15.2/gcc_64/bin/qmake ../czip.pro && \
    make"

# Run AppImage step (if not skipped)
if [ "$SKIP_APPIMAGE" = false ]; then
  echo "Creating AppImage..."
  docker run --rm -it \
    --device /dev/fuse \
    --cap-add SYS_ADMIN \
    --security-opt apparmor:unconfined \
    -v "$CZIP_DIR/src":/src \
    -v "$CZIP_DIR/assets":/assets \
    -w /src/build \
    qt5.14-qca \
    bash -c "\
      cp /assets/czip.desktop . && \
      cp /assets/czip.png . && \
      /usr/local/bin/linuxdeployqt ./czip -appimage && \
      mv cZip-*.AppImage cZip.AppImage"
else
  echo "AppImage creation skipped."
fi

echo "Done!"