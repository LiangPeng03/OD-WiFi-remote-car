/*
 * WIFI.c
 *
 *  Created on: 2024年11月24日
 *      Author: lenovo
 */

#include "main.h"
#include "WIFI.h"
#include "uartstdio.h"

void delay_ms(uint32_t n)
{
//    for(uint32_t i = 0; i < n; i++)
        SysCtlDelay((SysCtlClockGet()*n)/3000);  //50000000
}

uint8_t step_cnt = 0;

WIFIDATA wifidata={0};

char direction[2]={};

void UART1IntHandler(void)
{
    uint32_t ui32IntStatus;
    uint8_t ui8RxBuffer[1024];
    uint8_t data = 0;
    uint8_t i = 0;

    ui32IntStatus = UARTIntStatus(UART1_BASE, true);

    UARTIntClear(UART1_BASE, ui32IntStatus);
//  UARTprintf("RNTER INTERRPUT");
    while (UARTCharsAvail(UART1_BASE))
    {

        data = (uint8_t) UARTCharGet(UART1_BASE);
        wifidata.recvbuf[wifidata.recvcnt] = data;
        if((char)data=='+'){
            direction[0]=wifidata.recvbuf[wifidata.recvcnt-4];
            direction[1]=wifidata.recvbuf[wifidata.recvcnt-3];
        }
        wifidata.recvcnt++;
        wifidata.recvcnt%=1024;

    }
//    if (ui8RxBuffer[0] == 'O' && ui8RxBuffer[1] == 'K' && ui8RxBuffer[2] == 'c'
//            && ui8RxBuffer[3] == 'd')
//    {
//        UARTCharPut(UART1_BASE, 'o');
//        UARTCharPut(UART1_BASE, 'k');
//        UARTCharPut(UART1_BASE, 0x0d);
//        UARTCharPut(UART1_BASE, 0x0a);
//    }

}

uint8_t Wifi_Send_Cmd(char * cmd,char * recv,uint32_t timeout)
{
    uint32_t timecnt=0;
    delay_ms(10);
    memset(&wifidata,0,sizeof(wifidata));
    //delay_ms(10);
    UARTprintf("%s",(uint8_t *)cmd);
    //delay_ms(10);
    while(strstr((char *)wifidata.recvbuf,recv)==NULL){
        timecnt++;
        delay_ms(1);
        if(timecnt>=timeout){
            //UARTprintf("fail\n");
            UARTprintf("FAIL DATA = %s\n",wifidata.recvbuf);
            delay_ms(100);
    //      step_cnt = 1;
            return 0;
        }
    }
    //UARTprintf("this is :   %s\n",wifidata.recvbuf);
    delay_ms(100);
    return 1;

}

void esp8266Init(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1));
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

         //配置对应的IO口
        GPIOPinConfigure(GPIO_PB0_U1RX);
        GPIOPinConfigure(GPIO_PB1_U1TX);
        GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

          //配置串口1  8数据位，0校验位，1停止位，波特率115200
        UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
        // UARTstdio
        UARTStdioConfig(1, 115200, SysCtlClockGet());

        UARTFIFOEnable(UART1_BASE);
        UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX2_8, UART_FIFO_RX2_8);
        UARTIntEnable(UART1_BASE, UART_INT_RX);
        IntPrioritySet(INT_UART1, 0x0);
        UARTIntRegister(UART1_BASE, UART1IntHandler);
        IntEnable(INT_UART1);
        IntMasterEnable();

        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
        //    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
        //    {}
        GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_5);
        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0xFF);

        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        //    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
        //    {}
        GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1|GPIO_PIN_2);

//        delay_ms(500);
        if(Wifi_Send_Cmd("AT\r\n","OK",1000)== 1)
        {
            //UARTprintf("1111111");
        }

        if(Wifi_Send_Cmd("AT+CWMODE=1\r\n","OK",1000)== 1){//Station模式
            if(Wifi_Send_Cmd("AT+CWJAP=\"LP1\",\"22222222\"\r\n","OK",10000)== 1){//连接热点
                //delay_ms(5000);

                if(Wifi_Send_Cmd("AT+CIPMUX=1\r\n","OK",3000)== 1){//单路连接模式
                    //delay_ms(1000);

                    if(Wifi_Send_Cmd("AT+CIPSERVER=1,8080\r\n","OK",2000)== 1)
                    {

                         //UARTprintf("this is:  %s   END\n",wifidata.recvbuf);
                         Wifi_Send_Cmd("AT+CIFSR\r\n","OK",2000);
                         return 1;
                     }

                   }

                }
            }
       return 0;
}
char* getWASD(void){
    char temp[2]={};
    temp[0]=direction[0];
    temp[1]=direction[1];
    //printf("temp:%s.\n",temp);
    return *temp;
}
