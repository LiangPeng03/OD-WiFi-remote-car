/*
 * motor.h
 *
 *  Created on: 2024��11��22��
 *      Author: lenovo
 */

#ifndef MOTOR_H_
#define MOTOR_H_

void motor_init(void);
void motor_state(int out12_v,int out34_v);
char* move(char* direction, char * last_dir);

#endif /* MOTOR_H_ */
