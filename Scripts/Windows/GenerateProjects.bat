@echo off

SET WORKSPACE_ROOT=%~dp0\..\..

pushd "%WORKSPACE_ROOT%"

REM If no argument is provided, default to Visual Studio 2022 generator
if "%~1"=="" (
    echo No action specified. Defaulting to Visual Studio 2022 generator.
    cmake -S . -B Build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
) else (
    cmake -S . -B Build %*
)

popd