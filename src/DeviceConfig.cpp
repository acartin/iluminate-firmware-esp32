#include "DeviceConfig.h"

#include <Preferences.h>

namespace {
const char *NAMESPACE = "iluminate";
const char *KEY_WIFI_SSID = "wifi_ssid";
const char *KEY_WIFI_PASS = "wifi_pass";
const char *KEY_API_BASE = "api_base";
const char *KEY_CTRL_KEY = "ctrl_key";
}

bool loadDeviceConfig(DeviceConfig &config) {
  Preferences preferences;
  if (!preferences.begin(NAMESPACE, true)) return false;
  config.wifiSsid = preferences.getString(KEY_WIFI_SSID, "");
  config.wifiPassword = preferences.getString(KEY_WIFI_PASS, "");
  config.apiBaseUrl = preferences.getString(KEY_API_BASE, "");
  config.controllerKey = preferences.getString(KEY_CTRL_KEY, "");
  preferences.end();
  return hasRequiredDeviceConfig(config);
}

void saveDeviceConfig(const DeviceConfig &config) {
  Preferences preferences;
  preferences.begin(NAMESPACE, false);
  preferences.putString(KEY_WIFI_SSID, config.wifiSsid);
  preferences.putString(KEY_WIFI_PASS, config.wifiPassword);
  preferences.putString(KEY_API_BASE, config.apiBaseUrl);
  preferences.putString(KEY_CTRL_KEY, config.controllerKey);
  preferences.end();
}

void clearDeviceConfig() {
  Preferences preferences;
  preferences.begin(NAMESPACE, false);
  preferences.clear();
  preferences.end();
}

bool hasRequiredDeviceConfig(const DeviceConfig &config) {
  return config.wifiSsid.length() > 0 &&
         config.apiBaseUrl.length() > 0 &&
         config.controllerKey.length() > 0;
}

