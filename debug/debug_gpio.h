/*
 * GPIO Debugger
 */

#include <msp430.h>
#define LED1 BIT6 	//P1.0 green LED
#define B1 BIT3		//P1.3 btn

volatile unsigned int i,j;

void debug_gpio(void) {
    // halt watchdog
    WDTCTL = WDTPW + WDTHOLD;
    P1OUT |= LED1 + B1;
    P1DIR = LED1; 	// Set pin LED1 to output
    P1REN = B1; 	// Set pin to use pull-up res
    for(;;) 
    {
        // Button up
        while((P1IN & B1)!=0)
        {
            i = P1IN;
            j = P1OUT;
        }
        P1OUT &= ~LED1;     // Turn LED1 off
        // Button down
        while((P1IN & B1)==0)
        {
            i = P1IN;
            j = P1OUT;
        }
        P1OUT |= LED1;     // Turn LED1 on
    }
}
