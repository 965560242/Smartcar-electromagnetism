
/*
 * FJC_ControlServo.c
 *  Created on: 2021��4��3��
 */
#define ATOMSERVO1       IfxGtm_ATOM2_0_TOUT32_P33_10_OUT
#define ATOMSERVO2       IfxGtm_ATOM2_5_TOUT35_P33_13_OUT


#include "FJC_ControlServo.h"

PID s_pid;  // ���PD���ƽṹ��


/* ȫ�ֱ��� */
extern unsigned int ad_VAL[6];
unsigned int s_duty = Servo_Center_Mid;  // Ĭ����ֵ
int group_err[3]={0};
int test1,test2;




/* ��ʾ���� */
char txt[20] = {0};
char txt_sum[20] = {0};
char txt_e1[10] = {0};
char txt_e2[10] = {0};
char txt_e3[10] = {0};
char txt_output[15] = {0};
char txt_pid[15] = {0};

char txt_kp[10] = {0};
char txt_kd[10] = {0};


/*************************************************************************
*  �������ƣ�signed int Dir_Judge()
*  ����˵�����ж϶��ƫת������
*  �������أ���
*  �޸�ʱ�䣺2021��4��3��
*  ��    ע�����Ҵ��������ע�⣺ֻ��Ϊ���ԣ���δ���ǵ���Ų����������
*************************************************************************/
unsigned char Dir_Judge()
{
    static unsigned char res = 0;  // С��������λ��:0��ʾֱ��
    // ��ֵ�жϷ���
    if((ad_VAL[0] >= 40 && res == 0) && (ad_VAL[2] <=92 && ad_VAL[3] <= 92) || (res == 9 && ad_VAL[5] >= 90) || ((ad_VAL[0] + ad_VAL[5] >= 135) && res!=2))
        res = 1;        // ��ת��
    else if(ad_VAL[5] >= 40 && res == 0 && (ad_VAL[2] <=92 && ad_VAL[3] <= 92))
        res = 2;        // ��ת��
    // ֱ��
    else if((ad_VAL[2] >= 88 || ad_VAL[3] >= 88) && (ad_VAL[0]<=45 || ad_VAL[5] <= 45) &&
            (ad_VAL[0]<=75 && ad_VAL[5] <= 75))  // �����ֵ����Ҫ
        res = 0;

    // ����״̬���ж�
    if(res == 2 && group_err[0] >= 0 && ad_VAL[0] != 100)
        res = 8;
    else if(res == 1 && group_err[0] <= 0 && ad_VAL[5] != 100 && ((ad_VAL[0]+ad_VAL[1])>ad_VAL[5]+ad_VAL[4]))
        res = 9;
    return res;
}

/*************************************************************************
*  �������ƣ�void Sterring_PID_Init(void)
*  ����˵�������PD������ʼ��
*  �������ã���ʼ�����ü���
*************************************************************************/
void Sterring_PID_Init()
{
    s_pid.P = 1.5;
    s_pid.I = 0;
    s_pid.D = 0.0;
}

/*************************************************************************
*  �������ƣ�void SteeringCrol(void)
*  ����˵�����������ƫת,������Ƿ�Ӧ
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2021��4��3��
*  ��    ע����Ŵ���������ֵ����
*************************************************************************/
void SteeringCrol()
{
    unsigned char pos = 0;
    int input_error = 0;
    int sum_err=0;
    int sum_dg = 0;           //�˲���һ����ad����
    My_ReadADC();  // �ɼ����ֵ
    Get_ADC();
    /* ��Ե��ֵ����������� */
    for(int i=0;i<3;i++)
    {
//        group_err[i] = ((ad_VAL[i] - ad_VAL[5-i]) / (ad_VAL[i] + ad_VAL[5-i]+1)) * 100;  //ע:����ͨ�����ߵ��ֵ��ƫ�����õ�������������ĵ�λ��

        group_err[i] =   ad_VAL[i] - ad_VAL[5-i];
    }

    /* ��̬���� */
    group_err[0] += CPS_E1;
    group_err[1] += CPS_E2;
    group_err[2] += CPS_E3;

    /* TFT���궨�Ͳ��� */
//    Err_Test();

    /* ͣ���ж� */
    for(int i=0;i<5;i++)
        sum_dg+=ad_VAL[i];
    if(sum_dg < 10)
        ATOM_PWM_SetDuty(ATOMPWM0, 0, 12500);


    /* С����̬�ж� */
    pos = Dir_Judge();
    // ������ʾ����
    sprintf(txt,"flag:%d",pos);
    TFTSPI_P8X16Str(0, 0, txt,u16WHITE,u16BLACK);       //�ַ�����ʾ
    sprintf(txt_e1,"e1:%d",group_err[0]);
    TFTSPI_P8X16Str(0, 1, txt_e1,u16WHITE,u16BLACK);       //�ַ�����ʾ
    sprintf(txt_e2,"e2:%d",group_err[1]);
    TFTSPI_P8X16Str(0, 2, txt_e2,u16WHITE,u16BLACK);       //�ַ�����ʾ

    sprintf(txt_sum,"AD0:%d",ad_VAL[0]);
    TFTSPI_P8X16Str(0, 4, txt_sum,u16WHITE,u16BLACK);       //�ַ�����ʾ
    sprintf(txt_output,"AD5:%d",ad_VAL[5]);
    TFTSPI_P8X16Str(0, 5, txt_output,u16WHITE,u16BLACK);       //�ַ�����ʾ
    sprintf(txt_pid,"AD3:%d",ad_VAL[2]);
    TFTSPI_P8X16Str(0, 6, txt_pid,u16WHITE,u16BLACK);       //�ַ�����ʾ
    sprintf(txt_e3,"AD4:%d",ad_VAL[3]);
    TFTSPI_P8X16Str(0, 3, txt_e3,u16WHITE,u16BLACK);       //�ַ�����ʾ
    switch(pos)
    {
        /* ֱ�� */
        case 0:
            /* ����ˮƽ��������ƽ��ֵ */
            sum_err =  group_err[1] + group_err[2];
            input_error =  sum_err / 2;
            /* ���λ��ֵ���� */
            input_error =  input_error*2.5 + Servo_Center_Mid;
            /* �ֶ�PID */
            Set_PID(1.2,0,0.1);
            // PD����+�޷�
            s_duty = Servo_Center_Mid + PID_Servo_Contrl(Servo_Center_Mid,input_error);
            s_duty = constrain_int(s_duty,820,1600);  // �޷�
            /* ��������ֵ */
            ATOM_PWM_SetDuty(ATOMSERVO2, s_duty, 100);
            ATOM_PWM_SetDuty(ATOMPWM0,2500, 12500);    //���
        break;
        /* ��ֱ���� */
        case 1:
//            ATOM_PWM_SetDuty(ATOMSERVO2, Servo_LEFT_MAX, 100);
            /* �����������ƽ��ֵ */
            sum_err =  group_err[0];
            input_error =  sum_err;
            /* ���λ��ֵ���� */
            input_error =  input_error*2.5 + Servo_Center_Mid;
            Set_PID(8.0,0,0.5);
            // PD����+�޷�
            s_duty = Servo_Center_Mid + PID_Servo_Contrl(Servo_Center_Mid,input_error);
            s_duty = constrain_int(s_duty,820,1610);  // �޷�
            /* ��������ֵ */
            ATOM_PWM_SetDuty(ATOMSERVO2, s_duty, 100);
            ATOM_PWM_SetDuty(ATOMPWM0,1700, 12500);    //�趨�����ռ�ձ�
        break;
        /* ��ֱ���� */
        case 2:
//            ATOM_PWM_SetDuty(ATOMSERVO2, Servo_RIGHT_MAX, 100);
            /* �����������ƽ��ֵ */
            sum_err =  group_err[0];
            input_error =  sum_err;
            /* ���λ��ֵ���� */
            input_error =  input_error*2.5 + Servo_Center_Mid;
            Set_PID(8.0,0,0.5);
            //PD����
            s_duty = Servo_Center_Mid + PID_Servo_Contrl(Servo_Center_Mid,input_error);
            s_duty = constrain_int(s_duty,820,1610);  // �޷�
            ATOM_PWM_SetDuty(ATOMSERVO2, s_duty, 100);
            ATOM_PWM_SetDuty(ATOMPWM0,1700, 12500);    //�趨�����ռ�ձ�
        break;
        /* ��ֱ������м�״̬ */
        case 8:
            ATOM_PWM_SetDuty(ATOMSERVO2, Servo_RIGHT_MAX, 100);
        break;
        case 9:
            ATOM_PWM_SetDuty(ATOMSERVO2, Servo_LEFT_MAX, 100);
        break;

    }

}


/*************************************************************************
*  �������ƣ�void Key_Control()
*  ����˵������������
*************************************************************************/
void Key_Control()
{
    if(!KEY_Read(KEY0))
        s_pid.D = 0;
    if(!KEY_Read(KEY1))
        s_pid.P -= 0.03;
    if(!KEY_Read(KEY2))
        s_pid.P += 0.03;

    sprintf(txt_kp," Kp:%.2f\n ",s_pid.P);
    TFTSPI_P8X16Str(0, 5, txt_kp,u16WHITE,u16BLACK);
    sprintf(txt_kd," Kd:%.2f\n ",s_pid.D);
    TFTSPI_P8X16Str(0, 6, txt_kd,u16WHITE,u16BLACK);
}
/*************************************************************************
*  �������ƣ�void Set_PID(float P,float I,float D)
*  ����˵��������PID����
*************************************************************************/
void Set_PID(float P,float I,float D)
{
    s_pid.P = P;
    s_pid.I = I;
    s_pid.D = D;
}

/*************************************************************************
*  �������ƣ�void Err_Test()
*  ����˵�������ڱ궨���(TFT��ʾ)
*************************************************************************/
void Err_Test()
{
    sprintf(txt_e1,"e1:%d",group_err[0]);
    sprintf(txt_e2,"-:%d",test1);
    sprintf(txt_e3,"+:%d",test2);
    TFTSPI_P8X16Str(0, 0, txt_e1,u16WHITE,u16BLACK);       //�ַ�����ʾ
    TFTSPI_P8X16Str(0, 1, txt_e2,u16WHITE,u16BLACK);
    TFTSPI_P8X16Str(0, 2, txt_e3,u16WHITE,u16BLACK);
}











