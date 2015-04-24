/*
 * File:   HW4.c
 * Author: Paul
 *
 * Created on April 12, 2015, 5:37 PM
 */

#include <p32xxxx.h>
#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro
#include "Testlight.h" // Testlight File from HW1
#include "i2c_display.h"
#include "i2c_master_int.h"
#include "accel.h"
#include "OLED.h"

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
volatile unsigned int rowstart=0;
volatile unsigned int colstart=0;


//	Function Prototypes

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

    light_init();
    display_init();	
    acc_setup();

    while(1){

       //Temporary Location for variables/functions until modular functions

        unsigned char buffer [100];

       //Temporary Variables for Reading ACCEL
        short accels[3]; // accelerations for the 3 axes
        short mags[3]; // magnetometer readings for the 3 axes
        short tempt;  // temperature
        //int load, i;
        float loadmax = 10000000;  // not in use right now



        while (1) {
         _CP0_SET_COUNT(0); // set core timer to 0, remember it counts at half the CPU clock

        // wait for half a second, setting LED brightness to pot angle while waiting
            while (_CP0_GET_COUNT() < loadmax) {
                //i = _CP0_GET_COUNT();

                while(_CP0_GET_COUNT()< loadmax-2){
                    _nop();
                }
                acc_read_register(OUT_X_L_A, (unsigned char *) accels, 6);
                // need to read all 6 bytes in one transaction to get an update.

                acc_read_register(OUT_X_L_M, (unsigned char *) mags, 6);
                // read the temperature data. Its a right justified 12 bit two's compliment number

                acc_read_register(TEMP_OUT_L, (unsigned char *) &tempt, 2);
                sprintf(buffer,"Values (X:%d, Y:%d, Z:%d)",accels[0],accels[1],accels[2]);
                display_clear();
                OLED_WRITE(12,0, (unsigned char *) buffer);       
           }
       }
    }
}
