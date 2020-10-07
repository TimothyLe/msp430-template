/*
 * Example testdriver_gpio.c file for MSP430
 * Configure I/O port of Launchpad for input and output
 */

#include <msp430.h>
#define LED1 BIT0 	//P1.0 red LED
#define B1 BIT3		//P1.3 btn

int main() {
    // halt watchdog
    WDTCTL = WDTPW + WDTHOLD;
    P1OUT |= LED1 + B1;
    P1DIR = LED1; 	// Set pin LED1 to output
    P1REN = B1; 	// Set pin to use pull-up res
    for(;;) 
    {
        // Check if button is down
        if ((P1IN & B1)==0) 
        {
            P1OUT &= ~LED1; 	// YES turn off
        }
        else
        {
            P1OUT |= LED1; 	// NO turn on
        }
    }
}
