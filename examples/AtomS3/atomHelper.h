#include "esp32-hal-gpio.h"
#pragma once

/*********************************************************************************
* Local library for Atom control and management
*
* atom::setup()               initialise the M5 Atom S3
* atom::connecting()          display connecting message
* atom::connected()           display the connected message
* atom::screenOn()            turn on display
* atom::screenOff()           turn off display
* atom::brightness()          set the screen's brightness
*
*********************************************************************************/

// predeclare functions
void clearScreen();

int width = 0;                           // initialise the width integer
int height = 0;                          // initialise the height integer
#define backLight 16                     // backlight pin

namespace atom{

  void setup(){
    pinMode(backLight, INPUT);           // backlight input pin

    auto cfg = M5.config();
    AtomS3.begin(cfg);
    width = AtomS3.Display.width();          // get the screen's width
    height = AtomS3.Display.height();        // get the screen's height
  }

  void clearScreen(){
    AtomS3.Display.fillScreen(BLACK);
  }

  void connecting(){
    atom::clearScreen();
    AtomS3.Display.setTextColor(CYAN);
    AtomS3.Display.drawString("Connecting..", width, height, 2);
  }

  void connected(String ssidName){
    atom::clearScreen();
    AtomS3.Display.setTextDatum(middle_center);
    AtomS3.Display.fillCircle(AtomS3.Display.width() / 2, AtomS3.Display.height() / 2, 40, BLUE);
    AtomS3.Display.drawString(ssidName, width / 2, height / 2);
  }

  void screenOn(){
    digitalWrite(backLight, HIGH);                    // turn on backlight
  }

  void screenOff(){
    digitalWrite(backLight, LOW);                    // turn off backlight   
  }

  void screenBrightness(int value){
    value = map(value, 0, 100, 0, 255);               // change the range to match the display brightness value parameter
    AtomS3.Display.setBrightness(value);              // set the screen brightness to the mapped value   
  }

}