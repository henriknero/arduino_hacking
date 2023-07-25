/* some includes */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

/* the current avr-libc eeprom functions do not support the ATmega168 */
/* own eeprom write/read functions are used instead */
#if !defined(__AVR_ATmega168__) || !defined(__AVR_ATmega328P__) || !defined(__AVR_ATmega328__)
#include <avr/eeprom.h>
#endif

char getch(void);

/* some variables */
union address_union {
	uint16_t word;
	uint8_t  byte[2];
} address;

union length_union {
	uint16_t word;
	uint8_t  byte[2];
} length;

struct flags_struct {
	unsigned eeprom : 1;
	unsigned rampz  : 1;
} flags;

uint8_t buff[256];
uint8_t address_high;

uint8_t pagesz=0x80;

uint8_t i;
uint8_t bootuart = 0;

uint8_t error_count = 0;

void (*app_start)(void) = 0x0000;


int main(){
				address.word = address.word << 1;	        //address * 2 -> byte location
				/* if ((length.byte[0] & 0x01) == 0x01) length.word++;	//Even up an odd number of bytes */
				if ((length.byte[0] & 0x01)) length.word++;	//Even up an odd number of bytes
				cli();					//Disable interrupts, just to be sure

asm volatile(
    "clr	r17		\n\t"	//page_word_count
    "lds	r30,address	\n\t"	//Address of FLASH location (in bytes)
    "lds	r31,address+1	\n\t"
    "ldi	r28,lo8(buff)	\n\t"	//Start of buffer array in RAM
    "ldi	r29,hi8(buff)	\n\t"
    "lds	r24,length	\n\t"	//Length of data to be written (in bytes)
    "lds	r25,length+1	\n\t"
    "length_loop:		\n\t"	//Main loop, repeat for number of words in block							 							 
    "cpi	r17,0x00	\n\t"	//If page_word_count=0 then erase page
    "brne	no_page_erase	\n\t"						 
    "wait_spm1:		\n\t"
    "lds	r16,%0		\n\t"	//Wait for previous spm to complete
    "andi	r16,1           \n\t"
    "cpi	r16,1           \n\t"
    "breq	wait_spm1       \n\t"
    "ldi	r16,0x03	\n\t"	//Erase page pointed to by Z
    "sts	%0,r16		\n\t"
    "spm			\n\t"							 

    "wait_spm2:		\n\t"
    "lds	r16,%0		\n\t"	//Wait for previous spm to complete
    "andi	r16,1           \n\t"
    "cpi	r16,1           \n\t"
    "breq	wait_spm2       \n\t"									 

    "ldi	r16,0x11	\n\t"	//Re-enable RWW section
    "sts	%0,r16		\n\t"						 			 
    "spm			\n\t"

    "no_page_erase:		\n\t"							 
    "ld	r0,Y+		\n\t"	//Write 2 bytes into page buffer
    "ld	r1,Y+		\n\t"							 
                
    "wait_spm3:		\n\t"
    "lds	r16,%0		\n\t"	//Wait for previous spm to complete
    "andi	r16,1           \n\t"
    "cpi	r16,1           \n\t"
    "breq	wait_spm3       \n\t"
    "ldi	r16,0x01	\n\t"	//Load r0,r1 into FLASH page buffer
    "sts	%0,r16		\n\t"
    "spm			\n\t"
                
    "inc	r17		\n\t"	//page_word_count++
    "cpi r17,%1	        \n\t"
    "brlo	same_page	\n\t"	//Still same page in FLASH
    "write_page:		\n\t"
    "clr	r17		\n\t"	//New page, write current one first
    "wait_spm4:		\n\t"
    "lds	r16,%0		\n\t"	//Wait for previous spm to complete
    "andi	r16,1           \n\t"
    "cpi	r16,1           \n\t"
    "breq	wait_spm4       \n\t"
					 							 
    "ldi	r16,0x05	\n\t"	//Write page pointed to by Z
    "sts	%0,r16		\n\t"
    "spm			\n\t"

    "wait_spm5:		\n\t"
    "lds	r16,%0		\n\t"	//Wait for previous spm to complete
    "andi	r16,1           \n\t"
    "cpi	r16,1           \n\t"
    "breq	wait_spm5       \n\t"									 
    "ldi	r16,0x11	\n\t"	//Re-enable RWW section
    "sts	%0,r16		\n\t"						 			 
    "spm			\n\t"					 		 

    "same_page:		\n\t"							 
    "adiw	r30,2		\n\t"	//Next word in FLASH
    "sbiw	r24,2		\n\t"	//length-2
    "breq	final_write	\n\t"	//Finished
    "rjmp	length_loop	\n\t"
    "final_write:		\n\t"
    "cpi	r17,0		\n\t"
    "breq	block_done	\n\t"
    "adiw	r24,2		\n\t"	//length+2, fool above check on length after short page write
    "rjmp	write_page	\n\t"
    "block_done:		\n\t"
    "clr	__zero_reg__	\n\t"	//restore zero register

     : "=m" (SPMCSR) : "M" (PAGE_SIZE) : "r0","r16","r17","r24","r25","r28","r29","r30","r31"
    );
				/* Should really add a wait for RWW section to be enabled, don't actually need it since we never */
				/* exit the bootloader without a power cycle anyhow */
}

char getch(void)
{
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega1280__)
	uint32_t count = 0;
	if(bootuart == 1) {
		while(!(UCSR0A & _BV(RXC0))) {
			/* 20060803 DojoCorp:: Addon coming from the previous Bootloader*/               
			/* HACKME:: here is a good place to count times*/
			count++;
			if (count > MAX_TIME_COUNT)
				app_start();
		}

		return UDR0;
	}
	else if(bootuart == 2) {
		while(!(UCSR1A & _BV(RXC1))) {
			/* 20060803 DojoCorp:: Addon coming from the previous Bootloader*/               
			/* HACKME:: here is a good place to count times*/
			count++;
			if (count > MAX_TIME_COUNT)
				app_start();
		}

		return UDR1;
	}
	return 0;
#elif defined(__AVR_ATmega168__)  || defined(__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
	uint32_t count = 0;
	while(!(UCSR0A & _BV(RXC0))){
		/* 20060803 DojoCorp:: Addon coming from the previous Bootloader*/               
		/* HACKME:: here is a good place to count times*/
		count++;
		if (count > MAX_TIME_COUNT)
			app_start();
	}
	return UDR0;
#else
	/* m8,16,32,169,8515,8535,163 */
	uint32_t count = 0;
	while(!(UCSRA & _BV(RXC))){
		/* 20060803 DojoCorp:: Addon coming from the previous Bootloader*/               
		/* HACKME:: here is a good place to count times*/
		count++;
		if (count > MAX_TIME_COUNT)
			app_start();
	}
	return UDR;
#endif
}