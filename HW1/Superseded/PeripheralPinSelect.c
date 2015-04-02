
//Analog (ANx) pins default to be analog input. Use ANSELA or ANSELB to make them digital
  ANSELBbits.ANSB13 = 0; // 0 for digital, 1 for analog

//For an input function, use the (funtion name)Rbits.(function name) to set the pin, using table 12-1 in Chapter 12
  U1RXRbits.U1RXR = 0b0000; // set U1RX to pin A2

//For an output function, use the RP(pin name)Rbits.RP(pin name)R to set the pin, using table 12-2 in Chapter 12
  RPB15Rbits.RPB15R = 0b0001; // set B15 to U1TX
