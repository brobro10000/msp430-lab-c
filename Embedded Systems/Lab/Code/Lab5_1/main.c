#include <msp430fr6989.h>
#define redLED BIT0 //red at p1.0
#define greenLED BIT7   //green at p9.7
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
    P1OUT |= redLED;    //red on
    P9OUT &= ~greenLED; //green off

    //initializes the LCD_C module
    Initialize_LCD();
    display_num_lcd(65535);
    //The line below can be used to clear all the segments
    //LCDCMEMCTL = LCDCLRM;     //clears all segments
    // Display 430 on the rightmost 3 digits
//  LCDM8 = LCD_Num[0];
//  LCDM15 = LCD_Num[3];
//  LCDM19 = LCD_Num[4];
//  LCDM4 = LCD_Num[4];
//  LCDM6 = LCD_Num[5];
//  LCDM10 = LCD_Num[6];

    int n;
    //Flash the red and green LEDs
    for(;;){
        for(n=0; n<=50000; n++){} //delay loop
        P1OUT ^= redLED;
        P9OUT ^= greenLED;
    }
    return 0;
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
// do the first iteration of the number on lcd
        do{
           digit = n%10;
           *ptr[i] = LCD_Num[digit];
           n = n/10;
                   i++;
           }
//while the digit doesn’t equal zero
        while(n!=0);
//clears zero on screen while not in use
        while(i<=6)
        {
            *ptr[i] = 0;
            i++;
        }
}
