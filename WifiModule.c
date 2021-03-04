#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdio.h>

void wifiModule_init() {
    printf("AT+CIPRECVMODE=0\r\n");
    __delay_ms(1000); // Delay 1 seconds
    printf("AT+CIPSERVER=0\r\n");
    __delay_ms(1000); // Delay 1 seconds
    printf("AT\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function

void SetApMode() {
    printf("AT+CWMODE_CUR=1\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function

void SetupDhcp() {
    printf("AT+CWDHCP_CUR=1,1\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function

void ConnToAp() {
    printf("AT+CWJAP_CUR=\"WuggaNet\",\"FredagsBanan\"\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function

void SetMaxConn() {
    printf("AT+CIPSERVERMAXCONN=1\r\n");
    __delay_ms(1000); // Delay 1 seconds
    printf("AT+CIPMUX=1\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function

void SetupServer() {
    printf("AT+CIPSERVER=1,80\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function

void Getip() {
    printf("AT+CIFSR\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function
