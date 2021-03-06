#include <msp430FR6989.h>
#define redLED BIT0
#define greenLED BIT7
/**
 * main.c
 */
int main(void)
{
    volatile unsigned int i;
    volatile unsigned int j;
    volatile unsigned int k;
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;   // Disable GPIO power-on default high-impedance mode

	P1DIR|= redLED;
	P1OUT &= ~redLED;

    P9DIR|= greenLED;
    P9OUT &= greenLED;

	for(;;){
	    //Delay Loop
	   for(i=0; i<3; i++){
	    for(k=0; k<40000; k++){} // uncomment 4 delay
	      P1OUT ^= redLED; //uncomment for flashing red delay
	    }
	    //P1OUT ^= redLED; //uncomment for flashing red light
//
	     //Uncomment for flashing lights
	    P9OUT ^= greenLED;


	}
}
