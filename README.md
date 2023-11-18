# arduino_hacking
The purpose of this repository is to summarize all the stuff I do with Arduino to not be completely nullified every time I take up a half-done project again 5 months later.
## Projects
### [Enabling writing to flash on arduino at runtime](writing_to_flash/README.md)
The ATmega chip is a very cheap and powerful chip that is likely the most used hobbyist chip. One of the weaknesses of the chip is the lack of a bigger amount of non-volatile memory. There is however, a giant flash-memory for the program which you probably don't need all of(32kB-512bytes to be precise). The problem is only that it is not accessible.
### [Creating custom bootloader](custom_bootloader/README.md)
To be able to write to flash I need to modify the bootloader, I found a solved solution but the knowledge is still nice to have.
### [Debugging the atmega](arduino_debugging/README.md)
So basically I found this cool [project](https://sites.google.com/site/wayneholder/avrarduino-hardware-debugger-on-the-cheap) that is utilizing the proprietary protocol debugwire that Atmel has created(by first reverse engineering it). 

I then stumbled upon something I wanted to find before it. [Simavr](https://github.com/buserror/simavr/tree/master) is a simulator that can be used to debug AVR code on ubuntu. [This guide](https://aykevl.nl/2020/06/simavr-debug/) describes some kind of MVP to get started.

## Potential projects
### Building GDB-server for debugwire
Should be possible, protocol can be found open on internet. 
Multiple guides exist that cover close to topics:
* [Implement GDB remote debug protocol from scrats](https://medium.com/@tatsuo.nomura/implement-gdb-remote-debug-protocol-stub-from-scratch-1-a6ab2015bfc5)
* [GDB debug wire integrated server](https://github.com/tetofonta/gdb-debug-wire-integrated-server)

### Building an OS on atmega


## Notes found during research
Atmega168 is basically the same as ATMega328, only difference is memory(I think).
[Atmega168 bootloader source](https://github.com/arduino/ArduinoCore-avr/blob/master/bootloaders/atmega/ATmegaBOOT_168.c)

### Arduino as ISP
I should be able to use [Arduino as ISP](https://docs.arduino.cc/built-in-examples/arduino-isp/ArduinoISP) for burning the bootloader in a similar way to AVR-ISP, however it is not suggested as a possible solution on the [Hacking bootloader page](https://docs.arduino.cc/hacking/software/Bootloader) which is a bit concerning. Will need to verify in some way.

### Burning bootloader for attiny85




## Writing my own version and making it work on the arduino
[x] Create functioning optiboot image in repo.
[x] Create functioning and shortened Makefile in repo
[ ] Create custom SPM function in bootloader
[ ] Change bootsize to fit new bootloader
[ ] Create .ino file that tests and verifies that I can modify the flash during execution.

## Other questions

### How does arduino know that flash should be reprogrammed?


