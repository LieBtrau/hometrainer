#include "lcdcontrol.h"

lcdControl::lcdControl(byte yPinEnPower):_lcd(5, 6, 7, 8, 10, 9, 11)
{
    _yPinEnPower=yPinEnPower;
    _bTripNameNeedsUpdating=true;
    pinMode(_yPinEnPower, OUTPUT);
}

void lcdControl::init(){
    digitalWrite(_yPinEnPower,HIGH);
    delay(500);
    _lcd.initialize();
    _lcd.begin(20, 2);
    showTripData();
    showTripData(0,0);
}

void lcdControl::showTripData(word wRpm, unsigned long ulSeconds){
    char buffer[4];
    if(_bTripNameNeedsUpdating){
        showTripData();
    }
    _lcd.setCursor(0,0);
    sprintf(buffer,"%3d",wRpm);
    _lcd.print(buffer);
    _lcd.setCursor(12,0);
    showTime(ulSeconds);
}

void lcdControl::off(){
    _lcd.off();
    digitalWrite(_yPinEnPower,LOW);
    _bTripNameNeedsUpdating=true;
}

void lcdControl::goodbye(){
    _lcd.clear();
    _lcd.setCursor(0,0);
    _lcd.print("Tot ziens Frans...");
    _bTripNameNeedsUpdating=true;
}

void lcdControl::showTotalTime(unsigned long ulSeconds){
    _lcd.clear();
    _lcd.setCursor(0,0);
    _lcd.print("Total: ");
    showTime(ulSeconds);
    _bTripNameNeedsUpdating=true;
}

void lcdControl::showTripData(){
    _lcd.clear();
    _lcd.setCursor(3,0);
    _lcd.print("rpm");
    _lcd.setCursor(7,0);
    _lcd.print("Tijd:");
    _bTripNameNeedsUpdating=false;
}

void lcdControl::showTime(unsigned long ulSeconds){
    unsigned long ulTotalHours=ulSeconds/3600;
    ulSeconds%=3600;
    if(ulTotalHours>0){
        _lcd.print(ulTotalHours);
        _lcd.print("h");
    }
    byte yMinutes=ulSeconds/60;
    ulSeconds%=60;
    if(yMinutes>0){
        _lcd.print(yMinutes);
        _lcd.print("m");
    }
    _lcd.print(ulSeconds);
    _lcd.print("s");
}


