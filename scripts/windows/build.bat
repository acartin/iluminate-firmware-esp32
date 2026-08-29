@echo off
setlocal
cd /d "%~dp0..\.."

echo.
echo == Iluminate Firmware: build ==
pio run
if errorlevel 1 goto fail

echo.
echo Build succeeded.
exit /b 0

:fail
echo.
echo Build failed. Send the output above for review.
exit /b 1
