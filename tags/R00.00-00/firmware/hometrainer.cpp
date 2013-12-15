#include <Arduino.h>
//Modified LiquidCrystal.h
//Added "delayMicroseconds(2000);" as last instruction in  void LiquidCrystal::send(uint8_t value, uint8_t mode)

#include "Bounce.h"
#include <LiquidCrystal.h>
#include <avr/sleep.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"
#include "hometrainer.h"
#include "ttimer.h"

struct config_t
{
    unsigned long ulTotalTime;
} 
configuration;

const byte yPinTrigger=2;
const byte yPinButton=4;
const byte yPinClearMem=5;
const byte EXTINT1=1;
const word DISPLAY_ON_TIME=30000;

LiquidCrystal lcd(23, 25, 27, 29, 31, 33, 35);
Bounce boPinTrigger(yPinTrigger, 50);
Bounce boPinButton(yPinButton, 50);
Bounce boPinClearMem(yPinClearMem, 50);
TTimer ttClearRPM(5000);
TTimer ttClearDisplay(DISPLAY_ON_TIME);

void setup()
{
    Serial.begin(9600);
    pinMode(yPinTrigger, INPUT_PULLUP);
    pinMode(yPinButton, INPUT_PULLUP);
    pinMode(yPinClearMem, INPUT_PULLUP);
    if(!EEPROM_readAnything(0,configuration)){
        Serial.println("EEPROM data is corrupt, writing new data.");
        EEPROM_writeAnything(0,configuration);
    }
    Serial.print("Total time: ");
    Serial.println(configuration.ulTotalTime, DEC);
    wakeUpNow();
}

void loop() {
    static unsigned long ulTripTime_ms=0;
    boPinButton.update();
    boPinTrigger.update();
    boPinClearMem.update();

    if(boPinTrigger.fallingEdge()){
        word wCycleTime=getCycleTime();
        //Convert [ms/n] to [rpm], also taking into account the 16:39 transfer ratio of the gears.
        word wRpm=(wCycleTime>0?(60000<<4)/wCycleTime/39:0);
        ulTripTime_ms+=wCycleTime;
        showLcd(wRpm, ulTripTime_ms/1000);
        ttClearDisplay.start();
        ttClearRPM.start();
    }
    if(boPinButton.fallingEdge()){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Total: ");
        showFullTime(configuration.ulTotalTime+ulTripTime_ms/1000);
    }
    if(boPinClearMem.fallingEdge()){
        Serial.println("Clearing EEPROM");
        configuration.ulTotalTime=0;
        EEPROM_writeAnything(0,configuration);
    }
    if(ttClearRPM.runout()){
        showLcd(0, ulTripTime_ms/1000);
        ttClearRPM.start();//to avoid the showLCD being executed every loop after the timeout.
    }
    if(ttClearDisplay.runout()){
        ttClearDisplay.start();
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Tot ziens...");
        delay(1000);
        lcd.noDisplay();
        configuration.ulTotalTime+=ulTripTime_ms/1000;
        EEPROM_writeAnything(0,configuration);
        //variables will be restored after sleep.
        //for the trip time, this is undesired, because the trip is finished when sleeping.
        ulTripTime_ms=0;
        sleepNow();
    }
}

void showLcd(word wRpm, unsigned long ulTotalSeconds)
{  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(wRpm);
    lcd.setCursor(3,0);
    lcd.print("rpm");
    lcd.setCursor(7,0);
    lcd.print("Tijd:");
    showFullTime(ulTotalSeconds);
}

void showFullTime(unsigned long ulSeconds){
    unsigned long ulTotalHours=ulSeconds/3600;
    ulSeconds%=3600;
    if(ulTotalHours>0){
        lcd.print(ulTotalHours);
        lcd.print("h");
    }
    byte yMinutes=ulSeconds/60;
    ulSeconds%=60;
    if(yMinutes>0){
        lcd.print(yMinutes);
        lcd.print("m");
    }
    lcd.print(ulSeconds);
    lcd.print("s");
}


//Calculates the duration since last call of this function.
//Then converts that time difference to a frequency in [rpm]
//Then takes into account the 39/16-proportion of the cogwheels in the hometrainer.
//When it has been more than 5s since the last function call , then 0 will be returned.  This is too ignore too slow (<5RPM) cycling.
unsigned long getCycleTime(){
    static unsigned long ulPreviousTime=0;
    unsigned long result=millis()-ulPreviousTime;
    ulPreviousTime=millis();
    return (result>5000?0:result);
}

void sleepNow()
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    attachInterrupt(EXTINT1, wakeUpNow, LOW);
    sleep_mode();
    /* wake up here */
    sleep_disable();
    detachInterrupt(EXTINT1);
}

void wakeUpNow()        // here the interrupt is handled after wakeup
{
    // execute code here after wake-up before returning to the loop() function
    // timers and code using timers (serial.print and more...) will not work here.
    // we don't really need to execute any special functions here, since we
    // just want the thing to wake up
    lcd.begin(20, 2);
    lcd.clear();                  // start with a blank screen
    lcd.setCursor(0,0);           // set cursor to column 0, row 0 (the first row)
    lcd.print("Hallo Frans!");    // change this text to whatever you like. keep it clean.
}







