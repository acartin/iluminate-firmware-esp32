@echo off
setlocal
cd /d "%~dp0..\.."
call "%~dp0pio-env.bat"
if errorlevel 1 goto build_fail

echo.
echo == Iluminate Firmware: pull, build, upload ==
echo Close any serial monitor before continuing.

git pull --ff-only
if errorlevel 1 goto pull_fail

"%PIO_CMD%" run
if errorlevel 1 goto build_fail

"%PIO_CMD%" run -t upload
if errorlevel 1 goto upload_fail

echo.
echo Firmware updated and uploaded.
exit /b 0

:pull_fail
echo.
echo Pull failed. Resolve the message above before build/upload.
exit /b 1

:build_fail
echo.
echo Build failed. Upload was not attempted.
exit /b 1

:upload_fail
echo.
echo Upload failed. Check that the ESP32 is connected and COM is not busy.
exit /b 1
