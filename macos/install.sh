#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# --- Prerequisite Check for Intel Homebrew ---
X86_BREW_PATH="/usr/local/bin/brew"
if [ ! -f "$X86_BREW_PATH" ]; then
    echo "Error: Intel (x86_64) Homebrew installation not found at $X86_BREW_PATH."
    echo ""
    echo "To build Intel software on an Apple Silicon Mac, you need a separate Homebrew installation for Rosetta 2."
    echo "Please run this single command in your terminal to install it:"
    echo ""
    echo '   arch -x86_64 /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"'
    echo ""
    echo "After the installation is complete, you can run this script again."
    exit 1
fi

# --- Cleanup Function and Trap ---
function cleanup() {
    echo ""
    echo "🧹 Performing cleanup..."
    
    if [ ${#PACKAGES_TO_UNINSTALL[@]} -gt 0 ]; then
        echo "   -> Uninstalling temporary x86_64 Homebrew packages: ${PACKAGES_TO_UNINSTALL[*]}"
        arch -x86_64 "$X86_BREW_PATH" uninstall "${PACKAGES_TO_UNINSTALL[@]}"
    else
        echo "   -> No temporary Homebrew packages to uninstall."
    fi
    
    echo "Cleanup complete. Your system is back to its original state."
}
trap cleanup EXIT

# --- Dependency Management ---
echo "Checking for and installing x86_64 dependencies via Rosetta 2..."
REQUIRED_BREW_PACKAGES=("cmake" "ninja" "pkg-config" "openssl@3" "python")
PACKAGES_TO_UNINSTALL=()

for pkg in "${REQUIRED_BREW_PACKAGES[@]}"; do
    if ! arch -x86_64 "$X86_BREW_PATH" list --formula "$pkg" &>/dev/null; then
        echo "   -> Dependency '$pkg' (x86_64) not found. Temporarily installing..."
        arch -x86_64 "$X86_BREW_PATH" install --formula "$pkg"
        PACKAGES_TO_UNINSTALL+=("$pkg")
    else
        echo "   -> Dependency '$pkg' (x86_64) is already installed."
    fi
done
echo "x86_64 Homebrew dependencies are ready."

# --- Configuration ---
QT_VERSION="5.15.2"
APP_NAME="cZip"
TARGET_ARCH="x86_64"
CZIP_DIR=$(realpath "$(dirname "$0")/..")
PROJECT_PRO_FILE="$CZIP_DIR/src/czip.pro"

X86_PYTHON_PATH="/usr/local/bin/python3"
if [ ! -f "$X86_PYTHON_PATH" ]; then
    echo "Error: Could not find x86_64 python at $X86_PYTHON_PATH after installation."
    exit 1
fi

# --- Initialize Submodules (AURA) ---
echo "⚙️ Initializing and updating Git submodules (AURA and Botan)..."
cd "$CZIP_DIR"
git submodule update --init --recursive
cd "$(dirname "$0")" # Return to script directory

echo "=================================================================="
echo "Starting ${TARGET_ARCH} build..."
echo "   This will create an Intel binary that runs on all Macs via Rosetta 2."
echo "=================================================================="

# --- Directory Setup ---
BUILD_ENV_DIR="$CZIP_DIR/build_env_macos/$TARGET_ARCH"
SRC_BUILD_DIR="$CZIP_DIR/src/build_macos_$TARGET_ARCH"
QCA_INSTALL_DIR="$BUILD_ENV_DIR/qca_install"
VENV_DIR="$BUILD_ENV_DIR/python_venv" 

echo "  -> Wiping previous build environment to ensure a clean slate..."
rm -rf "$BUILD_ENV_DIR"

# --- Environment Setup Phase ---
echo "Setting up isolated build environment for ${TARGET_ARCH}..."
mkdir -p "$BUILD_ENV_DIR" "$QCA_INSTALL_DIR"

echo "   -> Setting up local x86_64 Python virtual environment..."
"$X86_PYTHON_PATH" -m venv "$VENV_DIR"
"$VENV_DIR/bin/pip" install -q aqtinstall==3.1.1

QT_HOST="mac"
QT_ARCH_IDENTIFIER="clang_64"
QT_BASE_DIR="$BUILD_ENV_DIR/Qt/$QT_VERSION"

echo "Downloading Qt $QT_VERSION for $TARGET_ARCH..."
"$VENV_DIR/bin/python3" -m aqt install-qt "$QT_HOST" desktop "$QT_VERSION" "$QT_ARCH_IDENTIFIER" -O "$BUILD_ENV_DIR/Qt"

QT_INSTALL_DIR=$(find "$QT_BASE_DIR" -maxdepth 1 -mindepth 1 -type d | head -n 1)
if [ -z "$QT_INSTALL_DIR" ] || [ ! -d "$QT_INSTALL_DIR/lib/cmake/Qt5" ]; then
    echo "Error: Could not find a valid Qt installation in '$QT_BASE_DIR'"
    exit 1
fi
echo "  -> Found Qt installation at: $QT_INSTALL_DIR"

QCA_SRC_DIR="$BUILD_ENV_DIR/qca"
git clone https://github.com/KDE/qca.git "$QCA_SRC_DIR"

QCA_CMAKE_FILE="$QCA_SRC_DIR/CMakeLists.txt"
if grep -q "find_package(Qt5 5.14" "$QCA_CMAKE_FILE"; then
    echo "  -> Patching QCA's CMakeLists.txt to require Qt 5.15..."
    sed -i '' 's/find_package(Qt5 5.14/find_package(Qt5 5.15/' "$QCA_CMAKE_FILE"
fi

echo "Building QCA for $TARGET_ARCH..."
mkdir -p "$QCA_SRC_DIR/build"
cd "$QCA_SRC_DIR/build"

SDK_PATH=$(xcrun --sdk macosx --show-sdk-path)

arch -${TARGET_ARCH} cmake .. -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$QT_INSTALL_DIR" \
    -DQt5_DIR="$QT_INSTALL_DIR/lib/cmake/Qt5" \
    -DCMAKE_OSX_SYSROOT="$SDK_PATH" \
    -DCMAKE_INSTALL_PREFIX="$QCA_INSTALL_DIR" \
    -DBUILD_PLUGINS="ossl" \
    -DQCA_SUFFIX=qt5

arch -${TARGET_ARCH} ninja
arch -${TARGET_ARCH} ninja install
cd "$CZIP_DIR"

echo "QCA build complete."

# --- Build Project Phase ---
echo "⚙️ Compiling $APP_NAME for ${TARGET_ARCH}..."
rm -rf "$SRC_BUILD_DIR"
mkdir -p "$SRC_BUILD_DIR"
cd "$SRC_BUILD_DIR"

OPENSSL_PREFIX=$(arch -x86_64 "$X86_BREW_PATH" --prefix openssl@3)

echo "   -> Running qmake with direct paths..."
arch -${TARGET_ARCH} "$QT_INSTALL_DIR/bin/qmake" \
    "$PROJECT_PRO_FILE" \
    "CONFIG+=sdk_no_version_check" \
    "QCA_PREFIX=$QCA_INSTALL_DIR" \
    "OPENSSL_PREFIX=$OPENSSL_PREFIX"

arch -${TARGET_ARCH} make -j$(sysctl -n hw.ncpu)

echo "Compilation complete."

# --- Packaging Phase ---
echo "Packaging application..."
APP_BUNDLE="$SRC_BUILD_DIR/$APP_NAME.app"

# Use macdeployqt with -libpath to automatically find and deploy the QCA framework
echo "   -> Running macdeployqt to deploy Qt and QCA frameworks..."
arch -${TARGET_ARCH} "$QT_INSTALL_DIR/bin/macdeployqt" "$APP_BUNDLE" -always-overwrite -libpath="$QCA_INSTALL_DIR/lib"

echo "  -> Deploying QCA OpenSSL plugin..."
QCA_PLUGIN_DIR="$APP_BUNDLE/Contents/PlugIns/crypto"
mkdir -p "$QCA_PLUGIN_DIR"
QCA_PLUGIN_SOURCE=$(find "$QCA_INSTALL_DIR/lib" -name "libqca-ossl.dylib")
if [ -z "$QCA_PLUGIN_SOURCE" ]; then
    echo "Warning: Could not find the QCA OpenSSL plugin to deploy."
else
    cp "$QCA_PLUGIN_SOURCE" "$QCA_PLUGIN_DIR/"
fi
# ---------------------------------------------------------

echo "  -> Creating DMG..."
DMG_NAME="$APP_NAME.dmg"
hdiutil create "$DMG_NAME" -volname "$APP_NAME" -fs HFS+ -srcfolder "$APP_NAME.app"
cd "$CZIP_DIR"

echo ""
echo "Success! Find your application for all Macs here:"
echo "  -> App Bundle: $APP_BUNDLE"
echo "  -> Disk Image: $SRC_BUILD_DIR/$DMG_NAME"
