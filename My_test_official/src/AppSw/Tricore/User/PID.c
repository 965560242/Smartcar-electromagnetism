#include "PID.h"
extern PID sptr;
extern PID s_pid;

/*************************************************************************
*  �������ƣ�int PID_MOTOR_Contrl(int set,int NowPoint)
*  ����˵����PID���Ƶ��
*  ����˵����sptrΪ��PID�ṹ�壬NowPointΪʵ���ٶȣ�ͨ����������ã�
*  �������أ�����PID����
*  �޸�ʱ�䣺2021��4��4��
*************************************************************************/
int PID_MOTOR_Contrl(int set,int NowPoint)
{
    int iError,iIncPid;                     //iError:���, iIncPid:����ֵ
    sptr.SetPoint=set;                      //����Ŀ��ֵ
    iError = sptr.SetPoint - NowPoint;      //��ǰ���  �趨��Ŀ��ֵ��ʵ��ֵ��ƫ��

    iIncPid = sptr.P * (iError - sptr.LastError)             //��������
              + sptr.I * iError;
              //+ sptr.D * (iError - 2*sptr.LastError + sptr.PrevError);

    /*�洢���  �����´μ���*/
    sptr.PrevError = sptr.LastError;
    sptr.LastError = iError;
    // �޷�(С�����Լ����������������ֵ���ƶ���)
    constrain_float((float)iIncPid, 0.0, 1000.0);
    return iIncPid;                         //��������ֵ
}


/*************************************************************************
*  �������ƣ�int PID_Servo_Contrl(int set,int NowPoint)
*  ����˵����PD���ƶ��(λ��ʽ)
*  ����˵����sptrΪ��PID�ṹ�壬NowPointΪʵ���ٶȣ�ͨ����������ã�
*  �������أ�����PID����
*  �޸�ʱ�䣺2021��4��4��
*************************************************************************/
int PID_Servo_Contrl(int set,int NowPoint)
{
    int iError,output;             //iError:���
    s_pid.SetPoint=set;              //����Ŀ��ֵ

    iError = s_pid.SetPoint - NowPoint;      //��ǰ���  �趨��Ŀ��ֵ��ʵ��ֵ��ƫ��

    output = s_pid.P * iError               //��������
              + s_pid.D * (iError - s_pid.LastError);

    /*�洢���  �����´μ���*/
    s_pid.PrevError = s_pid.LastError;
    s_pid.LastError = iError;

    return output;                         //����λ��ֵ
}

/*************************************************************************
 *  ��������constrain_( amt,  low,  high)
 *  ����˵�����޷�����
 *  ����˵����
  * @param    amt   �� ����
  * @param    low   �� ���ֵ
  * @param    high  �� ���ֵ
 *************************************************************************/
float constrain_float(float amt, float low, float high)
{
    return ((amt)<(low)?(low):((amt)>(high)?(high):(amt)));
}

unsigned int constrain_int(int amt, int low, int high)
{
    return ((amt)<(low)?(low):((amt)>(high)?(high):(amt)));
}




