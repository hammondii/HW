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
#include "pic32config.h"

//	Function Prototypes
int main(void);
int readADC(void);



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
