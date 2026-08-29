# Iluminate Firmware ESP32

ESP32 firmware for Iluminate `partitura.v1` validation and runtime development.

Current scope:

- PlatformIO + Arduino framework.
- FastLED output rendering.
- ArduinoJson partitura parsing.
- Three logical outputs are always present.
- Unused outputs are represented with `pixelCount: 0`.
- First hardware target: one WS2812B strip, 100 LEDs, output 1 on GPIO 25.
- Default scene: `calibration`.

Local upload from Windows:

```bat
pio device list
pio run -t upload
pio device monitor
```

Close the monitor before uploading again, because the COM port can only be opened by one process at a time.

## Windows Helper Scripts

From a VS Code terminal in this repository:

```bat
scripts\windows\pull.bat
scripts\windows\build.bat
scripts\windows\upload.bat
scripts\windows\monitor.bat
```

For the normal update cycle:

```bat
scripts\windows\update-build-upload.bat
```

The same commands are available as VS Code tasks:

```text
Terminal -> Run Task...
```

Then choose:

```text
Firmware: Pull
Firmware: Build
Firmware: Upload
Firmware: Monitor
Firmware: Update Build Upload
```

The Windows scripts first try `pio` from `PATH`; if VS Code runs them from a plain `cmd.exe`, they fall back to PlatformIO's default extension install path:

```text
%USERPROFILE%\.platformio\penv\Scripts\pio.exe
```

Recommended workflow:

1. Codex changes firmware in the server clone.
2. Codex commits and pushes to GitHub.
3. The Windows flashing workstation runs `scripts\windows\pull.bat`.
4. The Windows flashing workstation runs `scripts\windows\build.bat`.
5. If build succeeds, run `scripts\windows\upload.bat`.
6. Use `scripts\windows\monitor.bat` only when serial output is needed, and close it before uploading again.
