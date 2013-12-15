// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef hometrainer_H_
#define hometrainer_H_

#include "Arduino.h"

//add your includes for the project here


//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void showLcd(word wRpm, unsigned long ulTotalSeconds);
unsigned long getCycleTime();
boolean debounceInput(byte yPinNr, byte yDesiredPinState);
void triggerDetect();
void sleepNow();
void wakeUpNow();
void showFullTime(unsigned long ulSeconds);
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project here




//Do not add code below this line
#endif /* hometrainer_H_ */

