#include <msp430FR6989.h>
#define redLED BIT0
#define greenLED BIT7

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;
	
	P1DIR |= redLED;    //Direct pin as output
	P9DIR |= greenLED;  //Direct pin as output
	P1OUT &= ~redLED;   //Turn LED off
	P9OUT &= ~greenLED; //Turn LED off

	//Configure ACLK to the 32khz crystal
	config_ACLK_to_32KHz_crystal();

	//Timer_A configuration (fill the line below)
	//Use ACLK, divide by 1, continuous mode, TAR cleared,
	//enable interrupt for rollback to zero event
	TA0CTL = TASSEL_1 | ID_0 | MC_2 | TACLR | TAIE;

	//ensure the flag is cleared at the start
	TA0CTL &= ~TAIFG;

	//Enable LPM
	//_low_power_mode_3();

	//Enable global interrupt bit(call an intrinsic function
	_enable_interrupt();

	//Infinite loop... the code waits here between interrupts
	for(;;){}
}

	void config_ACLK_to_32KHz_crystal()
	{
	    //By default, ACLK runs on LFMODCLK at 5MHZ/128 = 39KHz

	    //reroute pins to LFXIN/LFXOUT functionality
	    PJSEL1 &= ~BIT4;
	    PJSEL0 |= BIT4;

	    //wait until the oscillator fault flags remain cleared
	    CSCTL0 = CSKEY;
	    do
	    {
	        CSCTL5 &= ~LFXTOFFG; //local fault flag
	        SFRIFG1 &= ~OFIFG;  //Global fault flag
	    }
	    while((CSCTL5 & LFXTOFFG) != 0);

	    CSCTL0_H=0; //lock CS registers
	    return;
	}
//********Writing to ISR*******
#pragma vector = TIMER0_A1_VECTOR //Link the ISR to the vector
	__interrupt void T0A1_ISR()
	{
	    //Toggle both LEDs
	    P1OUT ^= redLED;
	    P9OUT ^= greenLED;
	    //clear the TAIFG flag
	    TA0CTL &= ~TAIFG;
	}
