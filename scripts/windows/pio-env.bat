@echo off
set "PIO_CMD=pio"

where pio >nul 2>nul
if not errorlevel 1 exit /b 0

if exist "%USERPROFILE%\.platformio\penv\Scripts\pio.exe" (
  set "PIO_CMD=%USERPROFILE%\.platformio\penv\Scripts\pio.exe"
  exit /b 0
)

if exist "%USERPROFILE%\.platformio\penv\Scripts\platformio.exe" (
  set "PIO_CMD=%USERPROFILE%\.platformio\penv\Scripts\platformio.exe"
  exit /b 0
)

echo PlatformIO CLI was not found.
echo Open PlatformIO Home once or install PlatformIO Core, then retry.
exit /b 1
