/*
 * Example testdriver_adc.c file for MSP430
 * Sample ADC10
 */

#include <msp430.h>
// #include <intrinsics.h>
//#include <io430x11x1.h> // specific device
#define LED1 BIT0 	//P1.0 red LED

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer 
    // H&S time 16x, interrupt enabled
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10E;
    ADC10CTL1 = INCH_1; // Input from A1
    ADC10AE0 |= 0x02; // Enable pin A1 for analog in    
    P1DIR |= 0x01;
    ADC10CTL0 |= ENC + ADC10SC; // Start sampling
    for (;;)
    {

    }
    
}

// Interrupt service routine for Timer0_A
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void) 
{
    if (ADC10MEM < 0x1FF) P1OUT &= ~0x01;
    else P1OUT |= 0x01;
    ADC10CTL0 |= ENC + ADC10SC; // enable sampling
}
