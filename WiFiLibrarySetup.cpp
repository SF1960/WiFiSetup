#include "WiFiSetupLibrary.h"

#define EEPROM_SIZE 256
#define SSID_ADDR 0
#define PASS_ADDR 64
#define AUTO_CONNECT_ADDR 128
#define DEVICE_ID_ADDR 129
#define SECRET_CODE_ADDR 193

// Ths library constructor with member initialisation
WiFiSetup::WiFiSetup(const char* apSSID, const char* apPassword)
    : _apSSID(apSSID), _apPassword(apPassword), _server(80), _setupInProgress(false) {}

bool WiFiSetup::isSSIDEmpty() {
  for (int i = 0; i < 64; i++) {
    if (EEPROM.read(SSID_ADDR + i) != 0) {
      return false; // Found a non-zero byte, SSID is not empty
    }
  }
  return true; // All bytes are zero, SSID is empty
}

void WiFiSetup::begin(bool overrideAutoConnect, bool forceAutoConnect, bool showDebug) {
  //Serial.begin(115200); this should be in the .ino file
  EEPROM.begin(EEPROM_SIZE);

  _showDebug = showDebug; // Set debug flag from parameter 

// save memory by not compiling Serial.printf statements
#if _showDebug
  #define DEBUG_PRINTF(format, ...) Serial.printf(format, ##__VA_ARGS__)
  #define DEBUG_PRINTF_LN(format, ...) { Serial.printf(format, ##__VA_ARGS__); Serial.println(); }
#else
  #define DEBUG_PRINTF(format, ...)
  #define DEBUG_PRINTF_LN(format, ...)
#endif


  if (forceAutoConnect) {
    autoConnect();
    _apMode = false;
  } else if (overrideAutoConnect) {
    startAP();
    _apMode = true;
  } else if (EEPROM.read(AUTO_CONNECT_ADDR) == 1 && !isSSIDEmpty()) {
    autoConnect();
    _apMode = false;
  } else {
    startAP();
    _apMode = true;
  }

  _server.on("/", [this]() { handleRoot(); });
  _server.on("/connect", HTTP_POST, [this]() { handleConnect(); });
  _server.begin();
  _setupInProgress = true; // Setup is now in progress
}

void WiFiSetup::autoConnect() {
  char ssid[64] = {0};
  char password[64] = {0};
  char deviceId[64] = {0};
  char secretCode[64] = {0};

  for (int i = 0; i < 64; i++) {
    ssid[i] = EEPROM.read(SSID_ADDR + i);
    password[i] = EEPROM.read(PASS_ADDR + i);
    deviceId[i] = EEPROM.read(DEVICE_ID_ADDR + i);
    secretCode[i] = EEPROM.read(SECRET_CODE_ADDR + i);
  }

  if (strlen(ssid) > 0) {
    
    Serial.printf("WiFiSetup::Automatically connecting to: %s\n", ssid);

    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      if(_showDebug){Serial.print(".");}
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      if(_showDebug){
      Serial.printf("\nWiFiSetup::WiFi connected!\n");
      Serial.printf("WiFiSetup::IP address: %s\n", WiFi.localIP().toString());
      if(_showDebug){Serial.printf("WiFiSetup::Device ID: %s\n", deviceId);}
      if(_showDebug){Serial.printf("WiFiSetup::Secret Code: %s\n", secretCode);}
      }
    } else {
      Serial.printf("\nWiFiSetup::WiFi connection failed. Starting AP.\n");
      startAP();
    }

  } else {
    if(_showDebug) {Serial.printf("WiFiSetup::No saved credentials. Starting AP.\n");}
    startAP();
  }
}

void WiFiSetup::startAP() {

  // Always display print statements in this routine despite _debugEnabled
  if(_showDebug){Serial.printf("WiFiSetup::Starting AP.\n");}

  WiFi.mode(WIFI_AP);
  WiFi.softAP(_apSSID, _apPassword);

  if(_showDebug){
    Serial.printf("Connect your mobile or PC to %s\n", _apSSID);
    Serial.printf("If requested, the password is %s\n", _apPassword);
    Serial.printf("Then connect your browser to %s\n", WiFi.softAPIP().toString());
    Serial.printf("If the page does not load, turn off your mobile data.\n");
    Serial.printf("Select the network from the list and enter the network's credentials\n");
  }

}

void WiFiSetup::handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>WiFi Setup</title>";
  html += "<style>";
  html += "body { font-family: sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; background-color: #f4f4f4; }";
  html += ".container { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1); width: 300px; }";
  html += ".container h1 { text-align: center; margin-bottom: 20px; color: #333; }";
  html += ".container label { display: block; margin-bottom: 5px; color: #555; }";
  html += ".container input[type='text'], .container input[type='password'], .container select { width: calc(100% - 12px); padding: 6px; margin-bottom: 10px; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }";
  html += ".container input[type='submit'] { width: 100%; background-color: #4CAF50; color: white; padding: 10px; border: none; border-radius: 4px; cursor: pointer; }";
  html += ".container input[type='submit']:hover { background-color: #45a049; }";
  html += ".container .checkbox-container { display: flex; align-items: center; margin-bottom: 10px; }";
  html += ".container .checkbox-container input[type='checkbox'] { margin-right: 5px; }";
  html += ".network-list { border: 1px solid #ddd; padding: 10px; margin-bottom: 10px; border-radius: 4px; overflow-y: auto; max-height: 150px; }";
  html += ".network-item { padding: 5px; cursor: pointer; border-bottom: 1px solid #eee; display: flex; align-items: center; justify-content: space-between;}";
  html += ".network-item:hover { background-color: #f0f0f0; }";
  html += ".signal-strength { font-size: 0.8em; color: #777; }";
  html += "hr { border: 1px solid #ddd; margin: 10px 0; }";
  html += "</style>";
  html += "</head><body>";
  html += "<div class='container'>";
  html += "<h1>WiFi Setup</h1>";
  html += "<form action='/connect' method='POST'>";

  int n = WiFi.scanNetworks();
  if (n == 0) {
    html += "<p>No networks found.</p>";
  } else {
    html += "<label for='ssid'>Available Networks: (refresh to update)</label>";
    html += "<div class='network-list'>";
    for (int i = 0; i < n; ++i) {
      int rssi = WiFi.RSSI(i);
      html += "<div class='network-item' onclick='selectNetwork(\"" + WiFi.SSID(i) + "\")'>";
      html += WiFi.SSID(i);
      html += "<span class='signal-strength'>\t\t(" + String(rssi) + " dBm)</span>";
      html += "</div>";
    }
    html += "</div>";
  }

  html += "<hr>";

  html += "<label for='ssid'>SSID:</label>";
  html += "<input type='text' name='ssid' id='ssid' value=''><br>";
  html += "<label for='password'>Password:</label>";
  html += "<input type='password' name='password' id='password' value=''><br>";
  html += "<div class='checkbox-container'>";
  html += "<input type='checkbox' id='showPassword' onclick='togglePasswordVisibility()'>";
  html += "<label for='showPassword'>Show Password</label>";
  html += "</div>";

  html += "<label for='deviceId'>Device ID:</label>";
  html += "<input type='text' name='deviceId' id='deviceId' value=''><br>";
  html += "<label for='secretCode'>Secret Code:</label>";
  html += "<input type='text' name='secretCode' id='secretCode' value=''><br>";
  html += "<input type='submit' value='Connect'><br>";
  html += "<div class='checkbox-container'>";
  html += "<input type='checkbox' id='autoConnect' name='autoConnect' value='1' checked>";
  html += "<label for='autoConnect'>Auto Connect on Reboot</label>";
  html += "</div>";
  html += "</form>";
  html += "</div>";
  html += "<script>";

  html += "function togglePasswordVisibility() {";
  html += " var passwordInput = document.getElementById('password');";
  html += " if (passwordInput.type === 'password') {";
  html += "  passwordInput.type = 'text';";
  html += " } else {";
  html += "  passwordInput.type = 'password';";
  html += " }";
  html += "}";

  html += "function selectNetwork(ssid) {";
  html += " document.getElementById('ssid').value = ssid;";
  html += "}";

  html += "</script>";

  html += "</body></html>";
  _server.send(200, "text/html", html);
}

void WiFiSetup::handleConnect() {
  String ssid = _server.arg("ssid");
  String password = _server.arg("password");
  String autoConnectStr = _server.arg("autoConnect");
  String deviceId = _server.arg("deviceId");
  String secretCode = _server.arg("secretCode");

  bool autoConnectEnabled = autoConnectStr == "1";

  writeEEPROM(ssid, SSID_ADDR);
  writeEEPROM(password, PASS_ADDR);
  writeEEPROM(deviceId, DEVICE_ID_ADDR);
  writeEEPROM(secretCode, SECRET_CODE_ADDR);

  EEPROM.write(AUTO_CONNECT_ADDR, autoConnectEnabled ? 1 : 0);
  EEPROM.commit();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  int attempts = 0;
  if(_showDebug){Serial.printf("WiFiSetup::Attempting to connect to %s ", ssid);}

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    if(_showDebug){Serial.print(".");}
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("\nWiFiSetup::WiFi connected!");

    if(_showDebug){
      Serial.print("WiFiSetup::IP address: \t");
      Serial.println(WiFi.localIP());
      Serial.print("WiFiSetup::SSID: \t");
      Serial.println(ssid);
      Serial.print("WiFiSetup::Device ID: \t");
      Serial.println(deviceId);
      Serial.print("WiFiSetup::Secret Code: ");
      Serial.println(secretCode);  
    }

    _setupInProgress = false;
    _server.close();

  } else {

    Serial.println("\nWiFiSetup::WiFi connection failed. Restarting AP.");
    startAP();

  }
}

void WiFiSetup::handleScan() {
  String networkListHtml = "<!DOCTYPE html><html><head><title>WiFi Networks</title></head><body>";
  int n = WiFi.scanNetworks();
  if (n == 0) {
    networkListHtml += "<p>No networks found.</p>";
  } else {
    for (int i = 0; i < n; ++i) {
      networkListHtml += "<p>" + WiFi.SSID(i) + " (" + String(WiFi.RSSI(i)) + " dBm)</p>";
    }
  }
  networkListHtml += "</body></html>";
  _server.send(200, "text/html", networkListHtml);
}

void WiFiSetup::handleCaptivePortal() {
  _server.send(204, "text/plain", "");
}

void WiFiSetup::handleRefresh() {
  _server.sendHeader("Location", "/", true);
  _server.send(302, "text/plain", "");
}

String WiFiSetup::getDeviceID() {
  char deviceId[64] = {0};
  for (int i = 0; i < 63; i++) {
    deviceId[i] = EEPROM.read(DEVICE_ID_ADDR + i);
    if (deviceId[i] == 0) {
      break;
    }
  }
  deviceId[63] = 0;
  return String(deviceId);
}

String WiFiSetup::getSecretCode() {
  char secretCode[64] = {0};
  for (int i = 0; i < 63; i++) {
    secretCode[i] = EEPROM.read(SECRET_CODE_ADDR + i);
    if (secretCode[i] == 0) {
      break;
    }
  }
  secretCode[63] = 0;
  return String(secretCode);
}

String WiFiSetup::getSSID() {
  char ssid[64] = {0};
  for (int i = 0; i < 63; i++) {
    ssid[i] = EEPROM.read(SSID_ADDR + i);
    if (ssid[i] == 0) {
      break;
    }
  }
  ssid[63] = 0;
  return String(ssid);
}

String WiFiSetup::getPassword() {
  char password[64] = {0};
  for (int i = 0; i < 63; i++) {
    password[i] = EEPROM.read(PASS_ADDR + i);
    if (password[i] == 0) {
      break;
    }
  }
  password[63] = 0;
  return String(password);
}

bool WiFiSetup::isSetupComplete() {
  return _setupInProgress;
}

void WiFiSetup::handleClient() {
  _server.handleClient();
}

void WiFiSetup::writeEEPROM(String txtField, int memAddress) {
  for (int i = 0; i < txtField.length(); i++) {
    EEPROM.write(memAddress + i, txtField[i]);
  }
  EEPROM.write(memAddress + txtField.length(), 0); // Null terminate
}
