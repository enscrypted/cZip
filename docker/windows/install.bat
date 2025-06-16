@echo off
setlocal enabledelayedexpansion

:: --- Robust Path Setup ---
:: Get the directory where this script is located (%~dp0) and resolve the project root.
for /f "delims=" %%i in ("%~dp0..\..") do set "PROJECT_ROOT=%%~fi"

:: --- Script Configuration & Argument Parsing ---
:: All major paths are now relative to the determined project root.
set "DOCKERFILE_PATH=%PROJECT_ROOT%\docker\windows\Dockerfile"
set "CZIP_DIR=%PROJECT_ROOT%"
set "SKIP_DEPLOYMENT=false"

:arg_loop
if "%~1"=="" goto end_arg_loop
if "%~1"=="--skip-deployment" (
    set "SKIP_DEPLOYMENT=true"
) else if "%~1"=="--czip-directory" (
    set "CZIP_DIR="%~2""
    shift
) else (
    echo "Unknown parameter: %1"
    exit /b 1
)
shift
goto arg_loop
:end_arg_loop

:: Resolve the full path for the CZIP_DIR just in case a relative one was passed.
for %%i in ("%CZIP_DIR%") do set "CZIP_DIR=%%~fi"
set "CZIP_DIR_DOCKER=%CZIP_DIR:\=/%"

echo "Using project root: %PROJECT_ROOT%"
echo "Using cZip directory: %CZIP_DIR%"

:: --- Initialize Git Submodules (AURA) ---
echo "Initializing and updating Git submodules..."
pushd "%PROJECT_ROOT%"
git submodule update --init --recursive
if %errorlevel% neq 0 (
    echo "ERROR: Git submodule update failed."
    popd
    exit /b 1
)
popd
echo "Submodules updated successfully."

:: --- Docker Image Build ---
:: Temporarily change to the project root to set the Docker build context,
:: then run the build command, then change back.
echo "Building Docker image 'qt5.15-qca-win' for environment setup..."
pushd "%PROJECT_ROOT%"
docker build -t qt5.15-qca-win -f "%DOCKERFILE_PATH%" .
if %errorlevel% neq 0 (
    echo "ERROR: Docker image build failed."
    popd
    exit /b 1
)
popd
echo "Docker image build successful."


:: --- Application Build ---
echo "Cleaning previous application build directory on host..."
if exist "%CZIP_DIR%\src\build" rd /s /q "%CZIP_DIR%\src\build"
mkdir "%CZIP_DIR%\src\build" 2>NUL

echo "Running application build (qmake and nmake) inside Docker container..."
docker run --rm -it --mount type=bind,source="%CZIP_DIR_DOCKER%",target=C:/project -w C:/project/src qt5.15-qca-win cmd /c "C:\project\docker\windows\build_app.bat"
if %errorlevel% neq 0 (
    echo "ERROR: Application compilation failed."
    exit /b 1
)
echo "Application build successful."


:: --- Verification & Deployment ---
echo "Verifying if cZip.exe was built successfully..."
if not exist "%CZIP_DIR%\src\build\release\czip.exe" (
    echo "ERROR: cZip.exe not found in %CZIP_DIR%\src\build\release. Application compilation might have failed."
    exit /b 1
)
echo "cZip.exe found."

if "%SKIP_DEPLOYMENT%"=="false" (
    echo "Cleaning previous deployed_app directory on host..."
    if exist "%CZIP_DIR%\src\build\deployed_app" rd /s /q "%CZIP_DIR%\src\build\deployed_app"
    
    set "TEMP_DEPLOY_DIR_HOST=%CZIP_DIR%\src\build\deployed_app"

    echo "Running deployment script inside container..."
    docker run --rm -it --mount type=bind,source="%CZIP_DIR_DOCKER%",target=C:/project -w C:/project/src qt5.15-qca-win cmd /c "C:\project\docker\windows\deploy_app.bat"
	if %errorlevel% neq 0 (
        echo "ERROR: Deployment script failed."
        exit /b 1
    )
    echo "Deployment successful."

    echo "Zipping the portable application..."
    powershell.exe -Command "Add-Type -AssemblyName System.IO.Compression.FileSystem; [System.IO.Compression.ZipFile]::CreateFromDirectory('!TEMP_DEPLOY_DIR_HOST!', '%CZIP_DIR%\src\build\cZip.zip')"
    if %errorlevel% neq 0 (
        echo "ERROR: Zipping failed."
        exit /b 1
    )
    echo "Zipping successful."
) else (
    echo "Portable application creation skipped."
)

echo "Done!"
endlocal