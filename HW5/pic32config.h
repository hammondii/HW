/* 
 * File:   pic32config.h
 * Author: Paul
 *
 * Created on April 24, 2015, 3:22 PM
 */

#ifndef PIC32CONFIG_H
#define	PIC32CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

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
    #pragma config FUSBIDIO = OFF // USB pins controlled by USB module
    #pragma config FVBUSONIO = OFF // controlled by USB module



#ifdef	__cplusplus
}
#endif

#endif	/* PIC32CONFIG_H */

