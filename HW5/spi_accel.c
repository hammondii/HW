#include "accel.h"
#include <xc.h>
#include "OLED.h"

#define LOADMAX 10000000


// interface with the LSM303D accelerometer/magnetometer using spi
// Wire GND to GND, VDD to 3.3V, 
// SDO1 (A4)            -> SDI (labeled SDA),
// SDI1 (A1)            -> SDO
// SCK1 (B14)           -> SCL
// some digital pin(B4) -> CS

#define CS LATBbits.LATB4 // replace x with some digital pin

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}


// read data from the accelerometer, given the starting register address.
// return the data in data
void acc_read_register(unsigned char reg, unsigned char data[], unsigned int len) {
  unsigned int i;
  reg |= 0x80; // set the read bit (as per the accelerometer's protocol)
  if(len > 1) {
    reg |= 0x40; // set the address auto increment bit (as per the accelerometer's protocol)
  }
  CS = 0;
  spi_io(reg);
  for(i = 0; i != len; ++i) {
    data[i] = spi_io(0); // read data from spi
  }
  CS = 1;
}


void acc_write_register(unsigned char reg, unsigned char data) {
  CS = 0;               // bring CS low to activate SPI
  spi_io(reg);
  spi_io(data);
  CS = 1;               // complete the command
}


void acc_setup() {
  TRISBbits.TRISB4 = 0; // set CS to output and digital if necessary
  CS = 1;

  // select a pin for SDI1
  SDI1Rbits.SDI1R = 0b0;
  ANSELAbits.ANSA1 = 0b0;
  
  // select a pin for SD01
  RPA4Rbits.RPA4R = 0b0011;

  //Turn off
  
  // Setup the master Master - SPI1
  // we manually control SS as a digital output 
  // since the pic is just starting, we know that spi is off. We rely on defaults here
 
  // setup spi1
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 0x3;            // baud rate to 5MHz [SPI1BRG = (40000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from active to inactive
                            //    (high to low since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi
 
  // set the accelerometer data rate to 1600 Hz. Do not update until we read values
  acc_write_register(CTRL1, 0xAF);
  acc_write_register(CTRL2, 0x00);

  // 50 Hz magnetometer, high resolution, temperature sensor on
  acc_write_register(CTRL5, 0xF0); 

  // enable continuous reading of the magnetometer
  acc_write_register(CTRL7, 0x0); 
}

void accel_read_print(){
    static unsigned char bufferaccel [100];
    //char *pbufferaccel = &bufferaccel;
   //Temporary Variables for Reading ACCEL
    short accels[3]; // accelerations for the 3 axes
    short mags[3]; // magnetometer readings for the 3 axes
    short tempt;  // temperature
    //int load =0;

     _CP0_SET_COUNT(0); // set core timer to 0, remember it counts at half the CPU clock

    while(_CP0_GET_COUNT()< LOADMAX){
        _nop();
    }
    acc_read_register(OUT_X_L_A, (unsigned char *) accels, 6);
    // need to read all 6 bytes in one transaction to get an update.

    acc_read_register(OUT_X_L_M, (unsigned char *) mags, 6);
    // read the temperature data. Its a right justified 12 bit two's compliment number

    acc_read_register(TEMP_OUT_L, (unsigned char *) &tempt, 2);
    sprintf(bufferaccel,"Values (X:%d, Y:%d, Z:%d)",accels[0],accels[1],accels[2]);
    display_clear();
    OLED_WRITE(0,0, bufferaccel);
//    OLED_WRITE(0,0, (unsigned char *) bufferaccel);
}

void accel_read_snake(){
   static unsigned char accsnake [100];
   //char *pbuffaccsnake = buffaccsnake;
   //Temporary Variables for Reading ACCEL
    short accels[3]; // accelerations for the 3 axes
    short mags[3]; // magnetometer readings for the 3 axes
    short tempt;  // temperature
    //int load =0;

     _CP0_SET_COUNT(0); // set core timer to 0, remember it counts at half the CPU clock

    while(_CP0_GET_COUNT()< LOADMAX){
        _nop();
    }
    acc_read_register(OUT_X_L_A, (unsigned char *) accels, 6);
    // need to read all 6 bytes in one transaction to get an update.

    acc_read_register(OUT_X_L_M, (unsigned char *) mags, 6);
    // read the temperature data. Its a right justified 12 bit two's compliment number

    acc_read_register(TEMP_OUT_L, (unsigned char *) &tempt, 2);
    OLED_SNAKE(accels);
    
}