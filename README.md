# Iluminate Firmware ESP32

ESP32 firmware for Iluminate `partitura.v1` validation and runtime development.

Current scope:

- PlatformIO + Arduino framework.
- FastLED output rendering.
- ArduinoJson partitura parsing.
- ESP32 local setup/runtime web.
- NVS device configuration through `Preferences`.
- HTTP download of the generated partitura from the configured Iluminate web/API.
- Three logical outputs are always present.
- Unused outputs are represented with `pixelCount: 0`.
- First hardware target: one WS2812B strip, 100 LEDs, output 1 on GPIO 25.
- Default scene: `calibration`.
- Web/API status LED on GPIO 2.

Local upload from Windows:

```bat
pio device list
pio run -t upload
pio device monitor
```

Close the monitor before uploading again, because the COM port can only be opened by one process at a time.

## Local Controller Web

The firmware exposes a small local web UI.

Startup rules:

1. If no saved config exists, the ESP32 starts setup AP mode.
2. If saved config exists, the ESP32 tries to connect to WiFi for 25 seconds.
3. If WiFi connects, runtime web is available at the ESP32 LAN IP.
4. If WiFi fails, setup AP mode starts automatically.

Setup AP:

```text
SSID: Iluminate-Setup-XXXXXX
URL:  http://192.168.4.1
```

Runtime web:

```text
http://<esp32-lan-ip>/
http://iluminate-esp32.local/ when mDNS is available
```

Current menu:

- `Setup / WiFi`
- `Partitura`
- status summary

Saved config fields:

- WiFi SSID
- WiFi password
- API base URL
- controller key

Partitura download:

```text
<api-base-url>/api/device/partituras/default_installation
```

The runtime downloads this generated JSON automatically after WiFi connects. It can also be triggered manually from:

```text
http://<esp32-lan-ip>/partitura
```

For this spike, the downloaded partitura is applied in RAM immediately. Persistence of the last known good downloaded partitura is a separate next step.

Status LED:

- GPIO 2 ON: the last web/API partitura download succeeded.
- GPIO 2 OFF: web/API has not been verified or the last download failed.

Button notes:

- `EN` is reset, not a normal input button.
- `BOOT` is usually GPIO0 and affects boot mode. It is useful as a rescue input only with care, so this version does not depend on it.

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
