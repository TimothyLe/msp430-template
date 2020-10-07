#include <msp430.h>
// #include <io430x20x2.h> // Header file for this device
// #include <intrinsics.h> // Intrinsic functions

// int i=1;
// void main(void) 
// {
//     WDTCTL = WDTPW | WDTHOLD; // Stop WDT
//     // TA1 trigger sample start
//     ADC10CTL1 = SHS_1 + CONSEQ_2 + INCH_1;
//     ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + ADC10IE;
//     __enable_interrupt();   // enable interrupts
//     TA0CCR0 = 30;           // Delay for Volt Ref to settle
//     TA0CCTL0 |= CCIE;       // compare-mode interrupt
//     TA0CTL = TASSEL_2 + MC_1; // SMCLK, up mode
//     /** 
//      * Setting time for internal ref is <30us
//      * If DCOCLK is 1 MHz, then 1 cycle takes 6us and 30 cycles is the delay for internal ref to settle
//      */
//     while(i);               // Wait for settle
//     TA0CCTL0 &= ~CCIE;      // Disable timer interrupt
//     __disable_interrupt();

//     /*
//         Timer0_A CCR1 out mode 3
//         OUT1 is set when timer counts to TA0CCR1 value and reset when timer counts to TA0CCR0
//      */
//     ADC10CTL0 |= ENC;       // ADC10 Enable
//     ADC10AE0 |= 0x02;       // P1.1 ADC10 option select
//     P1DIR |= 0x01;          // Set P1.0 output
//     TA0CCR0 = 2048 - 1;     // Sample period for ADC10
//     TA0CCTL1 = OUTMOD_3;    // TACCR1 set/reset
//     TA0CCR1 = 2046;         // TACCR1 OUT1 on time
//     TA0CTL = TASSEL_1 + MC_1;   // ACLK, up mode
//     while(1);
// }

// // ADC10 interrupt service routine
// #pragma vector=ADC10_VECTOR
// __interrupt void ADC10_ISR(void)
// {
//     /*
//         0x155 is 1/3 of 0x3FF which gives 0.5V of 1.5V  
//      */
//     if (ADC10MEM < 0x155)   // ADC10MEM = A1 > 0.5V
//         P1OUT &= ~0x01;     // Clear P1.0 LED off
//     else
//         P1OUT |= 0x01;      // Set P1.0 LED on
// }

// #pragma vector=TIMERA0_VECTOR
// __interrupt void ta0_isr(void)
// {
//     TA0CTL = 0;
//     i = 0;
// }

// #include <io430x20x2.h> // Header file for this device
#include <intrinsics.h> // Intrinsic functions
void main ( void )
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog
    P2SEL = BIT6; // OUT1 on P2 .6; no crystal
    P2OUT = 0; // Unused outputs driven low
    P2DIR = BIT6|BIT7;
    P1OUT = BIT5; // VCC to 10k fixed resistor
    P1DIR = BIT3|BIT5|BIT6|BIT7; // Unused outputs driven low
    // Timer_A for PWM at 125Hz on OUT1 , SMCLK/8, Up mode
    TACCR0 = BITA - 1; // Upper limit to match ADC10MEM
    TACCTL0 = OUTMOD_4; // Toggle OUT0 to stimulate ADC10
    TACCR1 = BIT9; // About 50% to start PWM
    TACCTL1 = OUTMOD_7; // Reset/set for positive PWM
    TACTL = TASSEL_2 | ID_3 | MC_1 | TACLR; // SMCLK/8, Up mode , clear
    // ADC on, refs VCC , VSS , sample 4 cycles , int ref off , interrupts
    ADC10CTL0 = SREF_0 | ADC10SHT_0 | ADC10ON | ADC10IE;
    // Input channel 1, start on rising edge of OUT0 , no clock division ,
    // internal ADC clock , single channel , repeated conversions
    ADC10CTL1 = INCH_1 | SHS_2 | ADC10DIV_0 | ADC10SSEL_0 | CONSEQ_2;
    ADC10AE0 = BIT1; // Enable analog input on channel 1
    ADC10CTL0_bit.ENC = 1; // Enable ADC10 conversions
    for (;;) { // Loop forever taking measurements
        __low_power_mode_0();
    }
}
//----------------------------------------------------------------------
// Interrupt service routine for ADC10 : update PWM with new reading
//----------------------------------------------------------------------
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR ( void ) // Acknowledged automatically
{
    TACCR1 = ADC10MEM; // Update PWM from new ADC reading
}