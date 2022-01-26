//Using Timer_A with 2 channels
// Using ACLK @ 32KHz (undivided)
//Channel 0 toggles the red LED every .1 seconds
//Channel 1 toggles the green LED ever .5 Seconds
#include <msp430fr6989.h>
#define redLED BIT0    //red at p.1.0
#define greenLED BIT7  //green at p9.7

/**
 * main.c
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       //Enable GPIO Pins

    P1DIR |= redLED;
    P9DIR |= greenLED;
    P1OUT &= ~redLED;
    P9OUT &= ~greenLED;

    //32KhZ crystal ACLK
    config_ACLK_to_32KHz_crystal();

    //configure channel 0
    TA0CCR0 = (820-1);       //@32KHz --> .1seconds
    TA0CCTL0 |= CCIE;
    TA0CCTL0 &= ~CCIFG;

    //configure channel 1
        TA0CCR1 = (4096-1);       //@32KHz --> .5seconds
        TA0CCTL1 |= CCIE;
        TA0CCTL1 &= ~CCIFG;

    //configure channel 2
        TA0CCR2 = (32768-1);
        TA0CCTL2 |= CCIE;
        TA0CCTL1 &= ~CCIFG;

    //Configure Timer(ACLK) (Divide by 4) (Continuous mode)
     TA0CTL = TASSEL_1 | ID_2 | MC_2 | TACLR;

     //Engage low-power mode
     _low_power_mode_3();

     return;
}
//ISR of channel 0(A0 Vector)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void T0A0_ISR()
{
    P1OUT ^= redLED;    //toggle the red led
    TA0CCR0 += 820;    //schedule the next interrupt
    //HW clears channel 0 flag(CCIFG in TA0CCTL0)
}
#pragma vector = TIMER0_A1_VECTOR
__interrupt void T0A1_ISR()
{
    if((TA0CCTL1 & CCIFG)!=0)
    {
    P9OUT ^= greenLED;  //toggle the greenLED
    TA0CCR1 += 4096;   //Schedule the next interrupt
    TA0CCTL1 &= ~CCIFG;  //Clear Channel 1 interrupt flag
    }
    static int flashing = 1;
    if((TA0CCTL2 & CCIFG)!=0)
    {
        if(flashing == 1)
        {
            TA0CCTL1 &= ~CCIE;
            TA0CCTL0 &= ~CCIE;
            P1OUT &= ~redLED;
            P9OUT &= ~greenLED;
            flashing = 0;
        } else if (flashing == 0)
        {
            TA0CCR1 = 4096;
            TA0CCTL1 &= ~CCIFG;
            TA0CCTL1 |= CCIE;
            TA0CCR0 = 820;
            TA0CCTL0 &= ~CCIFG;
            TA0CCTL0 |= CCIE;
            flashing = 1;
        }
        TA0CCR2 += 32768;
        TA0CCTL2 &= ~CCIFG;
    }
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
