#include "mcc_generated_files/mcc.h"
#include <string.h>

#include "i2c.h"

const uint8_t eeprom_Addr = 0x50;
const uint8_t nextPos[] = {
    0x00,
    0x00,
    0x00,
    0x03
}; // The position of the last good byte
const uint8_t verifyPos[] = {
    0x00,
    0x02,
    0x02
}; // The position of the verify byte

uint8_t logPos[2]; // Save the position of the cursor
uint8_t readPos[2] = {
    0x00,
    0x03
};
uint8_t logStatus[2] = {
    0x00,
    0x00
}; // Save the last status
uint8_t logData[4]; // For sending data
uint8_t readData[4];

void dataLogger_init() {
    // Start status
    i2c_WriteSerial(eeprom_Addr, verifyPos, 2); // Set cursor to verify position
    __delay_ms(10);

    i2c_ReadSerial(eeprom_Addr, logStatus, 1); // Read the byte of verify
    __delay_ms(10);
    // End status

    //    i2c_WriteSerial(eeprom_Addr, nextPos, 4); // Start from the beginning
    __delay_ms(10);

    // Start next byte    
    i2c_WriteSerial(eeprom_Addr, nextPos, 2); // Set cursor to next position
    __delay_ms(10);

    i2c_ReadSerial(eeprom_Addr, logPos, 2); // Read the bytes of next
    __delay_ms(10);
    // End next byte

    // Check if last job went well    
    if ((logStatus[0] != 0x02) && (logStatus[0] != 0x01)) {
        // If it's a new eeprom or the last didn't complete        
        printf("The last status is not 0x01 or 0x02 \r\n");

        i2c_WriteSerial(eeprom_Addr, verifyPos, 3); // Write 0x02 to verify position
        __delay_ms(10);

        i2c_WriteSerial(eeprom_Addr, verifyPos, 2); // Set cursor to verify position
        __delay_ms(10);

        i2c_ReadSerial(eeprom_Addr, logStatus, 1); // Read the byte of verify
        __delay_ms(10);
    } // End of if
} // End of function

void dataLogWrite(uint8_t tempRead[]) {
    // Start set verify to incomplete
    logData[0] = verifyPos[0]; // verifyPos[] MSB
    logData[1] = verifyPos[1]; // verifyPos[] LSB
    logData[2] = logStatus[1]; // 00 / incomplete

    i2c_WriteSerial(eeprom_Addr, logData, 3); // Set verify to incomplete
    __delay_ms(10);
    // End set verify to incomplete

    // Start insert data on next position
    logData[0] = logPos[0]; // logPos[] MSB
    logData[1] = logPos[1]; // logPos[] LSB
    logData[2] = tempRead[0]; // tempRead[] MSB
    logData[3] = tempRead[1]; // tempRead[] LSB

    i2c_WriteSerial(eeprom_Addr, logData, 4); // Insert data on next free position
    __delay_ms(10);
    // End insert data on next position

    // Start update verify status
    logData[0] = verifyPos[0]; // verifyPos[] MSB
    logData[1] = verifyPos[1]; // verifyPos[] LSB

    if (logStatus[0] == 0x01) {
        logStatus[0] = 0x02;
        /* 10 */
    } else {
        logStatus[0] = 0x01;
        /* 01 */
    }

    logData[2] = logStatus[0]; // Status into temp variable

    i2c_WriteSerial(eeprom_Addr, logData, 3); // Update verify status
    __delay_ms(10);
    // End update verify status

    // Start update size
    logData[0] = nextPos[0]; // sizePos[] MSB
    logData[1] = nextPos[1]; // sizePos[] LSB

    if ((logPos[1] + 2) != 0xFF) {
        logPos[1] += 2;
        /* Add 2 to position */
    } else if (((logPos[1] + 2) == 0xFF) && (logPos[0] != 0xFF)) {
        logPos[0] += 1; // Add 1 to position
        logPos[1] = 0x00; // Default start position
    } else if (((logPos[1] + 2) == 0xFF) && (logPos[0] == 0xFF)) {
        logPos[0] = 0x00; // Default start position
        logPos[1] = 0x03; // Default start position
    }

    logData[2] = logPos[0]; // logPos[] MSB
    logData[3] = logPos[1]; // logPos[] LSB

    i2c_WriteSerial(eeprom_Addr, logData, 4); // Update the last good byte position
    __delay_ms(10);
    // End update size
} // End of function

void dataLogRead() {
    i2c_WriteSerial(eeprom_Addr, readPos, 2); // Set cursor to start of data log
    __delay_ms(10);

    if ((readPos[1] + 2) != 0xFF) {
        readPos[1] += 4;
        /* Add 2 to position */
    } else if (((readPos[1] + 2) == 0xFF) && (readPos[0] != 0xFF)) {
        readPos[0] += 1; // Add 1 to position
        readPos[1] = 0x00; // Default start position
    } else if (((readPos[1] + 2) == 0xFF) && (readPos[0] == 0xFF)) {
        readPos[0] = 0x00; // Default start position
        readPos[1] = 0x03; // Default start position
    }

    i2c_ReadSerial(eeprom_Addr, readData, 4); // Read data
    __delay_ms(10);

    float readTemp;
    char readOutT[5];

    readTemp = ((175.72 * ((readData[0] << 8) + readData[1])) / 65536) - 46.85; // Combine 2 8 bit to a 16 bit & calculate temperature

    sprintf(readOutT, "%.1f", readTemp);
    printf("1st Read data: %s \r\n", readOutT); // Print out the temperature decimal from data logger
    __delay_ms(10);

    readTemp = ((175.72 * ((readData[2] << 8) + readData[3])) / 65536) - 46.85; // Combine 2 8 bit to a 16 bit & calculate temperature

    sprintf(readOutT, "%.1f", readTemp);
    printf("2nd Read data: %s \r\n", readOutT); // Print out the temperature decimal from data logger
    __delay_ms(10);
} // End of function
