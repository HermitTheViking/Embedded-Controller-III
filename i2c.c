#include "mcc_generated_files/mcc.h"
#include <string.h>

void i2c_WriteSerial(int addr, uint8_t *data, int length) {
    while ((SSP1STAT & 0x04) || (SSP1CON2 & 0x1F)); // Is the bus ideal

    SSP1CON2bits.SEN = 1;
    while (SSP1CON2bits.SEN == 1); // Start bit

    uint8_t targetAddr = (addr << 1); // Set address to write

    SSP1BUF = targetAddr; // Inset address to buffer
    while (SSP1STATbits.R_nW); // Is the buffer ready

    for (int i = 0; i < length; i++) {
        // Slave is ready
        if (SSP1CON2bits.ACKSTAT == 0) {
            SSP1BUF = (uint8_t) data[i];
            while (SSP1STATbits.R_nW); // Is the buffer ready
        } else {
        } // End of if
    } // End of for

    SSP1CON2bits.PEN = 1;
    while (SSP1CON2bits.PEN == 1); // End bit
} // End of function

void i2c_ReadSerial(int addr, uint8_t *data, int length) {
    while ((SSP1STAT & 0x04) || (SSP1CON2 & 0x1F)); // Is the bus ideal

    SSP1CON2bits.SEN = 1;
    while (SSP1CON2bits.SEN == 1); // Start bit    
    uint8_t targetAddr = (addr << 1) + 1; // Set address to read    
    SSP1BUF = targetAddr; // Inset address to buffer    
    while (SSP1STATbits.R_nW); // Is the buffer ready            
    // Slave is ready
    if (SSP1CON2bits.ACKSTAT == 0) {
        // Acknowledge after Byte ACK / NACK
        for (int i = 0; i < length; i++) {
            SSP1CON2bits.RCEN = 1;
            while (SSP1CON2bits.RCEN == 1); // Read    
            data[i] = SSP1BUF;
            if (i != (length - 1)) {
                SSP1CON2bits.ACKDT = 0; // ACK
            } else {
                SSP1CON2bits.ACKDT = 1; // NACK
            } // End of if else
            SSP1CON2bits.ACKEN = 1;
            while (SSP1CON2bits.ACKEN == 1); // Send acknowledge
        } // End of for

        SSP1CON2bits.PEN = 1;
        while (SSP1CON2bits.PEN == 1); // End bit
    } else {
    } // End of if else
} // End of function
