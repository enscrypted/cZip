# Docker Build Instructions

This project uses Docker to build and package the cZip project, including optional AppImage generation.

## Prerequisites

- Docker must be installed and running.
- Your project folder should be structured like this:

```
czip/
├── src/
│   ├── czip.pro
│   └── ... (source files)
├── assets/
│   ├── czip.desktop
│   └── czip.png
└── docker/
	└── linux
		├── Dockerfile
		├── install.sh
	...	└── INSTALL.md
```

## Usage

From the `czip/docker/linux` directory, run:

```
./install.sh
```

### Options

--skip-appimage  
  Skip AppImage packaging step.

--czip-directory <path>  
  Specify the root directory of your `czip` project (default: `../../cZip`).

## Examples

Run with defaults:

```
./install.sh
```

Skip AppImage creation:

```
./install.sh --skip-appimage
```

Use a custom czip directory:

```
./install.sh --czip-directory /path/to/my/czip
```

Combine both options:

```
./install.sh --czip-directory /path/to/my/czip --skip-appimage
```

## What It Does

1.  **Builds the Docker Image:** Creates a self-contained build environment using the `Dockerfile`, which includes all necessary build tools, Qt 5.15, and the required X11 libraries for AppImage packaging.
2.  **Initializes Dependencies:** The `install.sh` script ensures the `AURA` and `Botan` git submodules are checked out.
3.  **Runs the Build Script:** A container is started, which executes the `build.sh` script to perform a controlled, multi-stage compilation:
    1.  **Builds Botan:** First, it compiles the `Botan` cryptography library from source.
    2.  **Builds AURA:** Next, it compiles the `AURA` steganography library, linking it against the newly built Botan library.
    3.  **Builds cZip:** Finally, it compiles the main `cZip` application, linking it against the pre-built `AURA` and `Botan` libraries.
4.  **Packages the AppImage:** If not skipped, it uses `linuxdeployqt` to package the final `czip` executable and all its dependencies into a single, portable AppImage file.

## Output

- The compiled binary and files will be in:

```
czip/src/build/
```

- If AppImage is built, it will be saved as:

```
czip/src/build/cZip.AppImage
```

## Notes

- Depending on Docker setup, it might be necessary to run `sudo ./install.sh`
