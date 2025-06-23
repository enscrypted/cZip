@echo off
setlocal

:: --- Configuration ---
set "CZIP_DIR=%~dp0..\.."
for %%i in ("%CZIP_DIR%") do set "CZIP_DIR=%%~fi"
echo "Using project root: %CZIP_DIR%"

:: --- 1. Build Docker Image ---
echo "Building Docker image 'czip-build-win'..."
docker build -t czip-build-win -f "%CZIP_DIR%\docker\windows\Dockerfile" "%CZIP_DIR%"
if %errorlevel% neq 0 ( echo "Error: Docker image build failed." & exit /b 1 )
echo "Docker image build successful."

:: --- 2. Clean Host & Run Build/Deploy Container ---
echo "Cleaning previous build directory on host..."
if exist "%CZIP_DIR%\src\build" rd /s /q "%CZIP_DIR%\src\build"

echo "Running build and deployment inside container..."
docker run --rm -it -v "%CZIP_DIR%:C:\project" czip-build-win cmd.exe /c "C:\build_and_deploy.bat"
if %errorlevel% neq 0 ( echo "Error: Build and deployment script failed." & exit /b 1 )

:: --- 3. Zip the Final Artifact ---
echo "Zipping the portable application..."
powershell -Command "Add-Type -AssemblyName System.IO.Compression.FileSystem; [System.IO.Compression.ZipFile]::CreateFromDirectory('%CZIP_DIR%\src\build\deployed_app', '%CZIP_DIR%\src\build\cZip.zip')"
if %errorlevel% neq 0 ( echo "Error: Failed to zip the application." & exit /b 1 )

echo "Zipping successful."
echo "---"
echo "Success! Find your portable application at: %CZIP_DIR%\src\build\cZip.zip"