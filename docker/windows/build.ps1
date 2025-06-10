# This script will be copied into the container and executed.
# It ensures all commands run in the same session, solving PATH issues.

# Exit immediately if any command fails.
$ErrorActionPreference = 'Stop'

Write-Output "--- Refreshing environment variables..."
$env:Path = [System.Environment]::GetEnvironmentVariable('Path', 'Machine') + ';' + [System.Environment]::GetEnvironmentVariable('Path', 'User')

Write-Output "--- Installing dependencies with Chocolatey..."
choco install -y python --version 3.9.13 --params '/InstallDir:C:\Python39' --allow-empty-checksums
choco install -y git cmake --no-progress
choco install -y openssl --params "'/AddBinToPath'"
choco install -y pkgconfiglite --no-progress

Write-Output "--- Installing Visual Studio C++ Build Tools..."
choco install -y visualstudio2019buildtools --package-parameters '--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --includeRecommended --quiet'

Write-Output "--- Installing Windows 10 SDK..."
choco install -y windows-sdk-10.1

Write-Output "--- Refreshing PATH to find pip.exe and other tools..."
$env:Path = [System.Environment]::GetEnvironmentVariable('Path', 'Machine') + ';' + [System.Environment]::GetEnvironmentVariable('Path', 'User')

Write-Output "--- Installing Python packages..."
python -m pip install --upgrade pip
pip install --upgrade aqtinstall

Write-Output "--- Installing Qt..."
aqt install-qt windows desktop $env:QT_VERSION $env:QT_ARCH -O C:/Qt

$env:QT_DIR = "C:\Qt\$($env:QT_VERSION)\msvc2019_64"
Write-Output "--- Adjusted QT_DIR to: $($env:QT_DIR)"

Write-Output "--- Cloning QCA..."
git clone https://invent.kde.org/kde/qca.git C:/qca

# The QCA CMakeLists.txt file asks specifically for Qt 5.14.
# Patch the file to ask for 5.15 instead, to match the version installed.
$CmakeFile = 'C:\qca\CMakeLists.txt'
Write-Output "--- Patching CMakeLists.txt to use Qt 5.15..."
(Get-Content $CmakeFile) | ForEach-Object {
    $_ -replace 'find_package\(Qt5 5.14', 'find_package(Qt5 5.15'
} | Set-Content $CmakeFile
Write-Output "--- Patch successful."

Write-Output "--- Building QCA..."
New-Item -ItemType Directory -Force -Path C:\qca\build
Set-Location C:/qca/build

$vcvarsPath = "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

# Construct the build command.
$BuildCommand = "call `"$vcvarsPath`" && cmake .. -G `"NMake Makefiles`" -DCMAKE_PREFIX_PATH=`"$($env:QT_DIR)`" -DCMAKE_INSTALL_PREFIX=`"$($env:QT_DIR)`" -DCMAKE_BUILD_TYPE=Release -DQCA_SUFFIX=qt5 -DBUILD_TESTS=OFF -DBUILD_TOOLS=OFF -DQT_QMAKE_EXECUTABLE=`"$($env:QT_DIR)\bin\qmake.exe`" -DOPENSSL_ROOT_DIR=`"C:\Program Files\OpenSSL-Win64`" -DQCA_PLUGINS_INSTALL_DIR=`"$($env:QT_DIR)\plugins`" && nmake && nmake install"

# Execute the command
cmd.exe /c $BuildCommand

# Add error checking to ensure the script fails if the build fails.
if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE"
}

Write-Output "--- Build script finished successfully."

exit 0