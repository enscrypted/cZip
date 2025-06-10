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

1. Builds the Docker image using `Dockerfile`.
2. Removes any previous `/src/build/` directory.
3. Compiles the project using `qmake` and `make` inside Docker.
4. If not skipped, packages the binary into an AppImage using `linuxdeployqt`.

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
