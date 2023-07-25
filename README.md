# arduino_hacking
## Enabling writing to flash on arduino at runtime.
This is not intended, but I want to learn how to do it.
1. Figure out how to address flash memory at runtime.
2. Figure out if possible to write to flash at runtime.
3. Since I know it is not possible to write at runtime, figure out how to rewrite the firmware to be able to write code there.
4. Rewrite the firmware
5. Profit

## Researching stuff
Atmega168 is basically the same as ATMega328, only difference is memory(I think).
[Atmega168 bootloader source](https://github.com/arduino/ArduinoCore-avr/blob/master/bootloaders/atmega/ATmegaBOOT_168.c)

I should be able to use [Arduino as ISP](https://docs.arduino.cc/built-in-examples/arduino-isp/ArduinoISP) for burning the bootloader in a similar way to AVR-ISP, however it is not suggested as a possible solution on the [Hacking bootloader page](https://docs.arduino.cc/hacking/software/Bootloader) which is a bit concerning. Will need to verify in some way.

### How to write to flash (after I figured it out)
1. Create a custom bootloader that contains a function to write to the flash. (You need to do this since the SPM operation that is the only way to write to flash is only allowed to be run from the bootloader section)
2. Call that function from the application code.

Here is a dude that has done it: https://github.com/majekw/optiboot/blob/master/optiboot/bootloaders/optiboot/optiboot.c

### How does arduino lock flash after boot?
The arduino does not lock the flash during boot, the application part of the flash is never locked in the current configuration. This can be changed by setting the BLB01 and BLB02 to 0. However, the operation that is used to write to flash is only enabled in the bootloader section of the program.
```
ATmega48A/PA/88A/PA/168A/PA/328/P support a real Read-While-Write Self-Programming mechanism. There is a separate Boot Loader Section, and the SPM instruction can only execute from there.
```
### How does arduino know that flash should be reprogrammed?

### How to burn bootloader using avrdude?
The following command is used by the arduino IDE: 
```
"/home/john/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/bin/avrdude" "-C/home/john/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/etc/avrdude.conf" -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -e -Ulock:w:0x3F:m -Uefuse:w:0xFD:m -Uhfuse:w:0xDE:m -Ulfuse:w:0xFF:m
"/home/john/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/bin/avrdude" "-C/home/john/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/etc/avrdude.conf" -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 "-Uflash:w:/home/john/.arduino15/packages/arduino/hardware/avr/1.8.6/bootloaders/optiboot/optiboot_atmega328.hex:i" -Ulock:w:0x0F:m
```
Step one is to set the fuses to the correct values. The most important one here is the Lock bits, since this is what allows us to burn a new bootloader.

<img src="images/Screenshot from 2023-07-25 17-28-50.png" alt="drawing" width="700"/>

0x3F(0b00111111) meaning all BLBs and LBs are in unprogrammed mode giving access to both the bootloader part and the application part of the flash.

<img src="images/Screenshot from 2023-07-25 17-30-20.png" alt="drawing" width="600"/>
<img src="images/Screenshot from 2023-07-25 17-29-47.png" alt="drawing" width="600"/>

The second step is to flashes the memory with the optiboot bootloader and finishes by setting the Lockbits to 0x0F(0b00001111) which locks the SPM and LPM operations for the bootloader section.

The following command is used when only uploading new code without burning:
```
"/home/john/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/bin/avrdude" "-C/home/john/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/etc/avrdude.conf" -v -V -patmega328p -carduino "-P/dev/ttyACM0" -b115200 -D "-Uflash:w:/tmp/arduino/sketches/8C287CA5DFAAAC53D74333981D095D93/Blink.ino.hex:i"
```
+ -carduino specifies which programmer to use
+ -patmega328p specifies which platform/device we are uploading to
+ -P/dev/ttyACM0 specifies which Port
+ -b115200 specifies the baudrate to send at
+ -D disables autoerase for some reason??
+ "-Uflash:w:/tmp/arduino/sketches/8C287CA5DFAAAC53D74333981D095D93/Blink.ino.hex:i" specifies to perform a memory operation. Each parameter is separated by a colon. Variable 1 specifies where we want to write. Variable 2 specifies what operation ("r"ead, "w"rite, "v"erify). Variable 3 specifies where to read from or write to. Variable 4 specifies which format we are working with. E.g. Intel hex for i


### How to compile bootloader?
  - There exists Makefiles which can be run to create .hex. This can be found in source-code. [Atmega168 bootloader source](https://github.com/arduino/ArduinoCore-avr/blob/master/bootloaders/atmega/ATmegaBOOT_168.c).
* How do I inspect the compiled bootloader?
  - .HEX only contains the bare minimum, meaning it has stripped of all unnecessary stuff. Which is why its very small.
  - When compiling using Arduino IDE you get a version that contains both the binary, the hex representation, the binary+bootloader, and the same in hex. By using the following command you can disassemble the .hex file.
  - `avr-objdump -m avr:5 -D <hexfile>`
  - The bootloader resides at 0x7e00- and the application code resides at 0x0000.

## Debugging atmega328

## Building an OS on atmega
