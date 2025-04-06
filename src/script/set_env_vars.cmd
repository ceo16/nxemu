@echo off

if not exist "%~1" (
    echo Properties file not found: %~1
    exit /b 1
)

for /F "tokens=1* delims== " %%A in (%~1) do (
    set "%%A=%%B"
)