@echo off

REM Check if Hugo is available
where hugo >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Hugo was not found on your system.
    echo Please install Hugo from https://gohugo.io/installation/
    echo After installing, restart this script.
    pause
    exit /b 1
)

pushd "%~dp0\Documentation\"
start "" http://localhost:1313
hugo server -D
popd

exit /b 0