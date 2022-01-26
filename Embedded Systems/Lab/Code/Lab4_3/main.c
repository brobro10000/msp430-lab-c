#include <msp430FR6989.h>
#define redLED BIT0
#define greenLED BIT7
#define BUT1 BIT1//Button S1 at port 1.1
#define BUT2 BIT2 //button s2 at port 1.2

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= redLED;    //Direct pin as output
    P9DIR |= greenLED;  //Direct pin as output
    P1OUT &= ~redLED;   //Turn LED off
    P9OUT &= ~greenLED; //Turn LED off

    P1DIR &= ~(BUT1|BUT2);
    P1REN |= (BUT1|BUT2);
    P1OUT |= (BUT1|BUT2);
    P1IE  |= (BUT1|BUT2);
    P1IES |= (BUT1|BUT2);
    P1IFG &= ~(BUT1|BUT2);




    //Enable LPM
    //_low_power_mode_4();

    //Enable global interrupt bit(call an intrinsic function
    _enable_interrupt();

    //Infinite loop... the code waits here between interrupts
    for(;;){}
}
//********Writing to ISR*******
#pragma vector = PORT1_VECTOR //Link the ISR to the vector
    __interrupt void PORT1_ISR()
    {
        if((P1IFG & BUT1) != 0)
        {
        //Toggle both LEDs
        P1OUT ^= redLED;
        //Clear BUT1 in  P1IFG
        P1IFG &= ~BUT1;
        }

        if((P1IFG & BUT2) != 0)
        {
        //toggle the green LED
        P9OUT ^= greenLED;
        //clear BUT2 in P1IFG
        P1IFG &= ~BUT2;
        }
    }
