#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdio.h>

#include "StateMachine.h"
#include "wifiModule.h"
#include "i2c.h"
#include "Datalogger.h"

#define Display
#define Adafruit
//#define Datalogger
#define WifiModule

/*
 * Adafruit is being called every 1 second
 * 
 * 1. The data that is being store is the 2nd temperature bytes.
 * 2. The first 2 bytes is for remembering where the next data is to be written to.
 * 3. ??
 * 4/5. The 3rd byte is for remembering if the last bytes written is okay. If the last byte was not okay it will be over written.
 * 6. When it get to the end of the possible positions it will return to the start 0x03.
 * 7. Datalogger write function is being called every 3rd second. Datalogger read function is being called every 12th second.
 * 8. Start reading from the start (0x03) and increment by 4 every time. It takes 4 bytes every time and converts them to temperature decimal and prints them out.
 */

void i2c_display_init();
void adafruit_init();
void adafruit();

uint8_t index = 0;
char stringArry[64];
char oldStringArry[64];

#ifdef Display
const uint8_t displayAddr = 0x3C;
const uint8_t displayInit[] = {
    0x00,
    0x38,
    0x0F,
    0x06
};
const uint8_t clearDisplay[] = {
    0x00,
    0x01
};
uint8_t displayLine[2] = {
    0x00,
    0x00
};
uint8_t costumChar[2] = {
    0x40,
    0x00
};
#endif
#ifdef Adafruit
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
const uint8_t tempText[] = "@Temp:";
const uint8_t humiText[] = "@Humi:";

const uint8_t adafruit_Addr = 0x40;
const uint8_t adafruit_humi_command[] = {0xE5};
const uint8_t adafruit_temp_command[] = {0xE3};

int8_t tempRead[2];
uint8_t humiRead[2];
#endif

//Main application

void main(void) {
    SYSTEM_Initialize(); // Initialize the device

    INTERRUPT_GlobalInterruptEnable(); // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable(); // Disable the Global Interrupts
    INTERRUPT_PeripheralInterruptEnable(); // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable(); // Disable the Peripheral Interrupts

    EUSART1_Initialize();

#ifdef Display
    i2c_display_init(); // Initialize display with i2c
#endif    
#ifdef Adafruit
    TMR2_Initialize();
    adafruit_init(); // Initialize adafruit
#endif    
#ifdef Datalogger
    TMR4_Initialize();
    TMR6_Initialize();
    dataLogger_init(); // Initialize data logger
#endif

    __delay_ms(3000); // Delay 3 seconds

#ifdef Adafruit
    TMR2_SetInterruptHandler(adafruit);
#endif
#ifdef Datalogger
    TMR4_SetInterruptHandler(dataLogRead);
    TMR6_SetInterruptHandler(dataLogWrite);
    TMR4_StopTimer();
#endif    
#ifdef WifiModule
    stateMachine_t stateMachine;
    StateMachine_Init(&stateMachine);
#endif

    wifiModule_init();
    
    while (1) {
        if (EUSART1_is_rx_ready) {
            uint8_t tmp = EUSART1_Read();

#ifdef WifiModule
            if ((tmp == 10 || tmp == 13) && (index <= 64)) {
                index = 0;

                if (!strncmp(stringArry, "OK", 2)) {
                    if (!strncmp(oldStringArry, "AT+CWMODE_CUR=3", 15)) {
                        StateMachine_RunIteration(&stateMachine, EV_MODEOK);
                    } else if (!strncmp(oldStringArry, "AT+CWDHCP_CUR=1,1", 17)) {
                        StateMachine_RunIteration(&stateMachine, EV_DHCPOK);
                    } else if (!strncmp(oldStringArry, "AT+CWJAP_CUR=\"dummy\",\"GtLDPU43\"", 31)) {
                        StateMachine_RunIteration(&stateMachine, EV_CONNOK); //TODO: Make fix for no control connected or IP
                    } else if (!strncmp(oldStringArry, "AT+CIPMUX=1", 11)) {
                        StateMachine_RunIteration(&stateMachine, EV_MAXCONNOK);
                    } else if (!strncmp(oldStringArry, "AT+CIPSERVER=1,8080", 19)) {
                        StateMachine_RunIteration(&stateMachine, EV_SERVEROK);
                    } else if (!strncmp(oldStringArry, "AT", 2)) {
                        StateMachine_RunIteration(&stateMachine, EV_INITOK);
                    }
                } else if (!strncmp(stringArry, "WIFI DISCONNECT", 15)) { }
                else if (!strncmp(stringArry, "WIFI CONNECTED", 14)) { }
                else if (!strncmp(stringArry, "WIFI GOT IP", 11)) { }
                else if (strncmp(stringArry, "", 2)) {
                    strcpy(oldStringArry, stringArry);
                } else if (!strncmp(stringArry, "", 2)) { }
                else {
                    printf("Hello stringArry, %s \r\n", stringArry);
                }

                memset(stringArry, 0, sizeof (stringArry));
            } else if (index >= 65) {
                index--;
            } else {
                stringArry[index] = tmp;
                index++;
            }
#endif

#ifdef Datalogger
            if ((tmp == 10 || tmp == 13) && (index <= 64)) {
                index = 0;

                if (!strncmp(stringArry, "start", 4)) {
                    printf("Start timer \r\n");
                    TMR4_StartTimer();
                } else if (!strncmp(stringArry, "stop", 4)) {
                    printf("Stop timer \r\n");
                    TMR4_StopTimer();
                } else {
                    printf("%s, did not match a command \n\r", stringArry);
                }

                memset(stringArry, 0, sizeof (stringArry));
            } else if (index >= 65) {
                index--;
            } else {
                stringArry[index] = tmp;
                index++;
                stringArry[index] = 0x00;
            }
#endif
        }
    } // End of while
} // End of function

void i2c_display_init() {
    TRISCbits.RC3 = 1;
    TRISCbits.RC4 = 1;

    ANSELCbits.ANSC3 = 0;
    ANSELCbits.ANSC4 = 0;

    uint8_t i2c_clock = (_XTAL_FREQ / (4 * 100000) - 1);

    SSP1ADD = i2c_clock;
    SSP1CON1 = 0x28;
    SSP1CON2 = 0;

    i2c_WriteSerial(displayAddr, displayInit, 4);
} // End of function

void adafruit_init() {
    i2c_WriteSerial(displayAddr, procent_sign, 11); // Write custom charator to display ram
    i2c_WriteSerial(displayAddr, degree_sign, 11); // Write custom charator to display ram
} // End of function

void adafruit() {
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
// End of File