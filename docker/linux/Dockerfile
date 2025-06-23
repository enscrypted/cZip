# Use a stable base image like Ubuntu 20.04 LTS
FROM ubuntu:20.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install essential build tools, git, wget, python, cmake, ninja, and Qt dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    wget \
    cmake \
    ninja-build \
    python3 \
    python3-pip \
    python3-dev \
    fuse \
    patchelf \
    libgl1-mesa-dev \
    libxcb-xinerama0 \
    libglib2.0-0 \
    libfontconfig1 \
    libxcb1 \
    libxcb-render0 \
    libxcb-shm0 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-randr0 \
    libxcb-render-util0 \
    libxcb-xinerama0 \
    libxcb-xfixes0 \
    libxcb-shape0 \
    libxcb-sync1 \
    libxkbcommon-x11-0 \
    libdbus-1-3 \
    file \
    --no-install-recommends && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Install aqtinstall to fetch Qt
RUN python3 -m pip install aqtinstall==3.1.1

# Download and install Qt 5.15.2 into the /opt/ directory
RUN aqt install-qt linux desktop 5.15.2 gcc_64 -O /opt/Qt

# Download and install the linuxdeployqt AppImage utility
RUN wget -O /usr/local/bin/linuxdeployqt https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage && \
    chmod a+x /usr/local/bin/linuxdeployqt

# Set a default working directory for the container
WORKDIR /project