# WiFiSetup
Connect to any network. No need to hard code credentials. 
I have found  that the great WiFiManager library can sometimes be too big for smaller memory devices
Therefore, with the help of Gemini, I wrote this library.

ESP32 devices with WiFi capabilities.

The library requires...

  WiFi.h
  WebServer.h
  EEPROM.h

Code allows for...

Arduino Thing credential inputs.
Forcing Override autoconnect, Force AutoConnect, Degug Messages via the .begin() function.
Code writer can choose Acces Point Name and Password

The library allows calls for obtaining...

SSID name      getSSID()
Password       getPassword()
Device ID      getDeviceID()
Secret Code    getSecretCode()

Public functions

handleClient() - call to obtain available networks and enter credentials. Data save in device EEPROM
isSetupComplete() - a boolean check to see if user has completed the setup

Contact: Email: sgfpcb@gmail.com
