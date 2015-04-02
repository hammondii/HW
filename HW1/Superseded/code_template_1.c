#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro

// DEVCFGs here
		//These are the DEVCFG bits for the NU32 in standalone mode:
		#pragma config DEBUG = OFF          // Background Debugger disabled
		#pragma config FPLLMUL = MUL_20     // PLL Multiplier: Multiply by 20
		#pragma config FPLLIDIV = DIV_2     // PLL Input Divider:  Divide by 2
		#pragma config FPLLODIV = DIV_1     // PLL Output Divider: Divide by 1
		#pragma config FWDTEN = OFF         // WD timer: OFF
		#pragma config POSCMOD = HS         // Primary Oscillator Mode: High Speed xtal
		#pragma config FNOSC = PRIPLL       // Oscillator Selection: Primary oscillator w/ PLL
		#pragma config FPBDIV = DIV_1       // Peripheral Bus Clock: Divide by 1
		#pragma config BWP = OFF            // Boot write protect: OFF
		#pragma config ICESEL = ICS_PGx2    // ICE pins configured on PGx2, Boot write protect OFF.
		#pragma config FSOSCEN = OFF        // Disable second osc to get pins back
		#pragma config FSRSSEL = PRIORITY_7 // Shadow Register Set for interrupt priority 7


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
		#pragma config POSCMOD = EC // high speed crystal mode
		#pragma config OSCIOFNC = OFF // free up secondary osc pins
		#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
		#pragma config FCKSM = CCSDCMD // do not enable clock switch
		#pragma config WDTPS = PS1048576 // slowest wdt
		#pragma config WINDIS = OFF // no wdt window
		#pragma config FWDTEN = OFF // wdt off by default
		#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

		// DEVCFG2 - get the CPU clock to 40MHz
		#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
		#pragma config FPLLMUL = MUL_20 // multiply clock after FPLLIDIV
		#pragma config UPLLIDIV = DIV_2 // divide clock after FPLLMUL
		#pragma config UPLLEN = ON // USB clock on
		#pragma config FPLLODIV = DIV_1 // divide clock by 1 to output on pin

		// DEVCFG3
		#pragma config USERID = 0 // some 16bit userid
		#pragma config PMDL1WAY = OFF // not multiple reconfiguration, check this
		#pragma config IOL1WAY = OFF // not multimple reconfiguration, check this
		#pragma config FUSBIDIO = ON // USB pins controlled by USB module
		#pragma config FVBUSONIO = ON // controlled by USB module


int readADC(void);

int main() {

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

    // set up USER pin as input

		//For an input function, use the (function name)Rbits.(function name) to set the pin, using table 12-1 in Chapter 12
  		  ANSELBbits.ANB13 =0;
		  TRISBbits.TRISB13 = 0b0001; // set pin B15 to Digital Input User Button

	// set up LED1 pin as a digital output
		  LATBbits.LATB7= 1;
		  TRISBbits.TRISB7 = 0b0001; // set pin B7 to Digital Input
		  
    // set up LED2 as OC4 using Timer4 at 1kHz 

		//For an output function, use the RP(pin name)Rbits.RP(pin name)R to set the pin, using table 12-2 in Chapter 12
		  ANSELBbits.ANB13 = 0
		  RPB15Rbits.RPB13R = 0b0101; // set pin B15 to Digital Output
	
    // set up A0 as AN0
			ANSELAbits.ANSA0 = 1;
			AD1CON3bits.ADCS = 3;
			AD1CHSbits.CH0SA = 0;
			AD1CON1bits.ADON = 1;

			
			
			
    while (1) {
        ;
		;// invert pin every 0.5s, set PWM duty cycle % to the pot voltage output %
    }
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
