@echo off

call "%~dp0\Scripts\Windows\GenerateProjects.bat" %*

exit /b %errorlevel%
