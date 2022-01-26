#include <msp430.h> 
#define FLAGS UCA1IFG //Contains the transmit and receive flags
#define RXFLAG UCRXIFG //receive flag
#define TXFLAG UCTXIFG //Transmit flag
#define TXBUFFER UCA1TXBUF //Transmit buffer
#define RXBUFFER UCA1RXBUF //Receive Buffer
#define redLED BIT0
#define greenLED BIT7
/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;   //enable GPIO pins

    P1DIR |= redLED;    //pins as output
    P1OUT &= ~redLED;    //red off
    P9DIR |= greenLED;    //pins as output
    P9OUT &= ~greenLED;    //red off

    //Divert Pins to backchannel UART functionality
    //(UCA1TXD same as 3.4)(UCA1RXD same as 3.5)
    //P3SEL=00, P3SEL0 = 11 P2DIR=XX
    P3SEL1 &= ~(BIT4|BIT5);
    P3SEL0 |= (BIT4|BIT5);
    Initialize_UART();
    int i=65;
    int j=0;
    TXBUFFER &= ~TXFLAG;
//numbers
    while(i<58){
        P1OUT |= redLED;
    for(j=0; j<30000;j++){}
    uart_write_char(i++);

    uart_write_char('\n');
    uart_write_char('\r');
    //uart_read_char(i++);
    P1OUT &= ~redLED;
    if(i == 58)
         i=48;
    }
    TXBUFFER |= TXFLAG;
//TURNS ON AND OFF GREEN LED WITH INPUT 1 2
while(RXFLAG != 0)
{
 uart_read_char();
 if(RXBUFFER == '1')
 P9OUT |= greenLED;
 if(RXBUFFER == '2')
     P9OUT &= ~greenLED;
}
//    uart_write_uint16(953);
TXBUFFER |= TXFLAG;
}
void Initialize_UART(void)
{
    //Divert Pins to UART functionality
    P3SEL1 &= ~(BIT4|BIT5);
    P3SEL0 |= (BIT4|BIT5);

    //Use smclk clock; leave the other settings default
    UCA1CTLW0 |= UCSSEL_2;

    //Configure clock dividers and modulators
    //UCBR = 6, UCBRF = 13, UCBRS = 0x22, UCOS16=1 (oversampling)
    UCA1BRW = 6;
    UCA1MCTLW = UCBRS5 | UCBRS1| UCBRF3| UCBRF2| UCBRF0| UCOS16;

    //exit the reset state(so transmission/reception cna begin)
    UCA1CTLW0 &= ~UCSWRST;
}
void uart_write_char(unsigned char ch){
    //wait for any ongoing transmission to complete
    while((FLAGS & TXFLAG)==0){}

    //Write the byte to the transmit buffer
    TXBUFFER = ch;
}
unsigned char uart_read_char(void){
    unsigned char temp;
    //return null if no byte received
    if((FLAGS & RXFLAG)== 0)
    return;
    //otherwise, copy the received byte (clears the flag) and return it
    temp = RXBUFFER;
    return temp;
}
