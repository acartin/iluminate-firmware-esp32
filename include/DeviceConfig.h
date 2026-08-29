#pragma once

#include <Arduino.h>

struct DeviceConfig {
  String wifiSsid;
  String wifiPassword;
  String apiBaseUrl;
  String controllerKey;
};

bool loadDeviceConfig(DeviceConfig &config);
void saveDeviceConfig(const DeviceConfig &config);
void clearDeviceConfig();
bool hasRequiredDeviceConfig(const DeviceConfig &config);

