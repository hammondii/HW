/*
 * File:   HW4.c
 * Author: Paul
 *
 * Created on April 12, 2015, 5:37 PM
 *
 * Current State May 9, 2015
 * The OLED_Write function when doing sprintf will return what's in
 * accel_read_print() in the accel_snake() function.
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

#define WAIT 10000000

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

//    _CP0_SET_COUNT(0); // set core timer to 0, remember it counts at half the CPU clock
//    while (_CP0_GET_COUNT() < WAIT) _nop();

    light_init();
    display_init();
    acc_setup();

    while (1) {
        accel_read_print();
        //accel_read_snake();
    }
}

