#include <msp430.h>

//determined empirically to be the slowest this can go without being annoying
#define MAX_DELAY 10

//determined empirically to be the fastest this can go without being annoying
#define MIN_DELAY 6

// this has to be a power of 2 minus 1, and it will play that power of 2 number of notes in an open chord.
// 0x7, 0x3, and 0x1 are all good values, to play 8, 4, and 2 notes in an open chord, respectively.
// anything higher is just annoying
#define NUM_NOTES_IN_CHORD 0x03

// this must be no less than NUM_NOTES_IN_CHORD + 1 or else a divide by zero will occur
const unsigned int MIN_WAVELENGTH = NUM_NOTES_IN_CHORD + 1;

// this is more a matter of preference, determines the full range of tuning capability
const unsigned int MAX_WAVELENGTH = NUM_NOTES_IN_CHORD + 13;

// the pins used for the laser detectors
const unsigned int input = BIT5 | BIT6 | BIT7;

unsigned int wavelength;
unsigned int delay;

int main(void) {
	unsigned int counter = 0;
	wavelength = MAX_WAVELENGTH;
	delay = MAX_DELAY - 4;

	WDTCTL = WDTPW + WDTHOLD;		// Stop watchdog timer
	P1DIR = BIT0 | BIT1 | BIT2;		// Set output direction for said pins
	P1REN = BIT3 | BIT4 | input; 	// enable the internal pull-down resistors for said pins
	P1OUT = 0x00;					// init output state, cuz I'm being pedantic
	P1IE = BIT3 | BIT4;				// enable interrupts on user-input button pins
	P1IES = 0x00;					// make all of the interrupts, whatever they are, run on the leading edge
	P1IFG = 0x00;					// reset all of the interrupts to off
	__enable_interrupt();

	for(;; ++counter)
		P1OUT = (counter / (wavelength - ((counter >> delay) & NUM_NOTES_IN_CHORD))) & ((P1IN ^ input) >> 5);
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	if(P1IN & BIT3)
	{
		--wavelength;
		if(wavelength < MIN_WAVELENGTH)
			wavelength = MAX_WAVELENGTH; // anything less than 4 and we will end up with a divide-by-zero error
		P1IFG ^= BIT3;
	}
	if(P1IN & BIT4)
	{
		--delay;
		if(delay < MIN_DELAY)
			delay = MAX_DELAY;
		P1IFG ^= BIT4;
	}
}
