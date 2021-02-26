#include "mcc_generated_files/mcc.h"
#include <string.h>
#include "i2c.h"

const uint8_t adafruit_Addr = 0x40;
const uint8_t tempText[] = "@Temp:";
const uint8_t humiText[] = "@Humi:";
const uint8_t degree_sign[] = {
    0x80, 0x40, 0x40,
    0b00110,
    0b01001,
    0b01001,
    0b00110,
    0b00000,
    0b00000,
    0b00000,
    0b00000
};
const uint8_t procent_sign[] = {
    0x80, 0x48, 0x40,
    0b11000,
    0b11001,
    0b00010,
    0b00100,
    0b01000,
    0b10011,
    0b00011,
    0b00000
};
const uint8_t adafruit_humi_command[] = {0xE5};
const uint8_t adafruit_temp_command[] = {0xE3};

int8_t tempRead[2];
uint8_t humiRead[2];

void adafruit(uint8_t displayAddr, uint8_t displayLine[], uint8_t clearDisplay[], uint8_t costumChar[]) {
    
    i2c_WriteSerial(displayAddr, clearDisplay, 2); // Clear display

    // Start get humidity and calculate
    i2c_WriteSerial(adafruit_Addr, adafruit_humi_command, 1); // Set Adafruit to humidity
    i2c_ReadSerial(adafruit_Addr, humiRead, 2); // Get humidity from Adafruit

    float rh_code = ((humiRead[0] << 8) + humiRead[1]); // Combine 2 8 bit to a 16 bit
    float rh = ((125 * rh_code) / 65536) - 6; // Calculate humidity
    // End get humidity and calculate

    // Start write humidity to display
    displayLine[1] = 0x80; // Set cursor hex for 1 position
    i2c_WriteSerial(displayAddr, displayLine, 2); // Set cursor to 1 position first line
    i2c_WriteSerial(displayAddr, humiText, 6); // Write to display

    displayLine[1] = 0x86; // Set cursor hex for 6 position
    i2c_WriteSerial(displayAddr, displayLine, 2); // Set cursor to 6 position first line

    char outH[5];
    sprintf(outH, "@%.1f", rh);

    i2c_WriteSerial(displayAddr, outH, sizeof (outH)); // Write to display

    costumChar[1] = 0x01;
    i2c_WriteSerial(displayAddr, costumChar, 2); // Percent sign        
    // End write humidity to display

    // Start get temperature and calculate
    i2c_WriteSerial(adafruit_Addr, adafruit_temp_command, 1); // Set Adafruit to temperature
    i2c_ReadSerial(adafruit_Addr, tempRead, 2); // Get temperature from Adafruit

    float temp = ((175.72 * ((tempRead[0] << 8) + tempRead[1])) / 65536) - 46.85; // Combine 2 8 bit to a 16 bit & calculate temperature
    // End get temperature and calculate

    // Start write temperature to display
    displayLine[1] = 0xC0; // Set cursor hex for 1 position
    i2c_WriteSerial(displayAddr, displayLine, 2); // Set cursor to 1 position second line
    i2c_WriteSerial(displayAddr, tempText, 6); // Write to display

    displayLine[1] = 0xC6; // Set cursor hex for 6 position
    i2c_WriteSerial(displayAddr, displayLine, 2); // Set cursor to 6 position second line

    char outT[5];
    sprintf(outT, "@%.1f", temp);

    i2c_WriteSerial(displayAddr, outT, sizeof (outT)); // Write to display

    costumChar[1] = 0x00;
    i2c_WriteSerial(displayAddr, costumChar, 2); // Degree sign
    // End write temperature to display
} // End of function
