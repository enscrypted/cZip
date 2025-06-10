# Final Dockerfile
FROM mcr.microsoft.com/dotnet/framework/runtime:4.8-windowsservercore-ltsc2019

ENV QT_VERSION=5.15.2
ENV QT_ARCH=win64_msvc2019_64

# Install deps
ENV PATH="C:\\ProgramData\\chocolatey\\bin;C:\\Python39\\Scripts;C:\\Python39;C:\\Program Files\\Git\\cmd;C:\\Program Files\\CMake\\bin;${QT_DIR}\\bin;%PATH%"

# Create the main project directory in the container
WORKDIR C:/project

RUN C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command "[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))"

# ADD/COPY files relative to the project root build context
ADD docker/windows/build.ps1 /project/build.ps1
ADD docker/windows/build_app.bat /project/build_app.bat
ADD docker/windows/deploy_app.bat /project/deploy_app.bat
COPY assets /project/assets/

# Run the initial setup script from the project root
RUN C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe -ExecutionPolicy Bypass -File C:\project\build.ps1

# Clean up build artifacts from the temporary QCA build
WORKDIR C:/
RUN C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe -Command "rm -Force /qca -Recurse"

# Set the final default working directory for subsequent 'docker run' commands
WORKDIR C:/project/src