pushd "%~dp0"

set NO_OPEN=false
if "%1"=="--no-open" set NO_OPEN=true

:: Generate projects
call .\GenerateProjects.bat

:: Build Debug
call .\Build.bat Debug -skip-project-generation
if %ERRORLEVEL% neq 0 (
    echo Debug build failed. Exiting.
    exit /b %ERRORLEVEL%
)

:: Build Release
call .\Build.bat Release -skip-project-generation
if %ERRORLEVEL% neq 0 (
    echo Release build failed. Exiting.
    exit /b %ERRORLEVEL%
)

:: Build Publish
call .\Build.bat Publish -skip-project-generation
if %ERRORLEVEL% neq 0 (
    echo Publish build failed. Exiting.
    exit /b %ERRORLEVEL%
)

:: The output is located in a folder called ..\..\Build\Publish. Create an archive with this folder's content, and name the archive Overload-<version>-<platform>.zip.
for /f "delims=" %%v in (..\..\VERSION.txt) do set version=%%v
set platform=windows_x64

:: Navigate to the release folder
pushd ..\..\Build\

:: Delete any existing folder with the name Overload-%version%-%platform%
if exist Overload-%version%-%platform% (
    rmdir /s /q Overload-%version%-%platform%
)

:: Copy the folder "Publish" to a new folder called "Overload-%version%-%platform%"
xcopy Publish\ ..\Releases\Overload-%version%-%platform% /E /I

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
