#include <msp430fr6989.h>
#define redLED BIT0 //red at p1.0
#define greenLED BIT7   //green at p9.7
#define BUT1 BIT1
#define BUT2 BIT2

void Initialize_LCD();
void display_num_lcd(unsigned int n);

//The array has the shapes of the digits (0 to 9)
//Complete this array...
const unsigned char LCD_Num[10] = {0xFC, 0x60, 0xDB,0xF3, 0x67, 0xB7, 0xBF, 0xE0, 0xFF, 0xF7 };

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;   //enable GPIO pins

    P1DIR |= redLED;    //pins as output
    P9DIR |= greenLED;
    P1OUT &= ~redLED;    //red on
    P9OUT |= greenLED; //green off

    P1DIR &= ~(BUT1|BUT2);
    P1REN |= (BUT1|BUT2);
    P1OUT |= (BUT1|BUT2);

    //ACLK 32khz
    config_ACLK_to_32KHz_crystal();
    //up mode limit
    TA0CCR0 = (32-1);
    //increment through upmode cycles
    int i=0;
    //TA0CTL config
    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;
    //clear flag
    TA0CTL &= ~TAIFG;
    //initializes the LCD_C module
    Initialize_LCD();

    //The line below can be used to clear all the segments
    //LCDCMEMCTL = LCDCLRM;     //clears all segments

    int n;
    int status = 0;
    //Flash the red and green LEDs
    for(;;){
       while((TA0CTL & TAIFG) == 0){}
       //First BUT1 Press to change status and set LEDs
       while((P1IN & BUT1)==0){
           P9OUT &=~greenLED;
           P1OUT |=redLED;
           //Checks LED status before status change to reduce debouncing
           if(redLED!=0)
           status = 1;
           }
       //Checks Status
       if(status == 1)
       {
           //LCD Display stays on if 1
           while(status == 1)
           {
           display_num_lcd(i);
               //Reset to zero BUT2
               if((P1IN & BUT2)== 0)
               {
                     i=0;
               }
               //Set Status to 0 on second press
               if((P1IN & BUT1) ==0)
                    status = 0;
           }
       }
       //reset to zero
       if((P1IN & BUT2)== 0){
           i=0;
       }
       // @ zero status counts
       if(status == 0)
       {
       display_num_lcd(i++);
       P9OUT |= greenLED;
       P1OUT &= ~redLED;
       }

       TA0CTL &= ~TAIFG;
       }
}

//**************************************************
//initialize the LCD_C module
// *** Source: Function obtained from MSP430FR6989's sample code**
void Initialize_LCD(){
    PJSEL0 = BIT4 | BIT5;      //for LFXT

    //Initialize LCD segments 0-21; 26-43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;

    //Configure LFXT 32khz crystak
    CSCTL0_H = CSKEY >> 8;  //Unlock CS registers
    CSCTL4 &= ~LFXTOFF;
    do{
        CSCTL5 &= ~LFXTOFFG;    //Clear LFXT fault flag
        SFRIFG1 &= ~OFIFG;
    } while(SFRIFG1 & OFIFG);   //test oscillator fault flag
    CSCTL0_H = 0;               //Lock CS registers

    //initialize LCD_C
    //ACLK, DIVIDER=1, PREDIVIDER = 16; 4pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

    //VLCD generated internally,
    //V2-V4 generated internally, V5 to ground
    //Set VLCD to 2.6v
    //Enable charge pump and select internal references for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;

    LCDCCPCTL = LCDCPCLKSYNC; //clock sync enabled

    LCDCMEMCTL = LCDCLRM;    //CLear lcd memory

    //Turn LCD on
    LCDCCTL0 |= LCDON;

    return;
    }
void display_num_lcd(unsigned int n)
{
    unsigned char *ptr[6] = {&LCDM8,&LCDM15,&LCDM19,&LCDM4,&LCDM6,&LCDM10};
    int i = 0;
    int digit;

        do{
           digit = n%10;
           *ptr[i] = LCD_Num[digit];
           n = n/10;
                   i++;
           }
        while(n!=0);
        while(i<=6)
        {
            *ptr[i] = 0;
            i++;
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
