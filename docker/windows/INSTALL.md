# Windows Docker Build Instructions

This project uses Docker to create a self-contained environment to build and package the cZip project for Windows.

## Prerequisites

-   **Docker Desktop** must be installed and running on your Windows machine.
    * It is recommended to run Docker Desktop in "Windows containers" mode for the best compatibility with this setup.

-   Your project folder should be structured like this:

```
czip/
├── src/
│   ├── czip.pro
│   └── ... (source files)
├── external/
│   └── AURA/
└── docker/
    └── windows/
        ├── Dockerfile
        ├── install.bat
        ├── build_and_deploy.bat
        └── INSTALL.md
```

## Usage

From the `czip/docker/windows/` directory, simply execute the `install.bat` script:

```bash
install.bat
```

The script handles the entire process automatically and requires no arguments.

## What It Does

This streamlined process builds the entire application and its dependencies inside a controlled Docker environment.

1.  **Builds the Docker image** (`czip-build-win`).
    * The `Dockerfile` defines an environment with all necessary build tools, including Visual Studio 2019 Build Tools, Python, CMake, Ninja, and Qt 5.15.2.
    * All dependencies are installed in a single, atomic step to ensure the container's `PATH` is consistent and reliable.

2.  **Runs the Build & Deploy Container**.
    * The `install.bat` script starts a container from the newly built image.
    * It mounts your local `cZip` project directory into the container at `C:\project`.
    * Inside the container, a single `build_and_deploy.bat` script now orchestrates the entire build in the correct sequence:
        1.  **Sets up the Environment:** Initializes the MSVC compiler and explicitly adds Windows SDK paths to the environment.
        2.  **Builds Dependencies First:** Compiles the AURA/Botan submodule from source.
        3.  **Builds the Application:** Runs `qmake` then `nmake` on the main `cZip` project. This now works because the AURA/Botan libraries and headers have already been built.
        4.  **Deploys Binaries:** Creates a `deployed_app` folder and uses `windeployqt` to gather all necessary Qt DLLs. It also manually copies the required `botan.dll`.

3.  **Zips the Final Artifact**.
    * Back on your host machine, the `install.bat` script takes the contents of the `deployed_app` folder and compresses it into a `cZip.zip` archive.

## Output

-   The final, distributable archive will be located at:
    `czip\src\build\cZip.zip`

-   The un-zipped portable application contents can be found in:
    `czip\src\build\deployed_app\`