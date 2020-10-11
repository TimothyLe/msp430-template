// usci2cmaster1.c - receive temperature over I2C using USCI_B0
// Master mode , receive two bytes from slave; needs pull -ups on SCL , SDA
// Simple control flow for I2C , all in main routine , no interrupts
// FG4619 on TI Experimenter's Board , 32KHz crystal , 1MHz DCO ( default)
// SCL on P3.2, SDA on P3.1
// Display temperature on LCD , flash LED to show activity
// J H Davies , 2007-12-01; IAR Kickstart version 4.09A
//----------------------------------------------------------------------
#include <io430xG46x.h> // Needs corrected version for USCI!
#include <intrinsics.h> // Intrinsic functions
#include <stdint.h> // Integers of defined sizes
#include "LCDutils2.h" // TI exp board utility functions
#define SLAVE_ADDRESS 0x48 // I2C address of thermometer
// #include <msp430.h>
#define LED P5OUT_bit.P5OUT_1 // Output pin for LED (active high)
// Pin is output low by default
void test_usci2cmaster1 ( void )
{
    volatile uint16_t i; // Loop counter to stabilize FLL+
    int16_t Temperature; // Value received over I2C
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    FLL_CTL0 = XCAP14PF; // 14pF load caps; 1MHz default
    do { // Wait until FLL has locked
        for (i = 0x2700; i > 0; --i) { // One loop should be enough
        } // Delay for FLL+ to lock
        IFG1_bit.OFIFG = 0; // Attempt to clear osc fault flag
    } while (IFG1_bit.OFIFG != 0); // Repeat if not yet clear
    PortsInit(); // Initialize ports
    LCDInit(); // Initialize SBLCDA4
    DisplayHello(); // Display HELLO on LCD
    P3SEL = BIT1 | BIT2; // Route pins to USCI_B for I2C
    // 7-bit addresses (default), single master , master mode , I2C , synch
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;
    // Clock from SMCLK , receiver (default), hold in reset
    UCB0CTL1 = UCSSEL1 | UCSWRST;
    UCB0BR1 = 0; // Upper byte of divider word
    UCB0BR0 = 10; // Clock = SMCLK / 10 = 100KHz
    UCB0I2COA = 0; // Ignore genl call ; own address = 0
    UCB0CTL1 &= ˜ UCSWRST; // Release from reset
    // Set up basic timer for interrupts at 2Hz (500ms)
    BTCTL = BTHOLD | BT_ADLY_500; // Hold , period = 500 ms from ACLK
    BTCNT2 = 0; // Clear counter
    BTCTL &= ˜ BTHOLD; // Start basic timer
    IE2_bit. BTIE = 1; // Enable basic timer interrupts
    for (;;) { // Transfers triggered by BT
        __low_power_mode_3(); // Wait for BT (needs only ACLK)
        LED = 1; // Show start of activity
        UCB0I2CSA = SLAVE_ADDRESS; // Slave to be addressed
        UCB0CTL1 |= UCTXSTT; // Send Start and slave address
        while ((UCB0CTL1 & UCTXSTT) != 0) {
        } // Wait for address to be sent
        if ((UCB0STAT & UCNACKIFG) != 0) { // Address NOT acknowledged?
            UCB0CTL1 |= UCTXSTP; // Send Stop condition and finish
        } else { // Address acknowledged: receive
            while (IFG2_bit.UCB0RXIFG == 0) {
            } // Wait for first byte
            Temperature = UCB0RXBUF << 8; // MSB of temperature
            UCB0CTL1 |= UCTXSTP; // Send Stop condition after byte
            while (IFG2_bit.UCB0RXIFG == 0) {
            } // Wait for second byte
            Temperature |= UCB0RXBUF; // LSB of temperature
            DisplayCentiCels (Temperature); // Display temp to 0.01oC
        }
        LED = 0; // Show end of activity
    }
}
//----------------------------------------------------------------------
// ISR for basic timer : return to main routine
//----------------------------------------------------------------------
#pragma vector = BASICTIMER_VECTOR
__interrupt void BASICTIMER_ISR ( void ) // Acknowledged automatically
{
    __low_power_mode_off_on_exit(); // Return to start new I2C message
}