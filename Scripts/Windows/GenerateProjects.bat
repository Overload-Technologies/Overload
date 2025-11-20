@echo off

SET WORKSPACE_ROOT=%~dp0\..\..

pushd "%WORKSPACE_ROOT%"

REM If no argument is provided, default to vs2022
if "%~1"=="" (
    echo No action specified. Defaulting to 'vs2022'.
    call "Dependencies\premake5\bin\premake5.exe" vs2022
) else (
    call "Dependencies\premake5\bin\premake5.exe" %*
)

popd