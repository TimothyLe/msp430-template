/*
 * Example testdriver_power.c file for MSP430
 * Toggle P1.0 every 50000 cycles
 */

#include <msp430.h>
#define LED1 BIT0 	//P1.0 red LED

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer     
    P1DIR |= 0x01;      // set P1.0 as output
    TA0CCTL0 = CCIE;    // CCR0 interrupt enabled
    TA0CCR0 = 50000-1;
    TA0CTL = TASSEL_2 + MC_1;   // SMCLK up mode
    _BIS_SR(LPM0_bits + GIE);   // LPM0 with interrupt
}

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer0_A (void)
{
    P1OUT ^= 0x01;      // Toggle P1.0
}
