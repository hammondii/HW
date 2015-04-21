#include<xc.h> // processor SFR definitions
#include "Testlight.h"

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