FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive
ENV QT_VERSION=5.15.2
ENV QT_DIR=/opt/Qt/${QT_VERSION}/gcc_64

# Install deps
RUN apt-get update && apt-get install -y \
    python3-pip \
    build-essential \
    git \
    ninja-build \
    cmake \
    libssl-dev \
    libgcrypt20-dev \
    zlib1g-dev \
    wget \
    curl \
    ca-certificates \
    pkg-config \
    libgl1-mesa-dev \
    libfuse2 \
    fuse \
    libx11-xcb1 \
    libxrender1 \
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
    libdbus-1-3

# Install aqtinstall to get Qt non-interactively
RUN pip3 install aqtinstall==3.1.1

# Install Qt 5.15.2 (gcc_64 desktop)
RUN aqt install-qt linux desktop ${QT_VERSION} gcc_64 -O /opt/Qt

# Set environment variables for Qt tools
ENV PATH="${QT_DIR}/bin:${PATH}"
ENV CMAKE_PREFIX_PATH="${QT_DIR}"

# Build and install QCA
RUN git clone https://github.com/KDE/qca.git /tmp/qca && \
    mkdir -p /tmp/qca/build && cd /tmp/qca/build && \
    cmake .. -GNinja \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DCMAKE_BUILD_TYPE=Release \
        -DQCA_SUFFIX=qt5 && \
    ninja && ninja install && \
    cd / && rm -rf /tmp/qca

# Install linuxdeployqt
RUN wget -O /usr/local/bin/linuxdeployqt https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage && \
    chmod a+x /usr/local/bin/linuxdeployqt
