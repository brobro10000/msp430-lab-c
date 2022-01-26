#include <msp430fr6989.h>
#define redLED BIT0
#define greenLED BIT7

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;
	
	P1DIR |= redLED; //Direct pin as output
	P9DIR |= greenLED; //Direct pin as output
	P1OUT &= ~redLED; //Turn LED off
	P9OUT &= ~greenLED; //Turn LED off

	//Configure ACLK to the 32khz crystal(function call).
	config_ACLK_to_32KHz_crystal();
	//Set Timer Period
	TA0CCR0 = (32768-1);
	//Use ACLK, divide by 1, continuous mode, clear TAR
	TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;

	//Ensure flag is cleared at the start
	TA0CTL &= ~TAIFG;

	//Infinite loop
	for(;;)
	{
	    // empty while loop; waits here until TAIFG raised
	    while((TA0CTL & TAIFG) == 0){}

	    TA0CCR0 -=1000;
	    P1OUT ^= redLED;
	    P9OUT ^= greenLED;
	    TA0CTL &= ~TAIFG; //set flag
	     //set flag
	}
}
//configure CLK to the 32khz crystal(function call)
void config_ACLK_to_32KHz_crystal()
{
    //By Default, ACLK runs on LFMODCLK at 5MHz/128 = 39kHz

    //Reroute pins to LXIN/LFXOUT functionality
    PJSEL1 &= ~BIT4;
    PJSEL0 |= BIT4;

    //Wait until the oscillator fault flags remain cleared
    CSCTL0 = CSKEY;
    do
    {
        CSCTL5 &= ~LFXTOFFG;    //local fault flag
        SFRIFG1 &= ~OFIFG;  //Global fault flag
    }
    while((CSCTL5 & LFXTOFFG) != 0);

    CSCTL0_H = 0; //lock CS registers
    return;
}
