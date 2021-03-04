#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define Display
#define Adafruit
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
char ipStringArry[64];
bool wifiConnected = false;
bool gotIp = false;
const uint8_t htmlText[] = "<!DOCTYPE html><html><head><title>Page Title</title></head><body><h1>This is a Heading</h1><p>This is a paragraph.</p></body></html>";
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
#ifdef WifiModule
    wifiModule_init();
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
    TMR2_StopTimer();
    stateMachine_t stateMachine;
    StateMachine_Init(&stateMachine);
#endif

    i2c_WriteSerial(displayAddr, clearDisplay, 2); // Clear display
    __delay_ms(10); // Delay 10 milli seconds

    while (1) {
        if (EUSART1_is_rx_ready) {
            uint8_t tmp = EUSART1_Read();
            if ((tmp == 10 || tmp == 13) && (index <= 64)) {
                index = 0;
#ifdef WifiModule
                if (!strncmp(stringArry, "OK", 2)) {
                    displayLine[1] = 0x80; // Set cursor hex for 6 position
                    i2c_WriteSerial(displayAddr, displayLine, 2); // Set cursor to 6 position first line        
                    if (!strncmp(oldStringArry, "AT+CWMODE_CUR=", 14) && !wifiConnected && !gotIp) {
                        i2c_WriteSerial(displayAddr, "@CWMODE", sizeof ("@CWMODE")); // Write to display
                        StateMachine_RunIteration(&stateMachine, EV_MODEOK);
                    } else if (!strncmp(oldStringArry, "AT+CWDHCP_CUR=", 14) && !wifiConnected && !gotIp) {
                        i2c_WriteSerial(displayAddr, "@CWDHCP", sizeof ("@CWDHCP")); // Write to display
                        StateMachine_RunIteration(&stateMachine, EV_DHCPOK);
                    } else if (!strncmp(oldStringArry, "AT+CWJAP_CUR=", 13) && wifiConnected && gotIp) {
                        i2c_WriteSerial(displayAddr, "@CWJAP", sizeof ("@CWJAP")); // Write to display
                        StateMachine_RunIteration(&stateMachine, EV_CONNOK); //TODO: Make fix for no control connected or IP
                    } else if (!strncmp(oldStringArry, "AT+CIPMUX=", 10) && wifiConnected && gotIp) {
                        i2c_WriteSerial(displayAddr, "@CIPMUX", sizeof ("@CIPMUX")); // Write to display
                        StateMachine_RunIteration(&stateMachine, EV_MAXCONNOK);
                    } else if (!strncmp(oldStringArry, "AT+CIPSERVER=", 13) && wifiConnected && gotIp) {
                        i2c_WriteSerial(displayAddr, "@CIPSERVER", sizeof ("@CIPSERVER")); // Write to display
                        StateMachine_RunIteration(&stateMachine, EV_SERVEROK);
                    } else if (!strncmp(oldStringArry, "AT+CIFSR", 8) && wifiConnected && gotIp) {
                        i2c_WriteSerial(displayAddr, "@CIFSR", sizeof ("@CIFSR")); // Write to display
                        StateMachine_RunIteration(&stateMachine, EV_GETIPOK);
                    } else if (!strncmp(oldStringArry, "AT", 2) && !wifiConnected && !gotIp) {
                        i2c_WriteSerial(displayAddr, "@AT", sizeof ("@AT")); // Write to display
                        StateMachine_RunIteration(&stateMachine, EV_INITOK);
                    }
                } else if (!strncmp(stringArry, "AT", 2)
                        || !strncmp(stringArry, "AT+CWMODE_CUR", 13)
                        || !strncmp(stringArry, "AT+CWDHCP_CUR", 13)
                        || !strncmp(stringArry, "AT+CWJAP_CUR", 12)
                        || !strncmp(stringArry, "AT+CIPMUX", 9)
                        || !strncmp(stringArry, "AT+CIPSERVER", 12)
                        || !strncmp(stringArry, "AT+CIFSR", 8)) {
                    strcpy(oldStringArry, stringArry);
                }

                if (!strncmp(stringArry, "+CIFSR:STAIP", 12)) {
                    displayLine[1] = 0xC0; // Set cursor hex for 1 position
                    i2c_WriteSerial(displayAddr, displayLine, 2); // Set cursor to 1 position second line
                    i2c_WriteSerial(displayAddr, "@Hello ip", sizeof ("@Hello ip")); // Write to display
                    //                    strncpy(ipStringArry, stringArry + 14, 15);
                    //                    ipStringArry[15] = '\0';
                }

                if (!strncmp(stringArry, "+IPD,0,464:GET", 14) && wifiConnected && gotIp) {
//                    int counter;
//                    while (htmlText[counter] != '\0') {
//                        if (htmlText[counter] == 'a') {
//                            htmlText[counter] = '*';
//                        }
//                        counter++;
//                    }

                    printf("AT+CIPSEND=0,%d\r\n", sizeof (htmlText));
                    __delay_ms(1000); // Delay 1 seconds
                    printf("%s\n", htmlText);
                    __delay_ms(1000); // Delay 1 seconds
                }

                if (!strncmp(stringArry, "WIFI DISCONNECT", 15)) {
                    wifiConnected = false;
                    gotIp = false;
                } else if (!strncmp(stringArry, "WIFI CONNECTED", 14)) {
                    wifiConnected = true;
                    gotIp = false;
                }

                if (!strncmp(stringArry, "WIFI GOT IP", 11)) {
                    gotIp = true;
                }
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
    __delay_ms(10); // Delay 10 milli seconds

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