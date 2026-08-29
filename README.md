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

