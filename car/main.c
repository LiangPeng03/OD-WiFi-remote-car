/*
 * main.c
 *
 *  Created on: 2024Äê11ÔÂ22ÈÕ
 *      Author: lenovo
 */
#include "main.h"
#include "motor.h"
#include "WIFI.h"

volatile char direction[2]={};

volatile char last_dir[2]={};

void delayUs(uint32_t us){
    SysCtlDelay((SysCtlClockGet() / 3000000) * us );
}
void delayMs(uint32_t ms){
    SysCtlDelay((SysCtlClockGet()  / 3000)* ms );
}

int main(void){
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    motor_init();

    esp8266Init();

    char *qq;

    while(1){
//        printf("hello\n");
//        motor_state(99,99);
//        delayMs(2000);
//        motor_state(-99,-99);
//        delayMs(1000);
        //motor_state(60,60);



        *direction = getWASD();
        printf("qq:%s.\n",direction);
        //delayMs(1000);
        *last_dir = move(direction,last_dir);
    }
}
