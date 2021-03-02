#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define Display
//#define Adafruit
//#define Datalogger
#define WifiModule

#include "i2c.h"
#ifdef Display
void i2c_display_init();
#endif
#ifdef Adafruit
#include "Adafruit.h"
void adafruit_init();
void adafruit();
#endif
#ifdef Datalogger
#include "Datalogger.h"
#endif
#ifdef WifiModule
#include "StateMachine.h"
#include "WifiModule.h"
#endif

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

uint8_t index = 0;
char stringArry[64];

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
#endif
#ifdef WifiModule
char oldStringArry[64];
bool wifiConnected = false;
bool gotIp = false;
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
    wifiModule_init();
#endif

    while (1) {
        if (EUSART1_is_rx_ready) {
            uint8_t tmp = EUSART1_Read();
            if ((tmp == 10 || tmp == 13) && (index <= 64)) {
                index = 0;

#ifdef WifiModule
                if (!strncmp(stringArry, "OK", 2)) {
                    i2c_WriteSerial(displayAddr, clearDisplay, 2); // Clear display

                    displayLine[1] = 0x86; // Set cursor hex for 1 position
                    i2c_WriteSerial(displayAddr, displayLine, 2); // Set cursor to 6 position second line

                    if (!strncmp(oldStringArry, "AT+CWMODE_CUR=3", 15)) {
                        i2c_WriteSerial(displayAddr, "CWMODE", 6); // Write to display
                        StateMachine_RunIteration(&stateMachine, EV_MODEOK);
                    } else if (!strncmp(oldStringArry, "AT+CWDHCP_CUR=1,1", 17)) {
                        i2c_WriteSerial(displayAddr, "CWDHCP", 6); // Write to display
                        StateMachine_RunIteration(&stateMachine, EV_DHCPOK);
                    } else if (!strncmp(oldStringArry, "AT+CWJAP_CUR=\"WuggaNet\",\"FredagsBanan\"", 31)) {
                        i2c_WriteSerial(displayAddr, "CWJAP", 5); // Write to display       
                        StateMachine_RunIteration(&stateMachine, EV_CONNOK); //TODO: Make fix for no control connected or IP
                    } else if (!strncmp(oldStringArry, "AT+CIPMUX=1", 11)) {
                        i2c_WriteSerial(displayAddr, "CIPMUX", 6); // Write to display       
                        StateMachine_RunIteration(&stateMachine, EV_MAXCONNOK);
                    } else if (!strncmp(oldStringArry, "AT+CIPSERVER=1,80", 17)) {
                        i2c_WriteSerial(displayAddr, "CIPSERVER", 9); // Write to display       
                        StateMachine_RunIteration(&stateMachine, EV_SERVEROK);
                    } else if (!strncmp(oldStringArry, "AT", 2)) {
                        i2c_WriteSerial(displayAddr, "AT", 2); // Write to display       
                        StateMachine_RunIteration(&stateMachine, EV_INITOK);
                    }
                } else if (!strncmp(stringArry, "WIFI DISCONNECT", 15)) {
                    wifiConnected = false;
                    gotIp = false;
                } else if (!strncmp(stringArry, "WIFI CONNECTED", 14)) {
                    wifiConnected = true;
                } else if (!strncmp(stringArry, "WIFI GOT IP", 11)) {
                    gotIp = true;
                } else if (!strncmp(stringArry, "no change", 9)) { }
                else if (strncmp(stringArry, "", 2)) {
                    strcpy(oldStringArry, stringArry);
                } else if (!strncmp(stringArry, "", 2)) { }
#endif

#ifdef Datalogger
                if (!strncmp(stringArry, "start", 4)) {
                    printf("Start timer \r\n");
                    TMR4_StartTimer();
                } else if (!strncmp(stringArry, "stop", 4)) {
                    printf("Stop timer \r\n");
                    TMR4_StopTimer();
                } else {
                    printf("%s, did not match a command \n\r", stringArry);
                }
#endif
                memset(stringArry, 0, sizeof (stringArry));
            } else if (index >= 65) {
                index--;
            } else {
                stringArry[index] = tmp;
                index++;
                stringArry[index] = 0x00;
            }
        }
    } // End of while
} // End of function

#ifdef Display
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
#endif

#ifdef Adafruit
void adafruit_init() {
    i2c_WriteSerial(displayAddr, procent_sign, 11); // Write custom charator to display ram
    i2c_WriteSerial(displayAddr, degree_sign, 11); // Write custom charator to display ram
} // End of function

void adafruit() {
    i2c_WriteSerial(displayAddr, clearDisplay, 2); // Clear display

    // Start write humidity to display
    displayLine[1] = 0x80; // Set cursor hex for 1 position
    i2c_WriteSerial(displayAddr, displayLine, 2); // Set cursor to 1 position first line
    i2c_WriteSerial(displayAddr, humiText, 6); // Write to display

    displayLine[1] = 0x86; // Set cursor hex for 6 position
    i2c_WriteSerial(displayAddr, displayLine, 2); // Set cursor to 6 position first line

    char outH[5];
    sprintf(outH, "@%.1f", GetHumi());
    i2c_WriteSerial(displayAddr, outH, sizeof (outH)); // Write to display

    costumChar[1] = 0x01;
    i2c_WriteSerial(displayAddr, costumChar, 2); // Percent sign        
    // End write humidity to display

    // Start write temperature to display
    displayLine[1] = 0xC0; // Set cursor hex for 1 position
    i2c_WriteSerial(displayAddr, displayLine, 2); // Set cursor to 1 position second line
    i2c_WriteSerial(displayAddr, tempText, 6); // Write to display

    displayLine[1] = 0xC6; // Set cursor hex for 6 position
    i2c_WriteSerial(displayAddr, displayLine, 2); // Set cursor to 6 position second line

    char outT[5];
    sprintf(outT, "@%.1f", GetTemp());
    i2c_WriteSerial(displayAddr, outT, sizeof (outT)); // Write to display

    costumChar[1] = 0x00;
    i2c_WriteSerial(displayAddr, costumChar, 2); // Degree sign
    // End write temperature to display
} // End of function
#endif
// End of File