@echo off
SETLOCAL EnableDelayedExpansion

set "mode=%~1"
if "%mode%"=="" (
    echo Usage: manage_version.cmd [dev^|release] [version_file_path]
    echo.
    echo   dev     - Sets VERSION_PREFIX to "dev" (for development work^^^)
    echo   release - Sets VERSION_PREFIX to "" and increments VERSION_MINOR (for public release^^^)
    exit /b 1
)

:: Set up the base directory
set origdir=%cd%
cd /d %~dp0..\..
set base_dir=%cd%
cd /d %origdir%

:: Define the list of version files
set versionFiles[0]="%base_dir%\src\nxemu-core\version.h.in"
set versionFiles[1]="%base_dir%\src\nxemu-cpu\version.h.in"
set versionFiles[2]="%base_dir%\src\nxemu-os\version.h.in"
set versionFiles[3]="%base_dir%\src\nxemu-video\version.h.in"

:: Check if PowerShell script exists
set "psScript=%~dp0manage_version_powershell.ps1"
if not exist "!psScript!" (
    echo Error: PowerShell script not found: !psScript!
    exit /b 1
)

echo Version management mode: %mode%
echo.

:: Process each file in the list
set fileIndex=0
:processFiles
if not defined versionFiles[%fileIndex%] goto :endProcessFiles

:: Get the current file path
call set "currentFile=%%versionFiles[%fileIndex%]%%"
echo Processing file !fileIndex!: !currentFile!

:: Execute PowerShell script for the current file
powershell -ExecutionPolicy Bypass -File "!psScript!" -Mode "%mode%" -VersionFile !currentFile!

if %ERRORLEVEL% neq 0 (
    echo Error: PowerShell script failed for file !currentFile!
    set /a "fileIndex+=1"
    goto :processFiles
)

echo.
set /a "fileIndex+=1"
goto :processFiles

:endProcessFiles
echo Version management completed for all files.
exit /b 0