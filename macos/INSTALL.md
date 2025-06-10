# macOS Build Instructions

This document provides instructions for building the cZip project on a macOS computer. The script creates a self-contained, temporary environment to compile and package an `x86_64` (Intel) version of the application. This single binary will run natively on Intel-based Macs and seamlessly on Apple Silicon (M-series) Macs via Rosetta 2.

## Prerequisites

Before running the build script for the first time, your Mac may require a one-time setup to install the necessary development tools.

### Step 1: Install Xcode Command Line Tools

```
xcode-select --install
```

Follow the on-screen prompts to complete the installation.

### Step 2: Install Homebrew for Intel (Apple Silicon Macs Only)

**This step is only required for Macs with Apple Silicon (M1, M2, M3, etc.).** If you are on an Intel-based Mac, you can skip this step.

To build an Intel application, we need an Intel-based toolchain. The script requires a separate installation of Homebrew that runs under the Rosetta 2 translation layer.

Run this single command in your terminal to install it. This will not affect your existing native Homebrew installation.

```
arch -x86_64 /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

## Project Structure

Your project folder should be structured like this for the script to work correctly:

```
czip/
├── src/
│   ├── czip.pro
│   └── ... (source files)
├── assets/
│   ├── ... (icon files)
└── macos/
    └── install.sh  <-- The build script
```

## Usage

1.  **Navigate to the script's directory.** Open a terminal and change to the `macos` directory within your project folder.
    ```
    cd /path/to/your/project/macos
    ```

2.  **Make the script executable.** You only need to do this once.
    ```
    chmod +x install.sh
    ```

3.  **Run the script.**
    ```
    ./install.sh
    ```
    The script takes no arguments. It will automatically begin the build process.

## What It Does

The `install.sh` script automates the entire build process:

1.  **Checks for Prerequisites:** Verifies that the Intel version of Homebrew is installed (on Apple Silicon Macs).
2.  **Installs Temporary Tools:** Uses the Intel Homebrew to temporarily install `x86_64` versions of CMake, Ninja, Python, and OpenSSL.
3.  **Creates Isolated Environment:** Wipes any previous build and creates a clean build environment inside `build_env_macos/`.
4.  **Downloads Qt:** Downloads an `x86_64` version of Qt 5.15.2 into the isolated environment.
5.  **Builds Dependencies:** Clones and builds the QCA (Qt Crypto Abstraction) library from source against the downloaded Qt.
6.  **Compiles Project:** Compiles cZip using `qmake` and `make`.
7.  **Packages Application:** Uses `macdeployqt` to create a distributable `.app` bundle and then packages it into a `.dmg` disk image.
8.  **Cleans Up:** Automatically uninstalls the temporary tools it installed via Homebrew, leaving your system state unchanged.

## Output

- The compiled application bundle and related files will be in:
  `czip/src/build_macos_x86_64/`

- The final, distributable disk image will be saved as:
  `czip/src/build_macos_x86_64/cZip.dmg`

## Cleanup & Uninstallation

The build script cleans up its own temporary tools after each run. However, if you wish to reverse the one-time setup steps, you can do so as follows.

### Uninstalling Homebrew for Intel (Apple Silicon Macs Only)

If you no longer need the Intel Homebrew installation for any other development, you can remove it by running the official uninstaller under the `x86_64` architecture:

```
arch -x86_64 /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/uninstall.sh)"
```

### Removing Build Artifacts

The script creates local folders for the build environment and output. You can remove these at any time to clean up the project directory:

```
# Remove the isolated build environment
rm -rf ../build_env_macos

# Remove the build output directory
rm -rf ../src/build_macos_x86_64
```
