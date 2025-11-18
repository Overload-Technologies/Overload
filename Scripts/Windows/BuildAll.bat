@echo off
setlocal enabledelayedexpansion

:: Get the configuration parameter
set "CONFIGURATION=%~1"

:: Generate the projects
pushd "%~dp0\..\"
call GenerateProjects vs2022 %CONFIGURATION%
popd

:: Initialize variables
set "VSWHERE_PATH="
set "PROGRAM_FILES_X86=%ProgramFiles(x86)%"
if not defined PROGRAM_FILES_X86 set "PROGRAM_FILES_X86=%ProgramFiles%"

:: Common locations to check
set "LOCATIONS[0]=%PROGRAM_FILES_X86%\Microsoft Visual Studio\Installer\vswhere.exe"
set "LOCATIONS[1]=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
set "LOCATIONS[2]=%PROGRAM_FILES_X86%\Common Files\Microsoft\VSWhere\vswhere.exe"

echo Searching for vswhere.exe...

:: Check each location
for /L %%i in (0,1,2) do (
    if exist "!LOCATIONS[%%i]!" (
        set "VSWHERE_PATH=!LOCATIONS[%%i]!"
        goto :found
    )
)

:: Not found in common locations, try PATH
for %%i in (vswhere.exe) do set "VSWHERE_IN_PATH=%%~$PATH:i"
if defined VSWHERE_IN_PATH (
    set "VSWHERE_PATH=!VSWHERE_IN_PATH!"
    goto :found
)

:notfound
echo vswhere.exe was not found in common locations or PATH.
echo You may need to install Visual Studio or add vswhere to your PATH.
exit /b 1

:found
echo vswhere.exe found at: "!VSWHERE_PATH!"
echo.

:: Get the msbuild path using vswhere
echo Searching for msbuild.exe...

for /f "tokens=*" %%i in ('"!VSWHERE_PATH!" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do set "MSBUILD_PATH=%%i"

if not defined MSBUILD_PATH (
    echo msbuild.exe was not found using vswhere.
    exit /b 1
)

echo msbuild.exe found at: "!MSBUILD_PATH!"
echo.

:: Build the solution
echo Building Overload in !CONFIGURATION! mode...
echo.

pushd "%~dp0..\..\"
if "%CONFIGURATION%"=="" (
    echo Building with default configuration configuration...
    "!MSBUILD_PATH!" Overload.sln -m -verbosity:minimal
) else (
    echo Building with configuration: %CONFIGURATION%...
    "!MSBUILD_PATH!" Overload.sln -m -verbosity:minimal /p:Configuration=%CONFIGURATION% 
)
popd

if %errorlevel% neq 0 (
    echo Build failed.
    exit /b %errorlevel%
)

echo Build completed successfully.

endlocal & set "VSWHERE=%VSWHERE%"
exit /b 0
