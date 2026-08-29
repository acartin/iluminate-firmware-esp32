@echo off
setlocal
cd /d "%~dp0..\.."

echo.
echo == Iluminate Firmware: serial monitor ==
echo Press Ctrl+C to close the monitor before uploading again.
pio device monitor -b 115200
