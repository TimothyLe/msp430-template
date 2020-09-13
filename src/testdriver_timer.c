/*
 * Example testdriver_timer.c file for MSP430
 * Configure time based control for LED to flash at 1 Hz
 */

#include <msp430.h>
#define LED1 BIT0 	//P1.0 red LED

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer 
    P1OUT = ~LED1;
    P1DIR = LED1;   // set P1.0 & 6 to outputs
                    // (red & green LEDs)
    TA0CTL = MC_2|ID_3|TASSEL_2|TACLR; // Timer_A

    // More accurate timer if controlling amount to count 
    TACCR0 = 49999;                         // Upper limit of count for TAR 
    TACTL = MC_1 | ID_3 | TASSEL_2 | TACLR; // Set up and start Timer A 
    // Up to CCR0 mode, divide clock by 8, clock from SMCLK, clear timer

    for(;;)
    {
        /* Manual Counter; disadvantage is no usage of hardware timer */
        // volatile unsigned int i;
        // P1OUT ^= 0x41;  // Toggle P1.0 & 6 using XOR
        // i = 50000;      // Delay
        // do (i--);
        // while (i != 0);
        
        /* Timer method */
        while (TA0CTL_bit.TAIFG == 0) // wait overflow
        {
            // CPU polling
        }
        TA0CTL_bit.TAIFG = 0;   // Clear overflow flag
        P1OUT ^= LED1;          // Toggle LEDs
    }
}
