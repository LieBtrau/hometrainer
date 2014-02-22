#include <Arduino.h>

#include "Bounce.h"
#include <avr/sleep.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"
#include "hometrainer.h"
#include "ttimer.h"
#include "lcdcontrol.h"
#include "stopwatch.h"

#define sleep_bod_disable() \
{ \
uint8_t tempreg; \
__asm__ __volatile__("in %[tempreg], %[mcucr]" "\n\t" \
"ori %[tempreg], %[bods_bodse]" "\n\t" \
"out %[mcucr], %[tempreg]" "\n\t" \
"andi %[tempreg], %[not_bodse]" "\n\t" \
"out %[mcucr], %[tempreg]" \
: [tempreg] "=&d" (tempreg) \
: [mcucr] "I" _SFR_IO_ADDR(MCUCR), \
[bods_bodse] "i" (_BV(BODS) | _BV(BODSE)), \
[not_bodse] "i" (~_BV(BODSE))); \
};

struct config_t
{
    unsigned long ulTotalTime;
} 
configuration;

const byte yPinTrigger=2;
const byte yPinClearMem=12;
const byte yPinButton=13;
const byte EXTINT1=1;   //IO3 of Arduino Mini connected to EXTINT1
const word DISPLAY_ON_TIME=30000;

lcdControl lcdc(4);
Bounce boPinTrigger(yPinTrigger, 50);
Bounce boPinButton(yPinButton, 50);
Bounce boPinClearMem(yPinClearMem, 50);
TTimer ttClearRPM(5000);
TTimer ttClearDisplay(DISPLAY_ON_TIME);
StopWatch chrono(StopWatch::SECONDS);

void setup()
{
    pinMode(yPinTrigger, INPUT_PULLUP);
    pinMode(yPinButton, INPUT_PULLUP);
    pinMode(yPinClearMem, INPUT_PULLUP);
    if(!EEPROM_readAnything(0,configuration)){
        EEPROM_writeAnything(0,configuration);
    }
    //set low power
    bitClear(SPCR,SPE);     //disable SPI
    bitClear(TWCR,TWEN);    //disable TWI
    bitSet(ACSR,ACD);       //power off analog comparator
    bitClear(ADCSRA,ADEN);  //disable ADC
    bitSet(PRR,PRTWI);      //disable clock to TWI
    bitSet(PRR,PRTIM2);     //disable clock to timer2
    bitSet(PRR,PRTIM1);     //disable clock to timer1
    bitSet(PRR,PRSPI);      //disable clock to SPI
    bitSet(PRR,PRUSART0);   //disable USART0
    bitSet(PRR,PRADC);      //shutdown ADC
    delay(1000);
    wakeUpNow();
}

void loop() {
    boPinButton.update();
    boPinTrigger.update();
    boPinClearMem.update();
    //someone is cycling
    if(boPinTrigger.fallingEdge()){
        if(!chrono.isRunning()){
            chrono.start();
        }
        word wCycleTime=getCycleTime();
        //Convert [ms/n] to [rpm], also taking into account the 16:39 transfer ratio of the gears.
        word wRpm=(wCycleTime>0?(60000<<4)/wCycleTime/39:0);
        lcdc.showTripData(wRpm, chrono.elapsed());
        ttClearDisplay.start();
        ttClearRPM.start();
    }
    //someone wants to check the total time of all trips, including this one
    if(boPinButton.fallingEdge()){
        lcdc.showTotalTime(configuration.ulTotalTime+chrono.elapsed());
    }
    //someone wants to clear the data stored in the eeprom
    if(boPinClearMem.fallingEdge()){
        configuration.ulTotalTime=0;
        EEPROM_writeAnything(0,configuration);
    }
    //someone is taking a rest while cycling
    if(ttClearRPM.runout()){
        lcdc.showTripData(0, chrono.elapsed());
        chrono.stop();
        ttClearRPM.start();//to avoid the showLCD being executed every loop after the timeout.
    }
    //someone has stopped cycling.  Go to sleep
    if(ttClearDisplay.runout()){
        ttClearDisplay.start();
        lcdc.goodbye();
        delay(1000);
        configuration.ulTotalTime+=chrono.elapsed();
        EEPROM_writeAnything(0,configuration);
        sleepNow();
    }
}

unsigned long getCycleTime(){
    static unsigned long ulPreviousTime=0;
    unsigned long result=millis()-ulPreviousTime;
    ulPreviousTime=millis();
    return (result>5000?0:result);
}

void sleepNow()
{
    lcdc.off();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    attachInterrupt(EXTINT1, wakeUpNow, LOW);
    cli();
    sleep_enable();
    sleep_bod_disable();
    sei();
    sleep_cpu();
    sleep_disable();
    sei();
}

void wakeUpNow()        // here the interrupt is handled after wakeup
{
    // execute code here after wake-up before returning to the loop() function
    // timers and code using timers (serial.print and more...) will not work here.
    // we don't really need to execute any special functions here, since we
    // just want the thing to wake up
    detachInterrupt(EXTINT1);
    lcdc.init();
    chrono.reset();
}







