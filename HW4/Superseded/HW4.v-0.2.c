/*
 * File:   HW4.c
 * Author: Paul
 *
 * Created on April 12, 2015, 5:37 PM
 */

/******************************************************************************
 * Software License Agreement
 *
 * Copyright � 2011 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED ìAS ISî WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *
 *****************************************************************************/


#include <p32xxxx.h>
#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro
#include "Testlight.h" // Testlight File from HW1
#include "i2c_display.h"
#include "i2c_master_int.h"


// DEVCFGs here

    //These are the available DEVCFG bits for the PIC32MX250F128B are listed in the documentation that comes with XC32, in microchip/xc32/v1.33/docs/config_docs/32mx250f128b.html

    // DEVCFG0
    #pragma config DEBUG = OFF // no debugging
    #pragma config JTAGEN = OFF // no jtag
    #pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
    #pragma config PWP = OFF // no write protect
    #pragma config BWP = OFF // not boot write protect
    #pragma config CP = OFF // no code protect

    // DEVCFG1
    #pragma config FNOSC = PRIPLL // use primary oscillator with pll
    #pragma config FSOSCEN = OFF // turn off secondary oscillator
    #pragma config IESO = OFF // no switching clocks
    #pragma config POSCMOD = HS // high speed crystal mode
    #pragma config OSCIOFNC = OFF // free up secondary osc pins
    #pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
    #pragma config FCKSM = CSDCMD // do not enable clock switch
    #pragma config WDTPS = PS1048576 // slowest wdt
    #pragma config WINDIS = OFF // no wdt window
    #pragma config FWDTEN = OFF // wdt off by default
    #pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

    // DEVCFG2 - get the CPU clock to 40MHz
    #pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
    #pragma config FPLLMUL = MUL_20 // multiply clock after FPLLIDIV
    #pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 40MHz
    #pragma config UPLLIDIV = DIV_2 // divide 8MHz input clock, then multiply by 12 to get 48MHz for USB
    #pragma config UPLLEN = ON // USB clock on

    // DEVCFG3
    #pragma config USERID = 0 // some 16bit userid
    #pragma config PMDL1WAY = ON // not multiple reconfiguration, check this
    #pragma config IOL1WAY = ON // not multimple reconfiguration, check this
    #pragma config FUSBIDIO = ON // USB pins controlled by USB module
    #pragma config FVBUSONIO = ON // controlled by USB module


//	Function Prototypes
int main(void);
int readADC(void);

int main(void) {

    // startup

    //Startup code to run as fast as possible and get pins back from bad defaults

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that
    // kseg0 is cacheable (0x3) or uncacheable (0x2)
    // see Chapter 2 "CPU for Devices with M4K Core"
    // of the PIC32 reference manual
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // no cache on this chip!

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to be able to use TDI, TDO, TCK, TMS as digital
    DDPCONbits.JTAGEN = 0;

    __builtin_enable_interrupts();

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

/*   This section is for the HW1 Testlight.  It will blink the LED1 while
 * LED2 stays constant.
 *  while(1){
        testlight();
    }*/

    
    while(1){

        display_init();
        int row, col;
        display_pixel_set(0,0,1);
        display_pixel_set(0,127,1);
        display_pixel_set(63,127,1);
        display_pixel_set(63,0,1);
        for(col=0;col< WIDTH; col++){
                display_pixel_set(0,col,1);
                display_draw();
            }
        display_draw();
        for(row=15;row< 19;row++){
            for(col=30;col< 90; col++){
                display_pixel_set(row,col,1);
                display_draw();
            }
            col=30;
        }
       
        //for(col = 0; col < WIDTH; ++col) { // draw a diagonal line
        //row = 2*col % HEIGHT; // when we hit the last row
        //display_pixel_set(row,col,1); // start from row 0, but keep advancing
        // the column
        display_draw(); // we draw every update, to display progress.
       // }
        testlight();
       // display_draw();
    }
}



