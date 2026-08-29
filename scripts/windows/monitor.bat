@echo off
setlocal
cd /d "%~dp0..\.."
call "%~dp0pio-env.bat"
if errorlevel 1 exit /b 1

echo.
echo == Iluminate Firmware: serial monitor ==
echo Press Ctrl+C to close the monitor before uploading again.
"%PIO_CMD%" device monitor -b 115200
