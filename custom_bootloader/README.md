# Custom bootloader

## How to burn bootloader using avrdude?
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

The second step is to flash the memory with the optiboot bootloader and finishes by setting the Lockbits to 0x0F(0b00001111) which locks the SPM and LPM operations for the bootloader section.

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


## How to compile bootloader?
The following code is getting run when running the command `make atmega328` in the optiboot bootloader folder:
```
avr-gcc -g -Wall -Os -fno-inline-small-functions -fno-split-wide-types -mshort-calls -mmcu=atmega328p -DF_CPU=16000000L  '-DLED_START_FLASHES=3' '-DBAUD_RATE=115200'   -c -o optiboot.o optiboot.c
avr-gcc -g -Wall -Os -fno-inline-small-functions -fno-split-wide-types -mshort-calls -mmcu=atmega328p -DF_CPU=16000000L  '-DLED_START_FLASHES=3' '-DBAUD_RATE=115200' -Wl,--section-start=.text=0x7e00 -Wl,--section-start=.version=0x7ffe -Wl,--relax -Wl,--gc-sections -nostartfiles -nostdlib -o optiboot_atmega328.elf optiboot.o 
avr-size optiboot_atmega328.elf
   text    data     bss     dec     hex filename
    532       0       0     532     214 optiboot_atmega328.elf
avr-objcopy -j .text -j .data -j .version --set-section-flags .version=alloc,load -O ihex optiboot_atmega328.elf optiboot_atmega328.hex
avr-objdump -h -S optiboot_atmega328.elf > optiboot_atmega328.lst
rm optiboot.o optiboot_atmega328.elf
```
and the below image is the "important" part of the Makefile for making sense of it.
The first 5 lines are variable definitions, and the last 2 lines are two "function" calls to *.hex and *.lst that we see in the next image.
![Alt text](<images/Screenshot from 2023-07-28 17-05-10.png>)
...  
Text omitted  
...
![Alt text](<images/Screenshot from 2023-07-28 17-05-27.png>)
Since the .hex has dependencies on the .elf that gets invoked. So the first thing we see in the code example is two calls to avr-gcc for creating that followed by the avr-size command, all three first commands are a part of the .elf "function"(Looks a bit confusing because 1 line leads to 2 gcc calls, but I think that is some kind of optimization from avr-gcc). 

After the .elf file is compiled the .hex gets made using avr-objcopy.
The .lst file follows the same pattern.

Most likely a similar story happens when being run for the original bootloader. The only difference should be that the bootloader here is quite a bit smaller and the bootloader therefore does not take up as much space. I have not checked but if I remember correctly the original arduino bootloader is starting at 0x7800 which would mean that the original bootloader is 2048bytes rather than 512bytes(The documentation is a bit confusing because they are counting in words in stead of bytes but I think they compensate by instead using 0x4000 rather than 0x8000. It does kind of make sense since each instruction is 1word(2bytes)). 


  - There exists Makefiles which can be run to create .hex. This can be found in source-code. [Atmega168 bootloader source](https://github.com/arduino/ArduinoCore-avr/blob/master/bootloaders/atmega/ATmegaBOOT_168.c).
* How do I inspect the compiled bootloader?
  - .HEX only contains the bare minimum, meaning it has stripped of all unnecessary stuff. Which is why its very small.
  - When compiling using Arduino IDE you get a version that contains both the binary, the hex representation, the binary+bootloader, and the same in hex. By using the following command you can disassemble the .hex file.
  - `avr-objdump -m avr:5 -D <hexfile>`
  - The bootloader resides at 0x7e00- and the application code resides at 0x0000.