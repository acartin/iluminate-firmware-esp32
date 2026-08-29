@echo off
setlocal
cd /d "%~dp0..\.."

echo.
echo == Iluminate Firmware: pull latest ==
git pull --ff-only
if errorlevel 1 goto fail

echo.
echo Done.
exit /b 0

:fail
echo.
echo Pull failed. Resolve the message above before continuing.
exit /b 1
