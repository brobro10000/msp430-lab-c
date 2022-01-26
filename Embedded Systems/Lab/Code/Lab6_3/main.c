//Generating a PWM on P1.0 (red LED)
//P1.0 coincides with TA0.1 (Timer0_A channel 1)
//Divert P1.0 pin to TA0.1 ---> P1DIR=1, P1SEL=0, P1SEL0=1
//PWM frequency 1000hz --> .001 seconds
#include <msp430fr6989.h>
#define PWM_PIN BIT0

/**
 * main.c
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    //Divert Pin to TA0.1 Functionality (complete the last 2 lines)
    P1DIR |= PWM_PIN;       //P1DIR bit = 1
    P1SEL1 &= ~BIT7;                  //P1SEL1 bit = 0
    P1SEL0 |= PWM_PIN;                //P1SEL0 bit = 1

    //32 KHZ clock
    config_ACLK_to_32KHz_crystal();

    //Starting the timer in up mode; period = .001 seconds
    //ACLK @ 32khz, DIV by 1, up mode
    TA0CCR0 = (33-1);
    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;

    //Configuring Channel 1 for PWM
    TA0CCTL1 |= OUTMOD_7;   //Output pattern: Reset/Set
    TA0CCR1 = 1;            //Modify this value between 0
                            //32 to adjust brightness level

    for(;;){}
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
