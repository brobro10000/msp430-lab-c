#include <msp430.h>
#define FLAGS UCA1IFG //Contains the transmit and receive flags
#define RXFLAG UCRXIFG //receive flag
#define TXFLAG UCTXIFG //Transmit flag
#define TXBUFFER UCA1TXBUF //Transmit buffer
#define RXBUFFER UCA1RXBUF //Receive Buffer
#define greenLED BIT7

/**
 * main.c
 */
int main(void)
{
      WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
        PM5CTL0 &= ~LOCKLPM5;   //enable GPIO pins

    // X-axis: A10/P9.2, for A10 (P9DIR=x, P9SEL1=1, P9SEL0=1)
    P9SEL1 |= BIT2;
    P9SEL0 |= BIT2;

    config_ACLK_to_32KHz_crystal();
    TA0CTL = TASSEL_1 | ID_0 | MC_2 | TACLR;
        TA0CTL &= ~TAIFG;

     P9OUT |= greenLED;
     P9DIR &= ~greenLED;

    Initialize_UART();
    Initialize_ADC();
    int i=0;
    int dataX = 0;
    int dataY = 0;
    char * headerX[] = 'X Coordinate';
    char * headerY[] = 'Y Coordinate';
    for(;;)
    {
        ADC12CTL0 |= ADC12SC;
        while((ADC12CTL1 & ADC12BUSY) != 0){}
        if((TA0CTL & TAIFG) != 0){
            dataX = ADC12MEM0;
            uart_write_char('X');
            uart_write_uint16(dataX);
            uart_write_char('\n');
            uart_write_char('\r');
            dataY = ADC12MEM1;
            uart_write_char('Y');
            uart_write_uint16(dataY);
            uart_write_char('\n');
            uart_write_char('\r');
            TA0CTL &= ~TAIFG;
        }

      }
    }



void Initialize_ADC() {
// Divert the pins to analog functionality
// X-axis: A10/P9.2, for A10 (P9DIR=x, P9SEL1=1, P9SEL0=1)
    P9SEL1 |= BIT2;
    P9SEL0 |= BIT2;
    //Y AXIS
    P8SEL1 |=BIT7;
    P8SEL0 |=BIT7;
// Turn on the ADC module
    ADC12CTL0 |= ADC12ON;
// Turn off ENC (Enable Conversion) bit while modifying the configuration
    ADC12CTL0 &= ~ADC12ENC;
//*************** ADC12CTL0 ***************
// Set ADC12SHT0 (select the number of cycles that you determined)
ADC12CTL0 |= ADC12SHT0_2 | ADC12MSC; //for multisample conversions
//*************** ADC12CTL1 ***************
// Set ADC12SHS (select ADC12SC bit as the trigger)
// Set ADC12SHP bit
// Set ADC12DIV (select the divider you determined)
// Set ADC12SSEL (select MODOSC)
ADC12CTL1 |= ADC12SHS_0 | ADC12SHP | ADC12DIV_0 | ADC12SSEL_0 | ADC12CONSEQ_1;


//*************** ADC12CTL2 ***************
// Set ADC12RES (select 12-bit resolution)
// Set ADC12DF (select unsigned binary format)
ADC12CTL2 |= ADC12RES_2;
ADC12CTL2 &= ~ADC12DF;
//*************** ADC12CTL3 ***************
// Leave all fields at default values
ADC12CTL3 |= ADC12CSTARTADD_0; //STARTADD to 0
//*************** ADC12MCTL0 ***************
// Set ADC12VRSEL (select VR+=AVCC, VR-=AVSS)
// Set ADC12INCH (select channel A10)
ADC12MCTL0 |= ADC12VRSEL_0 | ADC12INCH_10;

//*************** ADC12MCTL1 ***************
// Set ADC12VRSEL (select VR+=AVCC, VR-=AVSS)
// Set ADC12INCH (select the analog channel that you found)
// Set ADC12EOS (last conversion in ADC12MEM1)
ADC12MCTL1 |= ADC12VRSEL_0 | ADC12INCH_4 | ADC12EOS;
// Turn on ENC (Enable Conversion) bit at the end of the configuration
ADC12CTL0 |= ADC12ENC;
return;
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
void uart_write_char(unsigned char ch)
{
    //wait for any ongoing transmission to complete
    while ((FLAGS & TXFLAG) == 0)
    {
    }

    //Write the byte to the transmit buffer
    TXBUFFER = ch;
}
void uart_write_string(char * str)
{
    int i;
    for(i=0; i < strlen(str); i++){
        uart_write_char(str[i]);
    }
}

void uart_write_uint16(unsigned int n)
{
    int temp;
    if(n>=10000)
    {
        temp = n/10000;
        uart_write_char(temp+48);
    }
    if(n>=1000)
    {
        temp = n/1000 % 10;
        uart_write_char(temp+48);
    }
    if(n>=100)
    {
        temp = n/100 % 10;
        uart_write_char(temp+48);
    }
    if(n>=10)
    {

        temp = (n/10) % 10;
        uart_write_char(temp+48);
    }
     n = n % 10;
     uart_write_char(n+48);

    //Write the byte to the transmit buffer
    //TXBUFFER = n;
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
    while ((CSCTL5 & LFXTOFFG) != 0);

    CSCTL0_H = 0; //lock CS registers
    return;
}
