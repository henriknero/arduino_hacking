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

* How does arduino write to flash during boot?
  - The Z-pointer(Reg-31, Reg-30) is used to address the flash when doing SPM(Store Program Memory) commands. 
* How does arduino lock flash after boot?
* How does arduino know that flash should be reprogrammed?
* How to burn bootloader using avrdude?
* How to compile bootloader?
  - There exists Makefiles which can be run to create .hex. This can be found in source-code.
* How do I inspect the compiled bootloader?
  - .HEX only contains the bare minimum, meaning it has stripped of all unnecessary stuff. Which is why its very small.
  - When compiling using Arduino IDE you get a version that contains both the binary, the hex representation, the binary+bootloader, and the same in hex. By using the following command you can disassemble the .hex file.
  - `avr-objdump -m avr:5 -D <hexfile>`
  - The bootloader resides at 0x7e00- and the application code resides at 0x0000.

## Debugging atmega328

## Building an OS on atmega
