# Enabling writing to flash on arduino at runtime.
The ATmega chip is a very cheap and powerful chip that is likely the most used hobbyist chip. One of the weaknesses of the chip is the lack of a bigger amount of non-volatile memory. There is however, a giant flash-memory for the program which you probably don't need all of(32kB-512bytes to be precise). The problem is only that it is not accessible. So the question is can we make it accessible?

The short answer is yes, yes we can. This guy [Majekw](https://github.com/majekw/optiboot/blob/master/optiboot/bootloaders/optiboot/optiboot.c) has already done it in a simple wrapper. The secret sauce is that you need to put the SPM instruction in the booloader section and call it from you application. If you do that you are good to go.

## How does the ATmega write to flash-memory?
The only instruction that can write to flash is the SPM instruction. This instruction utilize the SPMCSR(Store Program Memory Control and Status Register) for parameters and the Z register to address the flash-memory. Whenever you write something to SPMCSR you have 4 clockcycles to call the SPM function, unless you do that the register will clear itself. This instruction is not allowed to run except if it is in the bootloader section of the flash however, which is why we need to do some "hacking" to work around the problem.

## "Hacking" in the SPM instruction. 
Majekw has created a function called do_spm that resides in the bootloader. By making that function public, it can be called from the application. He has created a simple [example](https://github.com/majekw/optiboot/blob/master/optiboot/examples/test_dospm/test_dospm.ino) that can be used as well to understand it.

## How does arduino lock flash after boot?
The arduino does not lock the flash during boot, the application part of the flash is never locked in the current configuration. This can be changed by setting the BLB01 and BLB02 to 0. However, the operation that is used to write to flash is only enabled in the bootloader section of the program.
```
ATmega48A/PA/88A/PA/168A/PA/328/P support a real Read-While-Write Self-Programming mechanism. There is a separate Boot Loader Section, and the SPM instruction can only execute from there.
```