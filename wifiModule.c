void wifiModule_init() {
    printf("AT\r\n");
//    printf("AT+CWAUTOCONN=0\r\n");
} // End of function

void wifiModule_mode() {
    printf("AT+CWMODE_CUR=3\r\n");
} // End of function

void wifiModule_dhcp() {
    printf("AT+CWDHCP_CUR=1,1\r\n");
} // End of function

void wifiModule_conn() {
    printf("AT+CWJAP_CUR=\"dummy\",\"GtLDPU43\"\r\n");
} // End of function

void wifiModule_maxconn() {
    printf("AT+CIPMUX=1\r\n");
} // End of function

void wifiModule_server() {
    printf("AT+CIPSERVER=1,8080\r\n");
} // End of function
