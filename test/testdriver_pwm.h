/*
 * Example testdriver_pwm.c file for MSP430
 * Toggle ADC sampling on PWM
 */

#include <msp430.h>
#define LED1 BIT0 	//P1.0 red LED

void testdriver_pwm(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer     
    // TA1 trigger sample start
    ADC10CTL1 = SHS_1 + CONSEQ_2 + INCH_1;
    ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + ADC10IE;

    __enable_interrupt(); // enable interrupts 
    
    TA0CCTL0 |= CCIE;           // CCR0 interrupt enabled
    TA0CCR0 = 30;               // Delay for volt ref to settle
    TA0CTL = TASSEL_2 + MC_1;   // SMCLK up mode
    LPM0;                       // wait for settle
    TA0CCTL0 &= ~CCIE;          // disable timer interrupt
    __disable_interrupt();

    /** Timer_A CCR1 out mode 3
     *  OUT1 is set when the timer counts to the TACCR1 value
     *  and reset when the timer counts to TACCR0
     */
    ADC10CTL0 |= ENC;       // ADC10 enable
    ADC10AE0 |= 0x02;       // P1.1 ADC10 option select
    P1DIR |= 0x01;          // set P1.0 output
    TA0CCR0 = 2048 - 1;     // sampling period
    TA0CCTL1 = OUTMOD_3;    // TACCR1 set/reset
    TACCR1 = 2046;          // TACCR1 OUT1 on time
    TA0CTL = TASSEL_1 + MC_1; // ACLK up mode

    // Enter LPM3 w/ interrupts
    __bis_SR_register(LPM3_bits + GIE);
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void){
  if (ADC10MEM < 0x1FF)     // ADC10MEM = A1 > 0.5V?
    P1OUT &= ~0x01;         // Clear P1.0 LED off
  else
    P1OUT |= 0x01;          // Set P1.0 LED on
}

#pragma vector=TIMERA0_VECTOR
__interrupt void ta0_isr(void){
  TA0CTL = 0;
  LPM0_EXIT;                // Exit LPM0 on return
}

