#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdio.h>

void wifiModule_init() {
    printf("AT+CIPSERVER=0\r\n");
    __delay_ms(1000); // Delay 1 seconds
    printf("AT\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function

void wifiModule_mode() {
    printf("AT+CWMODE_CUR=1\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function

void wifiModule_dhcp() {
    printf("AT+CWDHCP_CUR=1,1\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function

void wifiModule_conn() {
    printf("AT+CWJAP_CUR=\"WuggaNet\",\"FredagsBanan\"\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function

void wifiModule_maxconn() {
    printf("AT+CIPMUX=1\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function

void wifiModule_server() {
    printf("AT+CIPSERVER=1,80\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function

void wifiModule_getip() {
    printf("AT+CIFSR\r\n");
    __delay_ms(1000); // Delay 1 seconds
} // End of function
