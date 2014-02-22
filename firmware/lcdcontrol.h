#ifndef LCDCONTROL_H
#define LCDCONTROL_H

#include "./LiquidCrystal.h"
#include "Arduino.h"

class lcdControl
{
public:
    lcdControl(byte yPinEnPower);
    void showTripData(word wRpm, unsigned long ulSeconds);
    void showTotalTime(unsigned long ulSeconds);
    void init();
    void off();
    void goodbye();
private:
    void showTime(unsigned long ulSeconds);
    void showTripData();
    byte _yPinEnPower;
    bool _bTripNameNeedsUpdating;
    LiquidCrystal _lcd;
};

#endif // LCDCONTROL_H
