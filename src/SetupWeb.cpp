#include "SetupWeb.h"

#include <ESPmDNS.h>
#include <WiFi.h>

namespace {
const IPAddress SETUP_IP(192, 168, 4, 1);
const IPAddress SETUP_GATEWAY(192, 168, 4, 1);
const IPAddress SETUP_SUBNET(255, 255, 255, 0);
}

SetupWeb::SetupWeb(DeviceConfig &deviceConfig, const char *version)
    : config(deviceConfig),
      firmwareVersion(version),
      server(80),
      setupPortalActive(false),
      downloadPartituraHandler(nullptr),
      webConnectionStatusHandler(nullptr) {}

void SetupWeb::onDownloadPartitura(DownloadPartituraHandler handler) {
  downloadPartituraHandler = handler;
}

void SetupWeb::onWebConnectionStatus(WebConnectionStatusHandler handler) {
  webConnectionStatusHandler = handler;
}

void SetupWeb::beginSetupPortal() {
  setupPortalActive = true;
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(SETUP_IP, SETUP_GATEWAY, SETUP_SUBNET);
  WiFi.softAP(setupSsid().c_str());
  registerRoutes();
  server.begin();

  Serial.print("Setup portal SSID: ");
  Serial.println(setupSsid());
  Serial.println("Setup portal URL: http://192.168.4.1");
}

void SetupWeb::beginRuntimeWeb() {
  setupPortalActive = false;
  registerRoutes();
  server.begin();

  if (MDNS.begin("iluminate-esp32")) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS: http://iluminate-esp32.local");
  }

  Serial.print("Runtime web URL: http://");
  Serial.println(WiFi.localIP());
}

void SetupWeb::handleClient() {
  server.handleClient();
}

bool SetupWeb::isSetupPortalActive() const {
  return setupPortalActive;
}

void SetupWeb::registerRoutes() {
  server.on("/", HTTP_GET, [this]() { handleHome(); });
  server.on("/partitura", HTTP_GET, [this]() { handlePartitura(); });
  server.on("/partitura/download", HTTP_POST, [this]() { handleDownloadPartitura(); });
  server.on("/setup/wifi", HTTP_GET, [this]() { handleWifiForm(); });
  server.on("/setup/wifi", HTTP_POST, [this]() { handleSaveWifi(); });
  server.on("/setup/clear", HTTP_POST, [this]() { handleClearConfig(); });
  server.onNotFound([this]() { handleNotFound(); });
}

void SetupWeb::handleHome() {
  Serial.println("HTTP GET /");
  Serial.flush();
  String body;
  body += "<div class='grid'>";
  body += "<a class='card' href='/setup/wifi'><strong>Setup / WiFi</strong><span>Configure WiFi, API URL and controller key.</span></a>";
  body += "<a class='card' href='/partitura'><strong>Partitura</strong><span>Download and apply the generated lighting JSON.</span></a>";
  body += "</div>";
  body += "<section><h2>Status</h2>";
  body += "<dl>";
  body += "<dt>Mode</dt><dd>" + String(setupPortalActive ? "Setup AP" : "Runtime") + "</dd>";
  body += "<dt>WiFi</dt><dd>" + htmlEscape(WiFi.status() == WL_CONNECTED ? String("Connected to ") + WiFi.SSID() : "Not connected") + "</dd>";
  body += "<dt>IP</dt><dd>" + htmlEscape(setupPortalActive ? WiFi.softAPIP().toString() : WiFi.localIP().toString()) + "</dd>";
  body += "<dt>API</dt><dd>" + htmlEscape(config.apiBaseUrl.length() ? config.apiBaseUrl : "Not configured") + "</dd>";
  body += "<dt>Controller</dt><dd>" + htmlEscape(config.controllerKey.length() ? config.controllerKey : "Not configured") + "</dd>";
  body += "<dt>Web</dt><dd>" + String(webConnectionStatusHandler && webConnectionStatusHandler() ? "Connected" : "Not verified") + "</dd>";
  body += "<dt>Firmware</dt><dd>" + htmlEscape(firmwareVersion) + "</dd>";
  body += "<dt>Heap</dt><dd>" + String(ESP.getFreeHeap()) + " bytes</dd>";
  body += "</dl></section>";
  server.send(200, "text/html", page("Iluminate Controller", body));
}

void SetupWeb::handlePartitura() {
  Serial.println("HTTP GET /partitura");
  Serial.flush();
  String body;
  body += "<section><h2>Download</h2><dl>";
  body += "<dt>API</dt><dd>" + htmlEscape(config.apiBaseUrl.length() ? config.apiBaseUrl : "Not configured") + "</dd>";
  body += "<dt>Controller</dt><dd>" + htmlEscape(config.controllerKey.length() ? config.controllerKey : "Not configured") + "</dd>";
  body += "<dt>Web</dt><dd>" + String(webConnectionStatusHandler && webConnectionStatusHandler() ? "Connected" : "Not verified") + "</dd>";
  body += "<dt>Firmware</dt><dd>" + htmlEscape(firmwareVersion) + "</dd>";
  body += "</dl></section>";
  body += "<form method='post' action='/partitura/download'>";
  body += "<p>Download the generated partitura from the configured API and apply it immediately.</p>";
  body += "<div class='actions'><button type='submit'>Download partitura</button><a href='/'>Back</a></div>";
  body += "</form>";
  server.send(200, "text/html", page("Partitura", body));
}

void SetupWeb::handleDownloadPartitura() {
  String message;
  Serial.println("Manual partitura download requested.");
  bool ok = downloadPartituraHandler && downloadPartituraHandler(message);
  Serial.print(ok ? "Manual partitura download succeeded: " : "Manual partitura download failed: ");
  Serial.println(message.length() ? message : "No detail available.");
  Serial.flush();
  String body;
  body += ok ? "<section class='ok'>" : "<section class='error'>";
  body += "<h2>" + String(ok ? "Downloaded" : "Download failed") + "</h2>";
  body += "<p>" + htmlEscape(message.length() ? message : "No detail available.") + "</p>";
  body += "</section>";
  body += "<p><a href='/partitura'>Back to partitura</a></p>";
  server.send(ok ? 200 : 502, "text/html", page("Partitura", body));
}

void SetupWeb::handleWifiForm() {
  String body;
  body += "<form method='post' action='/setup/wifi'>";
  body += "<label>WiFi SSID<input name='wifi_ssid' value='" + htmlEscape(config.wifiSsid) + "' required></label>";
  body += "<label>WiFi password<input name='wifi_password' type='password' value='" + htmlEscape(config.wifiPassword) + "'></label>";
  body += "<label>API base URL<input name='api_base_url' value='" + htmlEscape(config.apiBaseUrl) + "' placeholder='http://192.168.1.10:8420' required></label>";
  body += "<label>Controller key<input name='controller_key' value='" + htmlEscape(config.controllerKey) + "' placeholder='esp32-dev-001' required></label>";
  body += "<div class='actions'><button type='submit'>Save and restart</button><a href='/'>Cancel</a></div>";
  body += "</form>";
  body += "<form method='post' action='/setup/clear' class='danger'><button type='submit'>Clear config and restart</button></form>";
  server.send(200, "text/html", page("Setup / WiFi", body));
}

void SetupWeb::handleSaveWifi() {
  config.wifiSsid = server.arg("wifi_ssid");
  config.wifiPassword = server.arg("wifi_password");
  config.apiBaseUrl = server.arg("api_base_url");
  config.controllerKey = server.arg("controller_key");
  saveDeviceConfig(config);
  String body;
  body += "<p>Configuration saved. Restarting...</p>";
  body += "<p>The controller will return at <a href='http://iluminate-esp32.local/'>http://iluminate-esp32.local/</a> when WiFi is ready.</p>";
  body += "<script>setTimeout(function(){location.href='http://iluminate-esp32.local/';},9000);</script>";
  server.send(200, "text/html", page("Saved", body));
  delay(700);
  ESP.restart();
}

void SetupWeb::handleClearConfig() {
  clearDeviceConfig();
  server.send(200, "text/html", page("Cleared", "<p>Configuration cleared. Restarting into setup...</p>"));
  delay(700);
  ESP.restart();
}

void SetupWeb::handleNotFound() {
  server.send(404, "text/html", page("Not Found", "<p>Route not found.</p><p><a href='/'>Back home</a></p>"));
}

String SetupWeb::page(const String &title, const String &body) const {
  String html;
  html += "<!doctype html><html><head><meta charset='utf-8'>";
  html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<title>" + htmlEscape(title) + "</title>";
  html += "<style>";
  html += "body{margin:0;background:#111827;color:#e5e7eb;font-family:Arial,sans-serif;}";
  html += "main{max-width:760px;margin:0 auto;padding:24px;}h1{font-size:24px;font-weight:500;margin:0 0 18px;}h2{font-size:15px;margin:22px 0 10px;color:#cbd5e1;}";
  html += ".grid{display:grid;gap:12px}.card{display:block;border:1px solid #334155;border-radius:8px;padding:14px;color:#e5e7eb;text-decoration:none;background:#1f2937}.card span{display:block;margin-top:5px;color:#94a3b8;font-size:13px}";
  html += "dl{display:grid;grid-template-columns:130px 1fr;gap:8px 12px;border:1px solid #334155;border-radius:8px;padding:14px;background:#0f172a}dt{color:#94a3b8}dd{margin:0;word-break:break-word}";
  html += "section.ok,section.error{border:1px solid #334155;border-radius:8px;padding:14px;background:#0f172a}.ok h2{color:#86efac}.error h2{color:#fca5a5}p{color:#cbd5e1;line-height:1.45}a{color:#7dd3fc}";
  html += "form{display:grid;gap:14px;border:1px solid #334155;border-radius:8px;padding:14px;background:#0f172a}label{display:grid;gap:6px;color:#cbd5e1;font-size:13px}input{height:38px;border-radius:6px;border:1px solid #475569;background:#111827;color:#f8fafc;padding:0 10px;font-size:15px}";
  html += "button,.actions a{height:38px;border:0;border-radius:6px;background:#38bdf8;color:#082f49;padding:0 14px;font-weight:700;text-decoration:none;display:inline-flex;align-items:center}.actions{display:flex;gap:10px;align-items:center}.danger button{background:#fca5a5;color:#450a0a}";
  html += "</style></head><body><main>";
  html += "<h1>" + htmlEscape(title) + "</h1>";
  html += body;
  html += "</main></body></html>";
  return html;
}

String SetupWeb::htmlEscape(const String &value) const {
  String escaped = value;
  escaped.replace("&", "&amp;");
  escaped.replace("<", "&lt;");
  escaped.replace(">", "&gt;");
  escaped.replace("\"", "&quot;");
  escaped.replace("'", "&#39;");
  return escaped;
}

String SetupWeb::setupSsid() const {
  uint64_t chipId = ESP.getEfuseMac();
  char suffix[7];
  snprintf(suffix, sizeof(suffix), "%06X", (uint32_t)(chipId & 0xFFFFFF));
  return String("Iluminate-Setup-") + suffix;
}
