#include <msp430fr6989.h>
#define redLED BIT0 //Red LED at P1.0
#define greenLED BIT7
#define BUT1 BIT1 //Button S1 at Port 1.1
#define BUT2 BIT2
/**
 * main.c
 */
int status = 0;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;   //enable GPIO pins

    P1DIR |= redLED;    //pins as output
    P1OUT &= ~redLED;    //red off
    P9DIR |= greenLED;
    P9OUT &= ~greenLED;

    P1DIR &= ~(BUT1|BUT2); //Button 1 enable for interrupt 16-21
    P1REN |= (BUT1|BUT2);
    P1OUT |= (BUT1|BUT2);
    P1IE  |= (BUT1|BUT2);
    P1IES |= (BUT1|BUT2);
    P1IFG &= ~(BUT1|BUT2);

    //ACLK @ 32khz
    config_ACLK_to_32KHz_crystal();


    TA0CCR0 = (6000-1);
    TA0CCTL0 |= CCIE;
    TA0CCTL0 &= ~CCIFG;

    // up Mode ACLK /1 div
    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;

    //enable LPM3
    _low_power_mode_3();

}
//********ISR PORT1 for button**********
#pragma vector = PORT1_VECTOR //Link the ISR to the vector
__interrupt void PORT1_ISR()
{
    while(status==0){
    if((P1IFG & BUT1)!= 0)
        {
            P1IFG &= ~BUT1;
            P1IE &= ~BUT1;
            TA0CCTL0 |= CCIE;
            TA0CCTL0 &= ~CCIFG;
            status = 1;
        }
    if((P1IFG & BUT2)!= 0)
    {
        P1IFG &= ~BUT2;
        P1IE &= ~BUT2;
        TA0CCTL0 |= CCIE;
        TA0CCTL0 &= ~CCIFG;
        status = 2;
    }
    }
}
//**********ISR Timer0 for timer***********
#pragma vector = TIMER0_A0_VECTOR //Link the ISR to the Vector
__interrupt void T0A0_ISR()
{
    while(status == 2)
    {
    P1IFG &= ~BUT2;
    P1IE |= BUT2;
    if((BUT2)!= 0)
    {
        P9OUT ^= greenLED;
        P1IFG &= ~BUT2;
    }
    status = 0;
    }
    while(status == 1)
    {
    P1IFG &= ~BUT1;
    P1IE |= BUT1;
    if((BUT1)!= 0)
    {
        P1OUT ^= redLED;
        P1IFG &= ~BUT1;
    }

    status = 0;
    }
    TA0CCTL0 &= ~CCIFG;
    TA0CCTL0 &= ~CCIE;
}
void config_ACLK_to_32KHz_crystal()
    {
        //By default, ACLK runs on LFMODCLK at 5MHz/128 = 39kHz

        //Reroute pins to LFXIN/LFXOUT functionality
        PJSEL1 &= ~BIT4;
        PJSEL0 |= BIT4;

        //Wait until the oscillator fault flags remain cleared
        CSCTL0 = CSKEY;
        do
        {
            CSCTL5 &= ~LFXTOFFG;    //local fault flag
            SFRIFG1 &= ~OFIFG;      //Global fault flag
        }
        while((CSCTL5 & LFXTOFFG) != 0);

        CSCTL0_H = 0; //lock CS registers
        return;
    }

