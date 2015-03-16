### Version 0 ###
> This version was the first trial version.  It has some problems:
  * [PDF](https://drive.google.com/file/d/0B5_mAlpV8IjvWEx4NjdpbnpqXzg/edit?usp=sharing)
  * It works only for supply voltages of 3.3V and higher.  For lower supply voltages, the GPIO voltage of the Arduino (V\_ol) is smaller than the input voltage for a high level (V\_ih) on the LCD.  A voltage level translation circuit consisting of a pull-up and an NMOS solves the problem.
  * SMD components were placed on top side, through hole components on bottom side so that routing could be done on top layer, whilst keeping a full ground plane on top side.  The problem here was that some SMD components were too high and touch the LCD module when the board is mounted in the chassis.

### Version 1 ###
> This version solves the issues of the version 0 PCB.

  * It's not code-compatible.  Two GPIO lines of the MCU have been switched because it was easier to rout the tracks on the PCB.
  * There's an extra ISP connector on the board because the Arduino Mini can no longer be programmed using its bootloader.  Some fuse settings also need to be changed and that can't be done with the bootloader.
  * [PDF](https://drive.google.com/file/d/0B5_mAlpV8IjvdmRJRzhJOWV1aTg/edit?usp=sharing)
  * Unfortunately, this version also has a hardware error.  When the wheel-sensor-switch remains closed after the user stops cycling, the MCU will never go to sleep and hence the batteries will run out soon.  So the level-triggering of the input has been converted to an edge-triggered input.  The patch can easily be done on Version 1.  I built it in like this.

### Version 2 ###
> This version solves the issues of the version 1 PCB.

  * [PDF of Bill of Materials](https://drive.google.com/file/d/0B5_mAlpV8IjvM3hTN3dHTHdEWkU/edit?usp=sharing)
  * [PDF of design data](https://drive.google.com/file/d/0B5_mAlpV8IjvWEUzSWY2bFpsWFU/edit?usp=sharing)