/*
 * FJC_ControlServo.h
 *
 *  Created on: 2021��4��3��
 *      Author: 96556
 */

#ifndef FJC_CONTROLSERVO_T_H_
#define FJC_CONTROLSERVO_T_H_
#include "FJC_Servo_t.h"
#include "PID.h"
#include <LQ_Atom_Motor.h>

#include <LQ_GPIO_KEY.h>
#include <IfxGtm_PinMap.h>
#include <LQ_GTM.h>
#include <LQ_MotorServo.h>
#include <LQ_ADC.h>
#include <LQ_STM.h>
#include <LQ_UART.h>
#include <stdio.h>
#include <LQ_TFT18.h>
#include <LQ_GTM.h>

// �����ƫתֵ
#define Servo_Center_Mid 1280
#define Servo_RIGHT_MAX 1840
#define Servo_LEFT_MAX 570
// ����ֵ
#define CPS_E1  20
#define CPS_E2 (-6)
#define CPS_E3 (-1)

unsigned char Dir_Judge();
void SteeringCrol();
void Sterring_PID_Init();
void Err_Test();
void Set_PID(float P,float I,float D);


#endif /* SRC_APPSW_TRICORE_USER_FJC_CONTROLSERVO_T_H_ */
