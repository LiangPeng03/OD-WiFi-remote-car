/*
 * motor.c
 *
 *  Created on: 2024年11月22日
 *      Author: lenovo
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "motor.h"


#define MOTOR_PORT  GPIO_PORTA_DATA_R
#define MOTOR_PIN   0x3C

void motor_init(void){
    //PA 2~5 -> L298n in 1~4
    SYSCTL_RCGCGPIO_R |= 0x01; // 启用端口A时钟
    while ((SYSCTL_PRGPIO_R & 0x01) == 0); // 等待端口A可用
    GPIO_PORTA_DIR_R |= MOTOR_PIN; // 设置引脚为输出
    GPIO_PORTA_PUR_R |= MOTOR_PIN;       // 启用上拉电阻到PA 2~5
    GPIO_PORTA_DEN_R |= MOTOR_PIN; // 使能引脚

    //配置PWM时钟（设置USEPWMDIV分频器）
    SysCtlPWMClockSet(SYSCTL_PWMDIV_8);                                                                                                 //PWM时钟 16M

    //使能时钟
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //PWM信号分配
    // 3. 配置 PB6 和 PB7 为 PWM 功能
    GPIOPinConfigure(GPIO_PB6_M0PWM0); // PB6 作为 PWM0
    GPIOPinConfigure(GPIO_PB7_M0PWM1); // PB7 作为 PWM1

    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 100); // 设定周期，比如 1000 个时钟周期

    // 5. 设置 PWM 占空比
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 0); // 设置 PB6 的占空比为 50%
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 0); // 设置 PB7 的占空比为 25%

    // 6. 启用 PWM 输出
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true); // 开启输出

}

void motor_state(int out12_v,int out34_v){      //  (-100 , +100)
    if (out12_v == 0){
        MOTOR_PORT &= ~0x0C;    //拉低PB2,3
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 0); // 设置 PB6 的占空比
    }
    if (out34_v == 0){
        MOTOR_PORT &= ~0x30;    //拉低PB4,5
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 0); // 设置 PB7 的占空比
    }
    if (out12_v > 0){
        MOTOR_PORT |= 0x04;     //PB2   高
        MOTOR_PORT &= ~0x08;    //PB3   低
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, out12_v);
    }
    if (out34_v > 0){
        MOTOR_PORT |= 0x10;     //PB4   高
        MOTOR_PORT &= ~0x20;    //PB5   低
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, out34_v);
    }
    if (out12_v < 0){
        MOTOR_PORT |= 0x08;     //PB3   高
        MOTOR_PORT &= ~0x04;    //PB2   低
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, -out12_v);
    }
    if (out34_v < 0){
        MOTOR_PORT |= 0x20;     //PB5   高
        MOTOR_PORT &= ~0x10;    //PB4   低
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, -out34_v);
    }
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true); // 开启输出
}

char* move(char* direction, char * last_dir){
    if(strcmp(direction, last_dir) != 0){
        if(strcmp(direction, "ww") == 0){
            //printf("99,99\n");
            motor_state(51,44);
        }
        else if(strcmp(direction, "ss") == 0){
            //printf("-99,-99\n");
            motor_state(-53,-44);
        }
        else if(strcmp(direction, "aa") == 0){
            motor_state(0,45);
        }
        else if(strcmp(direction, "dd") == 0){
            motor_state(50,0);
        }
        else if(strcmp(direction, "nn") == 0){
            motor_state(0,0);
        }
    //...//
        else if(strcmp(direction, "wa") == 0||strcmp(direction, "aw") == 0){
            motor_state(47,42);
        }
        else if(strcmp(direction, "wd") == 0||strcmp(direction, "dw") == 0){
            motor_state(50,43);
        }
        else if(strcmp(direction, "sa") == 0||strcmp(direction, "as") == 0){
            motor_state(-47,-45);
        }
        else if(strcmp(direction, "sd") == 0||strcmp(direction, "ds") == 0){
            motor_state(-55,-43);
        }
    }

    return * direction;
}

