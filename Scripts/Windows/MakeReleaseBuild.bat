pushd "%~dp0"

set NO_OPEN=false
if "%1"=="--no-open" set NO_OPEN=true

call .\Build.bat Debug
if %ERRORLEVEL% neq 0 (
    echo Debug build failed. Exiting.
    exit /b %ERRORLEVEL%
)

call .\Build.bat Release
if %ERRORLEVEL% neq 0 (
    echo Release build failed. Exiting.
    exit /b %ERRORLEVEL%
)

:: The output is located in a folder called ..\..\Build\Release. Create an archive with this folder's content, and name the archive Overload-<version>-<platform>.zip.
for /f "delims=" %%v in (..\..\VERSION.txt) do set version=%%v
set platform=windows_x64

:: Navigate to the release folder
pushd ..\..\Build\

:: Delete any existing folder with the name Overload-%version%-%platform%
if exist Overload-%version%-%platform% (
    rmdir /s /q Overload-%version%-%platform%
)

:: Copy the folder "Release" to a new folder called "Overload-%version%-%platform%"
xcopy Release\ ..\Releases\Overload-%version%-%platform% /E /I

:: Ensure the Releases folder exists
if not exist ..\Releases (
    mkdir ..\Releases
)

:: Create the archive, but first delete any existing archive with the same name
if exist ..\Releases\Overload-%version%-%platform%.zip (
    del ..\Releases\Overload-%version%-%platform%.zip
)
powershell Compress-Archive -Path ..\Releases\Overload-%version%-%platform%\ -DestinationPath ..\Releases\Overload-%version%-%platform%.zip -Force
echo Archive created: ..\Releases\Overload-%version%-%platform%.zip

:: Delete temporary build
if exist ..\Releases\Overload-%version%-%platform% (
    rmdir /s /q ..\Releases\Overload-%version%-%platform%
    echo Temporary build deleted.
)

:: Open the output folder in the file explorer (if not disabled)
if "%NO_OPEN%"=="false" explorer ..\Releases

:: Return to the original directory
popd
popd

exit /b 0
