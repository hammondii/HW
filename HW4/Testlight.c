#include<xc.h> // processor SFR definitions
#include "Testlight.h"

void light_int(){

        // INT step 3: This section sets up the timer.  Timer 2 will be enabled at the end.
    TMR2 = 0;					// Initial value for Timer 2 = 0
    PR2 = 39999;				// Set the Period Register
    T2CONbits.TGATE = 0;		// Turns off gated accumulation
    T2CONbits.TCKPS = 000;	// Sets the prescaler to 1
    T2CONbits.T32 = 0;		// Disable 32 bit timer
    T2CONbits.TCS = 0;		// Use PBCLK as clock

    // INT step 3: This section sets up the PWM using OC1.  OC1 to be enabled at the end
    OC1CONbits.OC32 = 0;		// Disable 32 bit Timer use for OC1
    OC1CONbits.OCTSEL = 0; 	// Set Timer 2 as Clock source for Output Compare
    OC1CONbits.OCM = 0b110;	// Disable Fault pin
    OC1RS = 20000;		// Set next duty cycle to 50% (OC1R/PR2)
    OC1R = 20000;		// Set the initial duty cycle to 50% (OC1R/PR2)

    // INT step 3: This section Enables Timer 3 and OC1
    T2CONbits.ON = 1; 	 // Enable Timer 2
    OC1CONbits.ON = 1;	 // Enable Output Compare 1

    // set up USER pin as input

    ANSELBbits.ANSB13 =0;
    TRISBbits.TRISB13 = 0b0001; // set pin B15 to Digital Input User Button

    // set up LED1 pin as a digital output
    LATBbits.LATB7= 1;
    TRISBbits.TRISB7 = 0b0000; // set pin B7 to Digital Output

    // set up LED2 as OC1 using Timer1 at 1kHz

    ANSELBbits.ANSB15 = 0;
    RPB15Rbits.RPB15R = 0b0101; // set pin B15 to Digital Output

    // set up A0 as AN0
    ANSELAbits.ANSA0 = 1;
    AD1CON3bits.ADCS = 3;
    AD1CHSbits.CH0SA = 0;
    AD1CON1bits.ADON = 1;

}

int readADC(void) {
    int elapsed = 0;
    int finishtime = 0;
    int sampletime = 20;
    int a = 0;

    AD1CON1bits.SAMP = 1;
    elapsed = _CP0_GET_COUNT();
    finishtime = elapsed + sampletime;
    while (_CP0_GET_COUNT() < finishtime) {
    }
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE) {
    }
    a = ADC1BUF0;
    return a;
}

void testlight(void){
    while (1) {
     _CP0_SET_COUNT(0); // set core timer to 0, remember it counts at half the CPU clock
    LATBINV = 0x080; // invert a pin B13

    // wait for half a second, setting LED brightness to pot angle while waiting
    while (_CP0_GET_COUNT() < 10000000) {
        int val;
        val = (readADC());
        OC1RS =  (val) * (PR2/1024);

        if (PORTBbits.RB13 == 1) {
            ;// nothing
        } else {
            LATBINV = 0x80;  //invert pin B7
        }
    }
  }
}