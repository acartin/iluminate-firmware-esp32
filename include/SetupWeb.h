#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include "DeviceConfig.h"

class SetupWeb {
public:
  explicit SetupWeb(DeviceConfig &config);

  void beginSetupPortal();
  void beginRuntimeWeb();
  void handleClient();
  bool isSetupPortalActive() const;

private:
  DeviceConfig &config;
  WebServer server;
  bool setupPortalActive;

  void registerRoutes();
  void handleHome();
  void handleWifiForm();
  void handleSaveWifi();
  void handleClearConfig();
  void handleNotFound();
  String page(const String &title, const String &body) const;
  String htmlEscape(const String &value) const;
  String setupSsid() const;
};

