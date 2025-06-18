:: deploy_app.bat
@echo off
setlocal

echo "--- Starting application deployment script inside container..."

:: --- Define all paths relative to the new C:\project structure ---
set "QT_BIN_PATH=C:\Qt\5.15.2\msvc2019_64\bin"
set "QT_PLUGINS_PATH=C:\Qt\5.15.2\msvc2019_64\plugins"
set "VCVARS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set "OPENSSL_BIN_PATH=C:\Program Files\OpenSSL-Win64\bin"

set "APP_EXE_PATH=C:\project\src\build\release\czip.exe"
set "DEPLOY_DIR=C:\project\src\build\deployed_app"
set "AURA_DEPS_INSTALL_PATH=C:\project\external\AURA\build\deps_install\bin" 

:: 1. Create the deployment directory before using it
echo "--- Creating deployment directory..."
mkdir "%DEPLOY_DIR%"
if %errorlevel% neq 0 ( exit /b 1 )

:: 2. Copy the main application executable
echo "--- Copying czip.exe to the deployment directory..."
copy "%APP_EXE_PATH%" "%DEPLOY_DIR%\"
if %errorlevel% neq 0 ( exit /b 1 )

:: 3. Set up the Visual Studio build environment
echo "--- Setting up Visual Studio build environment..."
call "%VCVARS_PATH%"
if %errorlevel% neq 0 ( exit /b 1 )

:: 4. Run windeployqt to add all necessary Qt DLLs
echo "--- Running windeployqt to deploy dependencies..."
"%QT_BIN_PATH%\windeployqt.exe" "%DEPLOY_DIR%\czip.exe" --dir "%DEPLOY_DIR%" --qmldir "C:\project\src"
if %errorlevel% neq 0 ( exit /b 1 )

:: 5. Manually copy the main QCA library DLL
echo "--- Copying main QCA library..."
copy "%QT_BIN_PATH%\qca-qt5.dll" "%DEPLOY_DIR%\"
if %errorlevel% neq 0 ( exit /b 1 )

:: 6. Copy the QCA OpenSSL plugin to its required subdirectory
echo "--- Copying QCA plugin..."
set "QCA_PLUGINS_DEST=%DEPLOY_DIR%\plugins\crypto"
mkdir "%QCA_PLUGINS_DEST%"
copy "%QT_PLUGINS_PATH%\crypto\qca-ossl.dll" "%QCA_PLUGINS_DEST%\"
if %errorlevel% neq 0 ( exit /b 1 )

:: 7. Copy the OpenSSL DLLs required by the QCA plugin
echo "--- Copying OpenSSL DLLs..."
copy "%OPENSSL_BIN_PATH%\*.dll" "%DEPLOY_DIR%\"
if %errorlevel% neq 0 ( exit /b 1 )

:: 8. Copy Botan DLLs
echo "--- Copying Botan DLLs..."
if exist "%AURA_DEPS_INSTALL_PATH%\botan.dll" (
    copy "%AURA_DEPS_INSTALL_PATH%\botan.dll" "%DEPLOY_DIR%\"
    echo "Copied botan.dll"
) else if exist "%AURA_DEPS_INSTALL_PATH%\botand.dll" (
    copy "%AURA_DEPS_INSTALL_PATH%\botand.dll" "%DEPLOY_DIR%\"
    echo "Copied botand.dll"
) else if exist "%AURA_DEPS_INSTALL_PATH%\botan-2.dll" (
    copy "%AURA_DEPS_INSTALL_PATH%\botan-2.dll" "%DEPLOY_DIR%\"
    echo "Copied botan-2.dll"
) else if exist "%AURA_DEPS_INSTALL_PATH%\botan-2d.dll" (
    copy "%AURA_DEPS_INSTALL_PATH%\botan-2d.dll" "%DEPLOY_DIR%\"
    echo "Copied botan-2d.dll"
) else (
    echo "WARNING: Neither botan.dll, botand.dll, botan-2.dll, nor botan-2d.dll found in Botan install path. Botan might be statically linked or missing."
    exit /b 1
)

:: 9. Clean up unused directories created by windeployqt
echo "--- Cleaning up unused directories..."
if exist "%DEPLOY_DIR%\translations" rd /s /q "%DEPLOY_DIR%\translations"
if exist "%DEPLOY_DIR%\styles" rd /s /q "%DEPLOY_DIR%\styles"
if exist "%DEPLOY_DIR%\iconengines" rd /s /q "%DEPLOY_DIR%\iconengines"
if exist "%DEPLOY_DIR%\imageformats" rd /s /q "%DEPLOY_DIR%\imageformats"

echo "--- Deployment script finished successfully."
exit /b 0