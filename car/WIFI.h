/*
 * WIFI.h
 *
 *  Created on: 2024Äê11ÔÂ24ÈÕ
 *      Author: lenovo
 */

#ifndef WIFI_H_
#define WIFI_H_
typedef struct{
    uint8_t recvbuf[1024];
    uint16_t recvcnt;
}WIFIDATA;
void UART1IntHandler(void);
uint8_t Wifi_Send_Cmd(char * cmd,char * recv,uint32_t timeout);
void esp8266Init(void);
char* getWASD(void);
#endif /* WIFI_H_ */
