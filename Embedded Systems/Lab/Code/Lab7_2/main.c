#include <msp430fr6989.h>
#define redLED BIT0 //Red LED at P1.0
#define BUT1 BIT1 //Button S1 at Port 1.1
int status = 0;
/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;   //enable GPIO pins

    P1DIR |= redLED;    //pins as output
    P1OUT &= ~redLED;    //red off

    P1DIR &= ~BUT1; //Button 1 enable for interrupt 16-21
    P1REN |= BUT1;
    P1OUT |= BUT1;
    P1IE  |= BUT1;
    P1IES |= BUT1;
    P1IFG &= ~BUT1;

    //ACLK @ 32khz
    config_ACLK_to_32KHz_crystal();

    //8192 per second * 3 for 3 second LED timer
    //TA0CCR0 = (24576-1);
    TA0CCTL0 |= CCIE;
    TA0CCTL0 &= ~CCIFG;
    // up Mode ACLK /4 div
    TA0CTL = TASSEL_1 | ID_2 | MC_2 | TACLR;

    //enable LPM3
    _low_power_mode_3();

}
//********ISR PORT1 for button**********
#pragma vector = PORT1_VECTOR //Link the ISR to the vector
__interrupt void PORT1_ISR()
{
    //Button interrupt
    //Check flag
//  if((P1IFG & BUT1) != 0)
//  {
      P1OUT |=redLED; //turn on LED
      TA0CCTL0 |= CCIE; //Turn on timer interrupt
      TA0CCTL0 &= ~CCIFG; //Turn on timer interrupt flag
      P1IFG &= ~BUT1; // reset button flag
      status = 1; // set status to 1
      TA0CCR0 = TA0R + (24576-1);
  if(status == 1)
  {
      //P1IE &= ~BUT1; //turn off button interrupt/flag
      P1IFG &= ~BUT1;
  }

}
//**********ISR Timer0 for timer***********
#pragma vector = TIMER0_A0_VECTOR //Link the ISR to the Vector
__interrupt void T0A0_ISR()
{

    if(status == 1)
     P1OUT &= ~redLED;//turn led off
     TA0CCTL0 &= ~CCIE;//disable timer and flag
     TA0CCTL0 &= ~CCIFG;
     P1IFG &= ~BUT1; //renable button interrupt
     P1IE |= BUT1;
     status = 0;

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

