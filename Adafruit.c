#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdio.h>
#include "i2c.h"

const uint8_t adafruit_Addr = 0x40;
const uint8_t adafruit_humi_command[] = {0xE5};
const uint8_t adafruit_temp_command[] = {0xE3};

uint8_t tempRead[2];
uint8_t humiRead[2];

float GetTemp() {
    // Start get temperature and calculate
    i2c_WriteSerial(adafruit_Addr, adafruit_temp_command, 1); // Set Adafruit to temperature
    i2c_ReadSerial(adafruit_Addr, tempRead, 2); // Get temperature from Adafruit

    float temp = ((175.72 * ((tempRead[0] << 8) + tempRead[1])) / 65536) - 46.85; // Combine 2 8 bit to a 16 bit & calculate temperature
    // End get temperature and calculate
    
    return temp;
} // End of function

float GetHumi() {
    // Start get humidity and calculate
    i2c_WriteSerial(adafruit_Addr, adafruit_humi_command, 1); // Set Adafruit to humidity
    i2c_ReadSerial(adafruit_Addr, humiRead, 2); // Get humidity from Adafruit

    float rh_code = ((humiRead[0] << 8) + humiRead[1]); // Combine 2 8 bit to a 16 bit
    float rh = ((125 * rh_code) / 65536) - 6; // Calculate humidity
    // End get humidity and calculate
    
    return rh;
} // End of function
