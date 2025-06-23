#!/bin/bash

set -e

echo "================================================="
echo "Running cZip Build inside Docker Container"
echo "================================================="

export QT_DIR=/opt/Qt/5.15.2/gcc_64
export PATH=$QT_DIR/bin:$PATH
export LD_LIBRARY_PATH=$QT_DIR/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=$QT_DIR/lib/pkgconfig:$PKG_CONFIG_PATH

DEPS_INSTALL_DIR="/project/deps_install"
mkdir -p "$DEPS_INSTALL_DIR"

echo "⚙️ Building Botan dependency independently..."
BOTAN_DIR="/project/external/AURA/external/botan"
cd "$BOTAN_DIR"
python3 configure.py --prefix="$DEPS_INSTALL_DIR"
make -j$(nproc)
make install
echo "✅ Botan build complete. Libraries installed to $DEPS_INSTALL_DIR"

echo "⚙️ Building AURA, linking against pre-built Botan..."
export CMAKE_GENERATOR="Ninja"
AURA_DIR="/project/external/AURA"
AURA_BUILD_DIR="$AURA_DIR/build"
mkdir -p "$AURA_BUILD_DIR"
cd "$AURA_BUILD_DIR"

BOTAN_INCLUDE_PATH=$(find "$DEPS_INSTALL_DIR/include" -name "botan-2" -type d)
echo "Found Botan include path at: $BOTAN_INCLUDE_PATH"

cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$DEPS_INSTALL_DIR" \
    -DAURA_USE_SYSTEM_BOTAN=ON \
    -DBOTAN_INCLUDE_DIR="$BOTAN_INCLUDE_PATH" \
    -DBOTAN_LIBRARY_DIR="$DEPS_INSTALL_DIR/lib"

ninja -j$(nproc)
echo "✅ AURA build complete. Library installed to $DEPS_INSTALL_DIR"

echo "⚙️ Building cZip application..."
CZIP_BUILD_DIR="/project/src/build"
mkdir -p "$CZIP_BUILD_DIR"
cd "$CZIP_BUILD_DIR"

qmake ../../src/czip.pro "CONFIG+=use_prebuilt_deps" "DEPS_PREFIX=$DEPS_INSTALL_DIR"
make -j$(nproc)
echo "✅ cZip compilation complete."

if [ "$SKIP_APPIMAGE" = "false" ]; then
    echo "📦 Packaging application into an AppImage..."
    BOTAN_LIB_PATH=$(find "$DEPS_INSTALL_DIR/lib" -name "libbotan-2.so.*" | head -n 1)
    cp "$BOTAN_LIB_PATH" .
    
    cp /project/assets/czip.desktop .
    cp /project/assets/czip.png .
    export LD_LIBRARY_PATH="$CZIP_BUILD_DIR:$DEPS_INSTALL_DIR/lib:$LD_LIBRARY_PATH"
    linuxdeployqt ./czip -appimage
    mv cZip-*.AppImage cZip.AppImage
    echo "✅ AppImage created successfully."
else
    echo "📦 AppImage creation skipped."
fi