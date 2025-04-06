@echo off
setlocal

set "PROPERTIES_FILE=%~1"

if not exist "%PROPERTIES_FILE%" (
    echo Properties file not found: %PROPERTIES_FILE%
    exit /b 1
)

for /F "tokens=1* delims==" %%A in (%PROPERTIES_FILE%) do (
    set "%%A=%%B"
)

endlocal
