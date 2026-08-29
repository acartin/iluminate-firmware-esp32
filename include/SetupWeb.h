#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include "DeviceConfig.h"

typedef bool (*DownloadPartituraHandler)(String &message);
typedef bool (*WebConnectionStatusHandler)();

class SetupWeb {
public:
  explicit SetupWeb(DeviceConfig &config);

  void onDownloadPartitura(DownloadPartituraHandler handler);
  void onWebConnectionStatus(WebConnectionStatusHandler handler);
  void beginSetupPortal();
  void beginRuntimeWeb();
  void handleClient();
  bool isSetupPortalActive() const;

private:
  DeviceConfig &config;
  WebServer server;
  bool setupPortalActive;
  DownloadPartituraHandler downloadPartituraHandler;
  WebConnectionStatusHandler webConnectionStatusHandler;

  void registerRoutes();
  void handleHome();
  void handlePartitura();
  void handleDownloadPartitura();
  void handleWifiForm();
  void handleSaveWifi();
  void handleClearConfig();
  void handleNotFound();
  String page(const String &title, const String &body) const;
  String htmlEscape(const String &value) const;
  String setupSsid() const;
};
