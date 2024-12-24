/*
 * motor.c
 *
 *  Created on: 2024��11��22��
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
    SYSCTL_RCGCGPIO_R |= 0x01; // ���ö˿�Aʱ��
    while ((SYSCTL_PRGPIO_R & 0x01) == 0); // �ȴ��˿�A����
    GPIO_PORTA_DIR_R |= MOTOR_PIN; // ��������Ϊ���
    GPIO_PORTA_PUR_R |= MOTOR_PIN;       // �����������赽PA 2~5
    GPIO_PORTA_DEN_R |= MOTOR_PIN; // ʹ������

    //����PWMʱ�ӣ�����USEPWMDIV��Ƶ����
    SysCtlPWMClockSet(SYSCTL_PWMDIV_8);                                                                                                 //PWMʱ�� 16M

    //ʹ��ʱ��
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //PWM�źŷ���
    // 3. ���� PB6 �� PB7 Ϊ PWM ����
    GPIOPinConfigure(GPIO_PB6_M0PWM0); // PB6 ��Ϊ PWM0
    GPIOPinConfigure(GPIO_PB7_M0PWM1); // PB7 ��Ϊ PWM1

    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 100); // �趨���ڣ����� 1000 ��ʱ������

    // 5. ���� PWM ռ�ձ�
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 0); // ���� PB6 ��ռ�ձ�Ϊ 50%
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 0); // ���� PB7 ��ռ�ձ�Ϊ 25%

    // 6. ���� PWM ���
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true); // �������

}

void motor_state(int out12_v,int out34_v){      //  (-100 , +100)
    if (out12_v == 0){
        MOTOR_PORT &= ~0x0C;    //����PB2,3
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 0); // ���� PB6 ��ռ�ձ�
    }
    if (out34_v == 0){
        MOTOR_PORT &= ~0x30;    //����PB4,5
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 0); // ���� PB7 ��ռ�ձ�
    }
    if (out12_v > 0){
        MOTOR_PORT |= 0x04;     //PB2   ��
        MOTOR_PORT &= ~0x08;    //PB3   ��
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, out12_v);
    }
    if (out34_v > 0){
        MOTOR_PORT |= 0x10;     //PB4   ��
        MOTOR_PORT &= ~0x20;    //PB5   ��
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, out34_v);
    }
    if (out12_v < 0){
        MOTOR_PORT |= 0x08;     //PB3   ��
        MOTOR_PORT &= ~0x04;    //PB2   ��
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, -out12_v);
    }
    if (out34_v < 0){
        MOTOR_PORT |= 0x20;     //PB5   ��
        MOTOR_PORT &= ~0x10;    //PB4   ��
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, -out34_v);
    }
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true); // �������
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

