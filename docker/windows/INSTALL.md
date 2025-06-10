# Windows Docker Build Instructions

This project uses Docker to build and package the cZip project for Windows, including optional portable application generation using `windeployqt`.

## Prerequisites

-   **Docker Desktop** must be installed and running on your Windows machine.
    * It's recommended to configure Docker Desktop to use the **Hyper-V backend** for Windows containers for optimal compatibility with this setup. You can switch this in Docker Desktop settings (General tab, uncheck "Use the WSL 2 based engine").
-   Your project folder should be structured like this:

    ```
    czip/
    ├── src/
    │   ├── czip.pro
    │   └── ... (source files)
    ├── assets/
    │   ├── czip.desktop
    │   └── czip.png
    └── docker/
        ├── linux/
        │   ├── Dockerfile
        │   ├── install.sh
        │   └── INSTALL.md
        └── windows/
            ├── Dockerfile
            ├── build.ps1
            ├── install.bat
            └── INSTALL.md
    ```

**Important Note for your `.pro` file:**

Your `czip.pro` file uses a variable `QT_PATH_WIN` to locate your Qt and QCA installations. This variable is automatically set to `C:/Qt/5.15.2/msvc2019_64` (the default path inside the Docker container where Qt is installed). You can override this default path by passing `QT_INSTALL_PATH` to `qmake` (e.g., `qmake czip.pro QT_INSTALL_PATH="C:/Some/Other/Path"`). The `install.bat` script automatically passes the correct container path.

## Usage

From the `czip/docker/windows/` directory, execute the `install.bat` script:

```bash
install.bat
```

### Options

`--skip-deployment`
:   Skips the `windeployqt` packaging and zipping step, leaving only the compiled executable.

`--czip-directory <path>`
:   Specifies the root directory of your `czip` project. By default, it assumes `czip` is the parent directory of `docker/windows/`.

## Examples

Run with default settings (builds image, compiles, deploys, and zips):

```bash
install.bat
```

Skip portable application creation:

```bash
install.bat --skip-deployment
```

Use a custom `czip` directory:

```bash
install.bat --czip-directory C:\path\to\my\czip
```

Combine both options:

```bash
install.bat --czip-directory C:\path\to\my\czip --skip-deployment
```

## What It Does

1.  **Builds the Docker image** (`qt5.15-qca-win`) using the `Dockerfile`.
    * This `Dockerfile` is based on a stable `.NET Framework 4.8` image.
    * It copies and executes the `build.ps1` script *inside the container*. This script handles all essential environment setup, including:
        * Installing Chocolatey, Python, Git, CMake, OpenSSL, pkg-config, and Visual Studio 2019 Build Tools.
        * Installing `aqtinstall` via `pip`.
        * Downloading and installing **Qt 5.15.2** (MSVC 2019 64-bit).
        * Cloning, patching, building, and installing **QCA** (Qt Cryptographic Architecture).
    * The use of `build.ps1` ensures all environment variable changes and tool installations occur within a consistent PowerShell session, overcoming common Windows Docker build challenges.
2.  **Cleans up any previous build artifacts** on your host machine (`czip/src/build/`).
3.  **Compiles the cZip project** (`czip.pro`) using `qmake` and `nmake` inside a **new Docker container**. This step leverages the environment set up in the image. `qmake` receives the Qt installation path dynamically via the `QT_INSTALL_PATH` variable.
4.  If not skipped (`--skip-deployment` is absent):
    * **Packages the binary into a portable application** using `windeployqt.exe` (which is part of the Qt installation). This gathers all necessary Qt DLLs and other dependencies into a `czip/src/build/deployed_app/` directory on your host.
    * **Manually copies QCA plugins** into the deployed application directory, as `windeployqt` may not automatically pick them up.
    * **Compresses the deployed application** into a `cZip_Portable.zip` archive.

## Output

-   The compiled `czip.exe` binary will be found in:
    ```
    czip/src/build/
    ```
-   If the portable application is built, it will be saved as:
    ```
    czip/src/build/cZip_Portable.zip
    ```
    The unzipped contents will be in:
    ```
    czip/src/build/deployed_app/
    ```