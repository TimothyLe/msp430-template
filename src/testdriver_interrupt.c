/*
 * Example testdriver_interrupt.c file for MSP430
 * Toggle LED on interrupts from Timer0_A3
 */

#include <msp430.h>
#include <intrinsics.h>
//#include <io430x11x1.h> // specific device
#define LED1 BIT0 	//P1.0 red LED

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer 
    P1OUT = LED1;
    P1DIR = LED1;
    TA0CCR0 = 49999; // Upper limit count for TAR 
    TA0CCTL0 = CCIE; // Enable interrupts 
    TA0CTL = MC_1|ID_3|TASSEL_2|TACLR;
    // Up mode, divide clock by 8, clock from SMCLK, clear
    __enable_interrupt(); // Enable interrupts (intrinsics)
    for (;;) 
    {
        // Loop forever
    }
}

// Interrupt service routine for Timer0_A
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR (void) 
{
    P1OUT ^= LED1; // TOGGLE LED
}
