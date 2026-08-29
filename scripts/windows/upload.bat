@echo off
setlocal
cd /d "%~dp0..\.."

echo.
echo == Iluminate Firmware: upload ==
echo Close any serial monitor before uploading.
pio run -t upload
if errorlevel 1 goto fail

echo.
echo Upload succeeded.
exit /b 0

:fail
echo.
echo Upload failed. Check that the ESP32 is connected and COM is not busy.
exit /b 1
