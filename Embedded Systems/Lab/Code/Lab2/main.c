#include <msp430fr6989.h>
#define redLED BIT0 //red led at p1.0

#define greenLED BIT7 //green led at p9.7
#define BUT1 BIT1 //button s1 at p1.1
#define BUT2 BIT2 //button s1 at p1.1
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;    //enable the gpio pins
    volatile unsigned int i;
    volatile unsigned int j;
    //Configure and inintialize LEDs
    P1DIR |= redLED;//Direct pin as output
    P9DIR |= greenLED;//Direct pin as output
    P1OUT &= ~redLED;//Turn LED off
    P9OUT &= ~greenLED; //Turn LED off

    //Configure buttons
    P1DIR &= ~BUT1;
    P1REN |= BUT1;
    P1OUT |= BUT1;

    //Configure buttons
    P1DIR &= ~BUT2;
    P1REN |= BUT2;
    P1OUT |= BUT2;


    //Polling the button in an infinite loop
    for(;;){
        //use while loop to hold value
        //Fill the if-statement below... RED LED

        if((P1IN & BUT1 ) == 0){

        for(i=0;i<25000;i++){}
        P9OUT ^= greenLED;
        P1OUT |= redLED;
        }


        if((P1IN & BUT2 ) == 0){
               P9OUT &= ~greenLED;
               P1OUT &= ~redLED;
               }
        }

}




