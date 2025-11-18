@echo off
SET version=%~1%
if "%~1"=="" SET version="vs2022"

SET premake_path="%~dp0\..\..\Dependencies\premake5\bin\premake5.exe"

pushd "%~dp0\..\.."
call %premake_path% %version%
popd
