/*
 * PID.c
 *
 *  Created on: 2021��4��5��
 *      Author: admin
 */
#include "CXJ_MOTOR.h"
PID sptr; //���PID�ṹ������
extern int encValue5;

int realpwm = 0;
/*************************************************************************
*  �������ƣ�void Set_Speed(unsigned int tar_speed)
*  ����˵����PID���Ƶ��ǰ��,ֻҪ�����趨Ŀ��PWMֵ����,�Զ��޷�(2000,6000)
*  ����˵����tar_speed��Ŀ��PWM,�����������趨
*  �޸�ʱ�䣺2021��4��4��
*************************************************************************/
void Set_Speed(unsigned int tar_speed)
{
    unsigned int tmp_PWM =0;
    int tar_ENC = (int)(0.7676*tar_speed - 1048); //Ŀ���������ֵ
    static unsigned int out_ENC = 0;
    volatile  int tmp_ENC = 0;
    char txt[20],txt1[10];

    // �õ���ǰ��������ֵ
    realpwm = (unsigned int)(1.2993*encValue5 + 1369.5);
//        sprintf(txt,"tar_enc:%d\r\n",tar_ENC);
//        UART_PutStr(UART0,txt);

   // ����PID�õ�Ӧ�����õ�ռ�ձȵ�ֵ
   out_ENC += PID_MOTOR_Contrl(tar_ENC,encValue5);    //��Ŀ���ENC�����ڵ�ENC�Ƚ�
//   sprintf(txt,"out_enc:%d  ",out_ENC);
//   UART_PutStr(UART0,txt);
   out_ENC = constrain_int(out_ENC,0,tar_ENC+20);
   //��ӡ����

//    sprintf(txt,"tmp_enc:%d    ",encValue5);
//    UART_PutStr(UART0,txt);

   tmp_PWM = (unsigned int)(1.2993*out_ENC + 1369.5);                 //�����ת����PWM
//   sprintf(txt,"out_pwm:%d\r\n",tmp_PWM);
//   UART_PutStr(UART0,txt);
   //�����ٶ�
   tmp_PWM = constrain_int(tmp_PWM,0,5000);
   ATOM_PWM_SetDuty(ATOMPWM0,tmp_PWM, 12500);
   ATOM_PWM_SetDuty(ATOMPWM1,0, 12500);
}


/*************************************************************************
*  �������ƣ�void Set_Speedback(unsigned int tar_speed)
*  ����˵����PID���Ƶ������,ֻҪ�����趨Ŀ��PWMֵ����,�Զ��޷�(2000,6000)
*  ����˵����tar_speed��Ŀ��PWM,�����������趨
*  �޸�ʱ�䣺2021��4��4��
*************************************************************************/
void Set_Speedback(unsigned int tar_speed)
{
    unsigned int tmp_PWM =0;
    int tar_ENC = (int)(0.7676*tar_speed - 1048); //Ŀ���������ֵ
    static short out_ENC = 0;
    volatile  int tmp_ENC = 0;
    char txt[20],txt1[10];

    // �õ���ǰ��������ֵ

   // ����PID�õ�Ӧ�����õ�ռ�ձȵ�ֵ
   out_ENC += PID_MOTOR_Contrl(tar_ENC,-encValue5);    //��Ŀ���ENC�����ڵ�ENC�Ƚ�
   out_ENC = constrain_int(out_ENC,tar_ENC,tar_ENC+10);

   tmp_PWM = 1.2993*out_ENC + 1369.5;                 //�����ת����PWM
   //�����ٶ�
   ATOM_PWM_SetDuty(ATOMPWM0,0, 12500);
   ATOM_PWM_SetDuty(ATOMPWM1,tmp_PWM, 12500);
}

void Test_BMQ()
{
    char txt[10];
    char txt_PWM[10];
    unsigned int tmp_PWM = 0;
    volatile int tmp = 0;

//    encValue5 = -ENC_GetCounter(ENC6_InPut_P20_3);
//    tmp_PWM = 54.648*tmp + 1100;
//    sprintf(txt_PWM,"PWM:%d    ",3500);
//    UART_PutStr(UART0,txt_PWM);

    ATOM_PWM_SetDuty(ATOMPWM0,2500, 12500);
    sprintf(txt,"cur:%d\r\n",encValue5);
    UART_PutStr(UART0,txt);
//    TFTSPI_P8X16Str(0, 1, txt,u16WHITE,u16BLACK);

}


/*************************************************
 *��������:void PID_Arg_Init(PID* sptr)
 *��    ��:������ʼ��
 *************************************************/
void PID_Arg_Init()
{
    sptr.SumError = 0;         //����ۼ�
    sptr.LastError = 0;        //�ϴ�ƫ��ֵ
    sptr.PrevError = 0;        //���ϴ�ƫ��ֵ

    sptr.P = 1.8;                //�������� //0.8
    sptr.I = 1.0;                //���ֳ��� //0.01
    sptr.D = 0;                //΢�ֳ���
}
