/*
* Set up a WiFi internet connection without hard-coding the networks name and password.

* WiFiSetup wifiSetup("AtomS3-Setup", "password123");
*   When setting an object to the library include an AP name and password 
* WiFiSetup.begin() takes 3 parameters overrideAutoConnect = false, bool forceAutoConnect = false, int debugLevel = 0
*   If forceAutoConnect is true, then the device will auto connect, regardless of the EEPROM setting.
*   If overrideAutoConnect is true, then the device will start the AP, regardless of the EEPROM setting.
*   If neither are true, then the device will follow the EEPROM settings.
*   debugLevel. 0=No messages. 1=High Level, 2=All 
*
* There are 2 user-entered parameters that can also be entered.
* They are useful for Arduino IOT devices which need a DeviceId and SecretCode
*
* Open a webpage at 192.168.4.1 and select one of the networks listed
* Enter the credentials and click Connect
*
* The library will attempt to connect to the network
*
*/

#ifndef WiFiSetupLibrary_h
#define WiFiSetupLibrary_h

#include "Arduino.h"
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

class WiFiSetup {
public:
  WiFiSetup(const char* apSSID, const char* apPassword);
  //void begin(bool overrideAutoConnect = false, bool forceAutoConnect = false, bool showDebug = true);
  void begin();               // with parameters now moved to functions
  String getDeviceID();       // return user entered Device ID string
  String getSecretCode();     // return user entered Secret Code string
  String getSSID();           // return the selected network name
  String getPassword();       // return the network's password
  void handleClient();        // connect to web and display networks and handle feedback
  bool isSetupComplete();     // used by ino to see if still WiFiSetup is complete

  // New setter functions:
  void setOverrideAutoConnect(bool overrideAutoConnect);
  void setForceAutoConnect(bool forceAutoConnect);
  void setShowDebug(bool showDebug);

private:
  const char* _apSSID;        // Network name used internally within the library
  const char* _apPassword;    // Password used internally
  WebServer _server;          // Server object
  bool _apMode;               // what mode the Access Point is in
  bool _setupInProgress;      // true when completed
  //bool _showDebug;            // true to show progress messages on serial monitor
  
  bool _overrideAutoConnect = false; // Default values
  bool _forceAutoConnect = false;
  bool _showDebug = true;

  void startAP();             // internal library function to start the devices Access Point
  void autoConnect();         // automatically connect without starting AP
  void handleRoot();          // display network list and obtain credentials
  void handleConnect();       // attempt to connect to the network
  void handleRefresh();       // if user refreshes the web page
  bool isSSIDEmpty();         // used to check existence of a network ssid name in EEPROM
  void writeEEPROM(String txt, int memAddress); // internal function to write the the EEPROM
  void handleScan();          // scan for networks
  void handleCaptivePortal(); // captive portal(not used)
};

#endif