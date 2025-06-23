# Use a stable Windows base image with the .NET Framework for compatibility.
FROM mcr.microsoft.com/dotnet/framework/runtime:4.8-windowsservercore-ltsc2019

# Set environment variables for the Qt installation.
ENV QT_VERSION=5.15.2
ENV QT_ARCH=win64_msvc2019_64
ENV QT_DIR="C:\\Qt\\${QT_VERSION}\\msvc2019_64"

# Set the working directory for the setup process.
WORKDIR C:/build

# Execute all environment setup in a single, atomic RUN command.
# This ensures that PATH changes made by one command (like choco install)
# are available to the subsequent commands in the same session.
RUN powershell.exe -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command " \
    # 1. Install Chocolatey
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1')); \
    \
    # 2. Add choco to the current session's PATH manually
    $env:Path = [System.Environment]::GetEnvironmentVariable('Path', 'Machine') + ';' + [System.Environment]::GetEnvironmentVariable('Path', 'User'); \
    \
    # 3. Install all choco packages
    Write-Host '--- Installing core dependencies with Chocolatey...'; \
    choco install -y python --version 3.9.13 --params '/InstallDir:C:\Python39' --allow-empty-checksums; \
    choco install -y git cmake --no-progress; \
    choco install -y visualstudio2019buildtools --package-parameters '--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --includeRecommended --quiet'; \
    choco install -y windows-sdk-10.1; \
    \
    # 4. Refresh PATH again to find newly installed tools like python.exe
    $env:Path = [System.Environment]::GetEnvironmentVariable('Path', 'Machine') + ';' + [System.Environment]::GetEnvironmentVariable('Path', 'User'); \
    \
    # 5. Install Python packages
    Write-Host '--- Installing Python packages for Qt...'; \
    python -m pip install --upgrade pip; \
    pip install --upgrade aqtinstall; \
    \
    # 6. Install Qt
    Write-Host '--- Installing Qt 5.15.2...'; \
    aqt install-qt windows desktop $env:QT_VERSION $env:QT_ARCH -O C:/Qt; \
    \
    Write-Host '--- Build environment setup complete.'"


COPY docker/windows/build_and_deploy.bat /

ENV PATH="C:\\ProgramData\\chocolatey\\bin;C:\\Python39\\Scripts;C:\\Python39;C:\\Program Files\\Git\\cmd;C:\\Program Files\\CMake\\bin;${QT_DIR}\\bin;%PATH%"