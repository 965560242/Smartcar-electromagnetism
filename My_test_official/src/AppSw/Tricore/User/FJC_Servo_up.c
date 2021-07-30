#include "FJC_Servo_up.h"


unsigned char transter_buff_s[30];
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)    ) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )


void Servo_Send()
{
    int _cnt=0;//_cnt��������
    unsigned short int tmp = 0;

    /* �����ֶ� */
    transter_buff_s[_cnt++]=0xAA;    //֡ͷ��AAAA
    transter_buff_s[_cnt++]=0xAA;
    transter_buff_s[_cnt++]=0x01;    //�����֣�OXFnֻ��������,����0x02��Ӧ�ڶ�֡����Ҫ��Ӧ����λ���ĵڶ�֡��Ȼ���޸�����λ
    transter_buff_s[_cnt++]=0;       //��Ҫ�������ݵ��ֽ�������ʱ��0�������ڸ�ֵ��

    /* ����λ6��uint16 */
    for(int i=0;i<6;i++)        // 6����ΪĿǰֻ��6�����
    {
        tmp = ad_VAL[i];
        transter_buff_s[_cnt++]= BYTE1(tmp);
        transter_buff_s[_cnt++]= BYTE0(tmp);
    }

    transter_buff_s[3]=_cnt-4;
    int sum=0;
    /* ��У�� */
    for(int i=0;i<_cnt;i++)
        sum += transter_buff_s[i];

    transter_buff_s[_cnt++] = sum;
    UART_PutBuff(UART0,transter_buff_s,_cnt); //��LQ_UART.c �ļ����鿴UART_PutBuff �����÷���
}
