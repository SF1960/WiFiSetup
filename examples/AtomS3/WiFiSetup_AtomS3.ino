/*
This is an example of how to use my WiFiSetUpLibrary.h

THIS SKETCH IS LOCKED - PLEASE MAKE A COPY TO INCLUDE/ENHANCE YOUR SKETCH TO INCLUDE CONNECTION TO ARDUINO

The object takes the name of the Access Point network name you want displayed in the network list
the password required the first time you access that AP name

You need to include the library WiFiSetupLibrary.h 
Set an object to the class WiFiSetup with a name for the AP and its password
and set a boolean called wifiConnected to false

Before running. begin() set your connect and debug requirements place these calls in the setup() routine

void setOverrideAutoConnect(bool overrideAutoConnect);
void setForceAutoConnect(bool forceAutoConnect);
void setShowDebug(bool showDebug);

*   If forceAutoConnect is true, then the device will auto connect, regardless of the EEPROM setting.
*   If overrideAutoConnect is true, then the device will start the AP, regardless of the EEPROM setting.
*   If neither are true, then the device will follow the EEPROM settings.
*   showDebug. 0=No messages. 1=High Level

place wifiSetup.begin(); in the setup

wifiSetup.handleClient();
*   place in your loop to call the webpage network selection page

Versions
* Original: Used WifiSetup.begin() to set 3 parameters
* 2025-03-24: Changed WiFiSetupLibrary to move these parameters to their own functions
* 2025-03-26: Changed to display on Atom S3

*/

// Arduino Libraries
#include <M5AtomS3.h>
#include <WiFiSetupLibrary.h>
WiFiSetup wifiSetup("AtomS3-Setup", "password123");

// Local libraries
#include "atomHelper.h"

// Initialise variables
String ssid;
String password;
String deviceId;
String secretCode;

void setup() {

  Serial.begin(115200);                           // Initialize Serial 
  delay(1500);                                    // wait to allow the monitor to start

  atom::setup();                                  // Initialise the Atom S3
  atom::screenOn();                               // Ensure the screen is on
  atom::screenBrightness(100);                    // an integer between 0 and 100 (converted in the function to 0 - 255)
  atom::connecting();                             // Display connecting message

  // Set the desired options: uncomment to change: Default values are accepted if not called
  // wifiSetup.setOverrideAutoConnect(false);        // default false
  // wifiSetup.setForceAutoConnect(false);           // default false
  // wifiSetup.setShowDebug(true);                   // default true

  wifiSetup.begin();                               // initalise the library

  Serial.printf("Setup Complete\n");              

}

void loop() {

  wifiSetup.handleClient();                           // Handle WebServer and netrwork connection requests

  if (WiFi.status() == WL_CONNECTED){                 // check connection status

    /*
    place your code that needs wifi connection here

    These lines are here to show how to use the library's functions
    Obtain credentials from AP and display on monitor
    */

    ssid = wifiSetup.getSSID();
    password = wifiSetup.getPassword();  
    deviceId = wifiSetup.getDeviceID();
    secretCode = wifiSetup.getSecretCode();

    atom::connected(ssid);

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

  } /* (WiFi.status() == WL_CONNECTED) */
 
  delay(10000);
    
} /* loop */



