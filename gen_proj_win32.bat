@echo off

REM Call the Windows GenerateProjects script
call "%~dp0\Scripts\Windows\GenerateProjects.bat" %*

exit /b %errorlevel%
