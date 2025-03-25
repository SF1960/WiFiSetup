/*
* This is an example of how to use my WiFiSetUpLibrary.h
*
* THIS SKETCH IS LOCKED - PLEASE MAKE A COPY TO INCLUDE/ENHANCE YOUR SKETCH TO INCLUDE CONNECTION TO ARDUINO
*
* The object takes the name of the Access Point network name you want displayed in the network list
* The password required the first time you access that AP name
*
* You need to include the library WiFiSetupLibrary.h 
* Set an object to the class WiFiSetup with a name for the AP and its password
* and set a boolean called wifiConnected to false
*
* place wifiSetup.begin(); in the setup
*
* begin() takes 3 parameters overrideAutoConnect = false, bool forceAutoConnect = false, bool showDebug = true)
*   If forceAutoConnect is true, then the device will auto connect, regardless of the EEPROM setting.
*   If overrideAutoConnect is true, then the device will start the AP, regardless of the EEPROM setting.
*   If neither are true, then the device will follow the EEPROM settings.
*   debugLevel. 0=No messages. 1=High Level, 2=All 
*
* wifiSetup.handleClient();
*   place in your loop to call the webpage network selection page
*
* Versions
* Original: Used WifiSetup.begin() to set 3 parameters
* 2025-03-24: Changed WiFiSetupLibrary to move these parameters to their own functions
*
*/

#include <WiFiSetupLibrary.h>
WiFiSetup wifiSetup("AtomS3-Setup", "password123");

String ssid;
String password;
String deviceId;
String secretCode;

void setup() {

  Serial.begin(115200);                           // Initialize Serial 
  delay(1500);                                    // wait to allow the monitor to start

  // Set the desired options:
  wifiSetup.setOverrideAutoConnect(true);         // default false
  wifiSetup.setForceAutoConnect(false);           // default false
  wifiSetup.setShowDebug(true);                   // defualt true

  // initalise wifi setup (These are now their own functions. overrideAutoconnect, forceAutoconnect, debugLevel)
  wifiSetup.begin();                 

}

void loop() {

  wifiSetup.handleClient();                         // Handle WebServer requests

  // check connection status
  if (WiFi.status() == WL_CONNECTED){

    // place your code that needs wifi connection here

    // these lines are here to show how to use the library's functions
    // obtain credentials from AP and display on monitor
    ssid = wifiSetup.getSSID();
    password = wifiSetup.getPassword();  
    deviceId = wifiSetup.getDeviceID();
    secretCode = wifiSetup.getSecretCode();

    // print results of calls to the library
    // using .c_str() to handle any special characters that are entered
    Serial.printf("\nWiFi Credentials Received!\n");
    Serial.printf("SSID: \t\t%s\n", ssid);
    Serial.printf("Password: \t%s\n", password.c_str());
    Serial.printf("Device ID: \t%s\n", deviceId.c_str());
    Serial.printf("Secret Code: \t%s\n", secretCode.c_str());

  } else {

    // place your code here to handle no connection
    Serial.printf("Device not yet connected\n");
    delay(5000);

  }
 
  delay(10000);
    
}



