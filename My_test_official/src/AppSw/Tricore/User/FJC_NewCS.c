/*
 * FJC_NewCS.c
 *
 *  Created on: 2021��4��29��
 *      Author: 96556
 */

#include "FJC_NewCS.h"
#include <LQ_STM.h>
#include <LQ_CCU6.h>

/****************ģ��pid���ư��**************************************/
#define PMAX 100
#define PMIN -100
#define DMAX 100
#define DMIN -100
#define FMAX 100       //����ֵ������ֵ

extern float ad_VAL[6];           //�˲���һ����ad����
extern float AD_VAL[6];

/***************������ư��*************************************/
extern volatile int encValue5;
unsigned char RemLength = 1;         //�������������뵹�����룬1Ϊֱ����¼��2Ϊֱ����/С�Ƕ������¼��3Ϊʮ�ּ�¼��4Ϊ������¼

volatile float HPsum = 0;                     //��������͸�������
volatile float ZTsum = 0;                     //ֱ������������
float DCsum = 0;                     //��������������
float SZsum = 0;                     //ʮ�ֱ���������
float HDsum = 0;                     //��������������

float Remforward = 40.36;                //����ֱ��·��
float Remsforward = 5;               //�����Ƕ�·��

extern volatile uint8 Game_Over; // С�����ȫ������ͣ��
volatile float distance[50] = {0};           //����ֱ�Ǿ��� ����������ֵ/512��
volatile int direction[50] = {0};            //����ֱ�Ƿ���1Ϊ��2Ϊ�ң�3Ϊ��С�Ƕȣ�4Ϊ��С�Ƕ�)

float laternum = 16.135;                  //ֱ���ͺ�
int goflag = 1;               //���ʹ��
/***************������״̬���**********************************/
extern unsigned char dao_flag;
unsigned char res = 0;


float forward = 320;               //ֱ�ǵ���ǰ������
float backward = -240;              //ֱ�ǵ������˾���

float sforward = 10.3;           //С�Ƕȵ���ǰ������
float sbackward = -8.97;            //С�Ƕȵ������˾���

float szkeep = 50;                //ʮ�ֱ���ʱ��
float hdkeep = 150;                //��������ʱ��
float out_hdkeep = 55;

unsigned char slow_flag = 0;

unsigned int fjc_duty = Servo_Center_Mid;
int set_speed = 3000;
int back_speed = 0;
unsigned int help_slow = 0;

unsigned char hd_dir = 0;
unsigned char hd_in = 0;
unsigned int speed = 2000;
extern unsigned char hd_flag;

extern volatile int cnt;
extern unsigned int cnt_return;
extern unsigned char dao_flag;

extern int realpwm;

int PFF[4] = {0,11,20,28};    //PFF[3]Ϊ������ֵ������ֵ��ֵ���Ӽ���
int DFF[4] = {0,7,13,19};      //DFF[3]Ϊ��б仯�������ֵ������ֵ��ֵ���Ӽ���
int UFF[7] = {0,180,350,500,600,680,720};     //�����ǹ���ռ�ձ�
float curr_val = 0.0;                       //���ƫ���ͱ�����Ľ��
float delta_val = 0.0;                      //���ֵƫ��仯��
float last_val = 0.0;                       //��һ�εĵ��ֵ
float up_curr_val = 0.0;                    //��ֱ���ƫ��
float up_last_val = 0.0;               //��ֱ�����һ�ε�ֵ
float up_delta_val = 0.0;                   //��ֱ��е�ű仯��
float elect_M ;                             //�м�������е�ƽ��ֵ
int fuzzy_txt[10];                          //���ڴ��͵��ù��������
float fuzzy_txt1[3] = {0};                         //��Ļ���U����ȺͲ����Ĵ�С
int fuzzy_txt2[1] = {0};
int rule[7][7]={//���仯�� -3,-2,-1, 0, 1, 2, 3                                                                                                                        ʹ��sendFuzzyData()����У׼ģ��pid��������ٽ綪�ߴ����й�������
            {-6,-6,-6,-6,-5,-5,-4,},                      //   -3      0
            {-6,-6,-6,-6,-4,-2,-1,},                      //   -2      1
            {-6,-6,-3,-1, 0, 1, 1,},                      //   -1      2
            {-1,-1,-2, 0, 4, 5, 6,},                      //    0      3
            {-2,-2, 0, 1, 6, 6, 6,},                      //    1      4
            { 1, 1, 5, 6, 6, 6, 6,},                      //    2      5
            { 4, 5, 5, 5, 6, 6, 6} };                     //    3      6


char txtq[15] = {0};
char txtb[15] = {0};
char txtd[15] = {0};
char txta[15] = {0};
char txtj[15] = {0};
char txte[15] = {0};
char txts[15] = {0};
extern char txt1[10];
extern char txt2[10];
extern char txt3[10] ;
extern char txt4[10] ;
extern char txt5[10] ;
extern char txt6[10] ;

extern char txt_fz[10] = {0};
/******************������ر���***********************/
unsigned int Circle_Flag = 0;             //������־λ
unsigned int LeftCircle_Flag = 0;         //��
unsigned int RightCircle_Flag = 0;        //�һ�

/*************************************************************************
*  �������ƣ�float my_sqrt(float number)
*  ����˵����ţ�ٵ���������ƽ����
*  ����˵����float number��ԭʼ��ֵ����
*  �������أ�����ƽ����
*  �޸�ʱ�䣺2021��4��28��
*  ��    ע��0x5f3759df�������Ƚϸ��ӣ����ά���ٿ�
*************************************************************************/
float my_sqrt(float number)
{
    long i;
    float x, y;
    const float f = 1.5F;
    x = number * 0.5F;
    y = number;
    i = * ( long * ) &y;
    i = 0x5f3759df - ( i >> 1 );

    y = * ( float * ) &i;
    y = y * ( f - ( x * y * y ) );
    y = y * ( f - ( x * y * y ) );
    return number * y;
}

/*************************************************************************
*  �������ƣ�float my_absolute(float number)
*  ����˵���������ֵ
*  ����˵����float number��ԭʼ��ֵ����
*  �������أ�����ƽ����
*  �޸�ʱ�䣺2021��4��29��
*  ��    ע��������ֵ������ֵ����
*************************************************************************/
float my_absolute(float number)
{
    if(number < 0)
    {
        number = -number;
    }
    return number;
}

/*************************************************************************
*  �������ƣ�void aricomsumope()
*  ����˵������ȺͲ�����(�����ĸ�ϵ������ȷ��,������ò�ȺͲ��㷨��
*  ����˵����acso_abcp��ֵ��Ҫ���ڵ����õ�
*  �������أ���
*  �޸�ʱ�䣺2021��4��29��
*  ��    ע����ȺͲ��㷨����ȺͲ��㷨��ϲ�Ⱥ��㷨ӵ�и��������³����
*************************************************************************/
void aricomsumope()
{
    const float acso_a = 0.4;
    const float acso_b = 0.6;
    const float acso_c = 0.6;
    const float acso_p = 1;      //����ϵ��p
    last_val = curr_val;
    curr_val =  acso_p*((acso_a*(ad_VAL[4] - ad_VAL[1])+acso_b*(ad_VAL[5] - ad_VAL[0]))/(acso_a*(ad_VAL[4] + ad_VAL[1])+acso_c*(my_absolute(ad_VAL[5]-ad_VAL[0]))));
    delta_val = curr_val - last_val;
    fuzzy_txt1[0] = curr_val;
}

/*************************************************************************
*  �������ƣ�float Vag_pid(float currval,float deltaval)
*  ����˵�������ģ��pid����
*  ����˵����currval:���ƫ���ȺͲ�����Ľ����deltaval�����ֵƫ��仯��;U:ռ�ձȱ仯��
*  �������أ�float���͵�ռ�ձ�
*  �޸�ʱ�䣺2021��5��5��
*  ��    ע������ģ��pid�Զ����ǽ���ʵʱ��������
*************************************************************************/
float Vag_pid(float currval,float deltaval)
{
    float U;                //ƫ��,ƫ��΢���Լ����ֵ�ľ�ȷ��
    float PF[2], DF[2], UF[4];//ƫ��,ƫ��΢���Լ����ֵ��������
    int Pn, Dn, Un[4];        //Un��Ӧ�����ĸ�����Ԫ��
    float temp1, temp2;

    fuzzy_txt1[0] = currval;
    fuzzy_txt1[1] = deltaval;
    /*�����ȵ�ȷ��*/
    /*����PD��ָ������ֵ�����Ч������*/
    if (currval > -PFF[3] && currval < PFF[3])
    {
        if (currval <= -PFF[2])
        {
            Pn = -2;
            PF[0] = FMAX * ((float)(-PFF[2] - currval) / (PFF[3] - PFF[2]));
        }
        else if (currval <= -PFF[1])
        {
            Pn = -1;
            PF[0] = FMAX * ((float)(-PFF[1] - currval) / (PFF[2] - PFF[1]));
        }
        else if (currval <= PFF[0])
        {
            Pn = 0;
            PF[0] = FMAX * ((float)(-PFF[0] - currval) / (PFF[1] - PFF[0]));
        }
        else if (currval <= PFF[1])
        {
            Pn = 1; PF[0] = FMAX * ((float)(PFF[1] - currval) / (PFF[1] - PFF[0]));
        }
        else if (currval <= PFF[2])
        {
            Pn = 2; PF[0] = FMAX * ((float)(PFF[2] - currval) / (PFF[2] - PFF[1]));
        }
        else if (currval <= PFF[3])
        {
            Pn = 3; PF[0] = FMAX * ((float)(PFF[3] - currval) / (PFF[3] - PFF[2]));
        }
    }
    else if (currval <= -PFF[3])
    {
        Pn = -2; PF[0] = FMAX;
    }
    else if (currval >= PFF[3])
    {
        Pn = 3; PF[0] = 0;
    }
    PF[1] = FMAX - PF[0];

    if (deltaval > -DFF[3] && deltaval < DFF[3])
    {
        if (deltaval <= -DFF[2])
        {
            Dn = -2; DF[0] = FMAX * ((float)(-DFF[2] - deltaval) / (DFF[3] - DFF[2]));
        }
        else if (deltaval <= -DFF[1])
        {
            Dn = -1;
            DF[0] = FMAX * ((float)(-DFF[1] - deltaval) / (DFF[2] - DFF[1]));
        }
        else if (deltaval <= DFF[0])
        {
            Dn = 0;
            DF[0] = FMAX * ((float)(-DFF[0] - deltaval) / (DFF[1] - DFF[0]));
        }
        else if (deltaval <= DFF[1])
        {
            Dn = 1;
            DF[0] = FMAX * ((float)(DFF[1] - deltaval) / (DFF[1] - DFF[0]));
        }
        else if (deltaval <= DFF[2])
        {
            Dn = 2; DF[0] = FMAX * ((float)(DFF[2] - deltaval) / (DFF[2] - DFF[1]));
        }
        else if (deltaval <= DFF[3])
        {
            Dn = 3; DF[0] = FMAX * ((float)(DFF[3] - deltaval) / (DFF[3] - DFF[2]));
        }
    }
    else if (deltaval <= -DFF[3])
    {
        Dn = -2;
        DF[0] = FMAX;
    }
    else if (deltaval >= DFF[3])
    {
        Dn = 3;
        DF[0] = 0;
    }
    DF[1] = FMAX - DF[0];

    /*ʹ����Χ�Ż���Ĺ����rule[7][7]*/
    /*���ֵʹ��13����������,����ֵ��UFF[7]ָ��*/
    /*һ�㶼���ĸ�������Ч*/
    fuzzy_txt[0]=Pn - 1 + 3;
    fuzzy_txt[1]=Dn - 1 + 3;
    fuzzy_txt[2]=Pn + 3;
    fuzzy_txt[3]=Dn - 1 + 3;
    fuzzy_txt[4]=Pn - 1 + 3;
    fuzzy_txt[5]=Dn + 3;
    fuzzy_txt[6]=Pn + 3;
    fuzzy_txt[7]=Dn + 3;
    // �õ�������е��ĸ�ֵ
    Un[0] = rule[Pn - 1 + 3][Dn - 1 + 3];
    Un[1] = rule[Pn + 3][Dn - 1 + 3];
    Un[2] = rule[Pn - 1 + 3][Dn + 3];
    Un[3] = rule[Pn + 3][Dn + 3];

    if (PF[0] <= DF[0]) UF[0] = PF[0];
    else    UF[0] = DF[0];
    if (PF[1] <= DF[0]) UF[1] = PF[1];
    else    UF[1] = DF[0];
    if (PF[0] <= DF[1]) UF[2] = PF[0];
    else    UF[2] = DF[1];
    if (PF[1] <= DF[1]) UF[3] = PF[1];
    else    UF[3] = DF[1];

    /*ͬ���������������ֵ���*/
    if (Un[0] == Un[1])
    {
        if (UF[0] > UF[1])  UF[1] = 0;
        else    UF[0] = 0;
    }
    if (Un[0] == Un[2])
    {
        if (UF[0] > UF[2])  UF[2] = 0;
        else    UF[0] = 0;
    }
    if (Un[0] == Un[3])
    {
        if (UF[0] > UF[3])  UF[3] = 0;
        else    UF[0] = 0;
    }
    if (Un[1] == Un[2])
    {
        if (UF[1] > UF[2])  UF[2] = 0;
        else    UF[1] = 0;
    }
    if (Un[1] == Un[3])
    {
        if (UF[1] > UF[3])  UF[3] = 0;
        else    UF[1] = 0;
    }
    if (Un[2] == Un[3])
    {
        if (UF[2] > UF[3])  UF[3] = 0;
        else    UF[2] = 0;
    }
//
    /*���ķ���ģ��*/
    /*Un[]ԭֵΪ�������������ţ�ת��Ϊ��������ֵ*/
    if (Un[0] >= 0) Un[0] = UFF[Un[0]];
    else            Un[0] = -UFF[-Un[0]];
    if (Un[1] >= 0) Un[1] = UFF[Un[1]];
    else            Un[1] = -UFF[-Un[1]];
    if (Un[2] >= 0) Un[2] = UFF[Un[2]];
    else            Un[2] = -UFF[-Un[2]];
    if (Un[3] >= 0) Un[3] = UFF[Un[3]];
    else            Un[3] = -UFF[-Un[3]];

    temp1 = UF[0] * Un[0] + UF[1] * Un[1] + UF[2] * Un[2] + UF[3] * Un[3];
    temp2 = UF[0] + UF[1] + UF[2] + UF[3];
    U = temp1 / temp2;



//    sendFuzzyData();
    return U;
}

/*************************************************************************
*  �������ƣ�void sendFuzzyData()
*  ����˵����У׼������е���ֵ
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2021��5��5��
*  ��    ע����
*************************************************************************/
void sendFuzzyData(){
  char txt[10];

   UART_PutStr(UART0, "-");

  for(int i = 0;i<8;i++){
    sprintf(txt,"%d",fuzzy_txt[i]);
    if(i==0 || i == 2 || i == 4 || i == 6){
        UART_PutStr (UART0, "[");
        UART_PutStr (UART0, txt);
        UART_PutStr (UART0, ".");
    }
    else{
        UART_PutStr (UART0, txt);
        UART_PutStr (UART0, "]");
    }
  }
  UART_PutStr (UART0, "\r\n");
}


/*************************************************************************
*  �������ƣ�void DJCrol()
*  ����˵����������ƺ���
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2021��7��16��
*  ��    ע����
*************************************************************************/
void DJCrol()
{
    Get_ADC();
    Datadeal();
    DirJudge();
    sprintf(txta,"res=%d\r\n",res);
    UART_PutStr (UART0, txta);

    if(RemLength == 1)            //ֱ�ǵ���״̬�²���¼����
    {
        HPsum = encValue5/512.0;      //�ۼƾ���
        ZTsum += HPsum;
//        sprintf(txte, "Enc5: %05d;", encValue5);
//        sprintf(txts, "ZTsum: %.3f;", ZTsum);
//        UART_PutStr(UART0,txte);
//        UART_PutStr(UART0,txts);
//        UART_PutStr(UART0,"\r\n");

    }
    else if(RemLength == 2)
    {
        HPsum = encValue5/512.0;      //�ۼƾ���
        DCsum += HPsum;
    }
    else if(RemLength == 3)
    {
        HPsum = encValue5/512.0;      //�ۼƾ���
        SZsum += HPsum;
    }
    else if(RemLength == 4)
    {
        HPsum = encValue5/512.0;      //�ۼƾ���
        HDsum += HPsum;
    }

    if(Game_Over == 0)
    {
        /* ��������ֵ */
        if(res == 1)    //��ת
        {
            if(RemLength == 1)
            {
                AddListTill(ZTsum,1);              //����ʱ���ۼ����������뵽����������
                ZTsum = 0;
            }
            if(dao_flag == 0)
            {
                fjc_duty = 570;
                set_speed = 4000;
            }
            if(DCsum >= forward || dao_flag == 1)
            {
                dao_flag = 1;
                fjc_duty = 1840;
                back_speed = 4000;
                if(DCsum <= (forward + backward))
                {
                    res = 0;
                    DCsum = 0;
                    dao_flag = 0;
                }
            }
            RemLength = 2;
        }
        else if(res == 2)
        {
            if(RemLength == 1)
            {
                AddListTill(ZTsum,2);              //����ʱ���ۼ����������뵽����������
                ZTsum = 0;
            }
            if(dao_flag == 0)
            {
                fjc_duty = 1840;
                set_speed = 4000;
            }
            if(DCsum >= forward || dao_flag == 1)
            {
                dao_flag = 1;
                fjc_duty = 570;
                back_speed = 4000;
                if(DCsum <= (forward + backward))
                {
                    res = 0;
                    DCsum = 0;
                    dao_flag = 0;
                }
            }
            RemLength = 2;
        }
        else if(res == 3)           //ʮ��
        {
            RemLength = 3;
            fjc_duty = 1240;
            set_speed = 3200;
            if(SZsum >= szkeep)
            {
                res = 0;
                SZsum = 0;
            }
        }
        else if(res == 4)
        {
            RemLength = 4;
            fjc_duty = 1840;
            set_speed = 3500;
            if(HDsum >= hdkeep)
            {
                res = 0;
                HDsum = 0;
                hd_in = 1;     //���뻷
            }
        }
        else if(res == 5)
        {
            RemLength = 4;
            fjc_duty = 1290;
            set_speed = 3700;
            if(HDsum >= out_hdkeep)
            {
                res = 0;
                HDsum = 0;
            }
        }
        //��С�Ƕ�
        else if(res == 8)
        {
//            float left_f = 0;
//            float left_b = 0;
//            sprintf(txt_fz,"var:%f",left_f);
//            UART_PutStr (UART0, txt_fz);
//
//            while(left_f < 160)
//            {
//                UART_PutStr (UART0, "zuofor\r\n");
//                left_f += encValue5/512.0;
//                ATOM_PWM_SetDuty(ATOMSERVO2, 570 , 100);
//                Set_Speed(1000);
//            }
//            while(left_b < 270)
//            {
//                UART_PutStr (UART0, "zuodao\r\n");
//
//                Set_Speedback(4200);
//                left_b += -encValue5/512.0;
//                ATOM_PWM_SetDuty(ATOMSERVO2, 1840 , 100);
//            }
//            res = 0;

            if(RemLength == 1)
            {
                AddListTill(ZTsum,3);              //����ʱ���ۼ����������뵽����������
                ZTsum = 0;
            }
            if(dao_flag == 0)
            {
//                UART_PutStr (UART0, "zuofor\r\n");
                fjc_duty = 570;
                set_speed = 2000;
            }
            if(DCsum >= sforward || dao_flag == 1)
            {
                dao_flag = 1;
                fjc_duty = 1840;
                back_speed = 4000;
                if(DCsum <= (sforward + sbackward))
                {
                    res = 0;
                    DCsum = 0;
                    dao_flag = 0;
                }
            }
            RemLength = 2;

        }
        //��С�Ƕ�
        else if(res == 9)
        {
            if(RemLength == 1)
            {
                AddListTill(ZTsum,4);              //����ʱ���ۼ����������뵽����������
                ZTsum = 0;
            }
            if(dao_flag == 0)
            {
                fjc_duty = 1840;
                set_speed = 4000;
            }
            if(DCsum >= sforward || dao_flag == 1)
            {
                dao_flag = 1;
                fjc_duty = 570;
                back_speed = 4000;
                if(DCsum <= (sforward + sbackward))
                {
                    res = 0;
                    DCsum = 0;
                    dao_flag = 0;
                }
            }
            RemLength = 2;
        }
        else
        {
            RemLength = 1;
            fjc_duty = Servo_Center_Mid + Vag_pid(curr_val*45,delta_val*100);

            //ֱ������
            if(ad_VAL[2] + ad_VAL[3] > 150)
                set_speed = set_speed + 30;

            // ���
            if(ad_VAL[0] > 15 || ad_VAL[5] > 30 && (ad_VAL[2]+ad_VAL[3] >= 10))
            {
                if(fjc_duty >= 550 && fjc_duty <= 580)
                {

                    if(set_speed >= 3000)
                    {
                        back_speed = 1;
                    }
                    else
                        set_speed = 2900;
                }
                if(fjc_duty >= 1730 && fjc_duty <= 1850)
                {
                    if(set_speed >= 3000)
                    {
                        back_speed = 800;
                    }
                    else
                        set_speed = 2700;
                }

                else if(fjc_duty > 1600 && fjc_duty <= 1740)
                {
                    set_speed = (unsigned int)(-1.33*fjc_duty + 5133);
                }
                /*else if(fjc_duty >= 570 && fjc_duty <= 729)
                {
                    set_speed = (unsigned int)(2.5*fjc_duty + 1175);
                }*/
                else
                    set_speed = 3000;
            }
        }
//        sprintf(txt_fz,"DUTY:%d\r\n",fjc_duty);
//        UART_PutStr (UART0, txt_fz);
        fjc_duty = constrain_int(fjc_duty,570,1840);  // �޷�
        ATOM_PWM_SetDuty(ATOMSERVO2, fjc_duty , 100);

/***********************����***********************/
//        sprintf(txt4,"DCsum:%.3f\r\n",DCsum);
//        UART_PutStr (UART0, txt4);
//
//        sprintf(txt5,"back_speed:%d\r\n",back_speed);
//        UART_PutStr (UART0, txt5);
//        sprintf(txt6,"set_speed:%d\r\n",set_speed);
//        UART_PutStr (UART0, txt6);

        set_speed = constrain_int(set_speed,2000,4000);

        if(back_speed == 0)
            Set_Speed(set_speed);
        else
        {
            Set_Speedback(back_speed);
            back_speed = 0;
        }


        //����
//        sprintf(txtq,"%.2f",fuzzy_txt1[0]);
//        sprintf(txtd,"%.2f",fuzzy_txt1[1]);
//        UART_PutStr (UART0, txtq);
//        UART_PutStr (UART0, " ");
//        UART_PutStr (UART0, txtd);
//        UART_PutStr (UART0, "\r\n");
    }
    else
    {
        SearchListTill();
        //TFTSPI_P8X16Str(0,0,"Mode is Rem",u16WHITE,u16BLACK);
        UART_PutStr (UART0, "Mode is Rem");

        sprintf(txtj,"later:%.3f",laternum);
        UART_PutStr (UART0, txtj);
        ZTsum = 0;
        UART_PutStr(UART0,"/***************************************/");
        //TFTSPI_P8X16Str(0,1, txtj,u16WHITE,u16BLACK);
    }
}

/*************************************************************************
*  �������ƣ�float KalmanFilter_Elect(float curr_elect_val,float last_elect_val)
*  ����˵�����������˲�
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2021��5��9��
*  ��    ע����
*************************************************************************/
float KalmanFilter_Elect(float curr_elect_val,float last_elect_val)
{
  static float Q_curr = 0.1;//1.0           //Q���󣬶�̬��Ӧ���󣬹���������Э����
  static float Q_last = 0.0001;         //����������Э�������������Э����Ϊһ��һ�����о���
  static float R_elect = 10.0;                  //����������Э���� ������ƫ��
  static float dt = 0.006;   //0.0055                    //���ּ��

  static float Pk[2][2] = { {1, 0}, {0, 1 }};

  static float Pdot[4] = {0,0,0,0};

  static float q_bias = 0.0;
  static float elect_err = 0.0;
  static float PCt_0 = 0.0;
  static float PCt_1 = 0.0;
  static float E = 0.0;
  static float K_0 = 0.0, K_1 = 0.0, t_0 = 0.0, t_1 = 0.0;

  Pdot[0] = Q_curr - Pk[0][1] - Pk[1][0];       //Pk-����������Э�����΢��
  Pdot[1] = -Pk[1][1];
  Pdot[2] = -Pk[1][1];
  Pdot[3] = Q_last;

  Pk[0][0] += Pdot[0] * dt;             //Pk-�����������Э����΢�ֵĻ���
  Pk[0][1] += Pdot[1] * dt;             //����������Э����
  Pk[1][0] += Pdot[2] * dt;
  Pk[1][1] += Pdot[3] * dt;

  elect_err = curr_elect_val - last_elect_val;          //ƫ�� = ����ֵ - Ԥ��ֵ���������

  PCt_0 = Pk[0][0];
  PCt_1 = Pk[1][0];

  E = R_elect + PCt_0;

  K_0 = PCt_0 / E;
  K_1 = PCt_1 / E;

  t_0 = PCt_0;
  t_1 = Pk[0][1];

  Pk[0][0] -= K_0 * t_0;                    //����������Э����
  Pk[0][1] -= K_0 * t_1;
  Pk[1][0] -= K_1 * t_0;
  Pk[1][1] -= K_1 * t_1;

  curr_elect_val += K_0 * elect_err;                //������� �������ŵ��ֵ ���ŵ��ֵ = Ԥ��ֵ + ����������*(����ֵ-Ԥ��ֵ)
  q_bias += K_1 * elect_err;                //������� �������

  return curr_elect_val;

}


/*************************************************************************
*  �������ƣ�void Datadeal()
*  ����˵�������ݴ�����
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2021��5��10��
*  ��    ע����
*************************************************************************/
void Datadeal()
{

    last_val = curr_val;
    up_last_val = up_curr_val;
    elect_M = 25.5;
      //ˮƽ��в�Ⱥ�
    curr_val =  (my_sqrt(ad_VAL[3]) - my_sqrt(ad_VAL[2] ))/ (my_sqrt(ad_VAL[3]) + my_sqrt(ad_VAL[2])+ my_sqrt(elect_M));
    //��ֱ��в�Ⱥ�
    up_curr_val =(my_sqrt(ad_VAL[5] ) - my_sqrt(ad_VAL[0])) / (my_sqrt(ad_VAL[5]) + my_sqrt(ad_VAL[0]) + my_sqrt(elect_M));

    curr_val = KalmanFilter_Elect(curr_val,last_val);                 //�������˲�
    up_curr_val = KalmanFilter_Elect(up_curr_val,up_last_val);

    fuzzy_txt1[0] = curr_val;
    fuzzy_txt1[1] = up_curr_val;

    delta_val = curr_val - last_val;                                            //��ű仯��
    up_delta_val = up_curr_val - up_last_val;


    if(ad_VAL[0] > 15 || ad_VAL[5] > 20)
    {//���ߵ�в�Ⱥʹ�������м���ֵ
        if(fabs(curr_val) < fabs(up_curr_val) || (ad_VAL[0] > 80 || ad_VAL[5] > 80))
        {
//          UART_PutStr (UART0, "ininin");
          curr_val = up_curr_val;
          delta_val = up_delta_val;
        }
    }

}

/*************************************************************************
*  �������ƣ�void DirJudge()
*  ����˵�����ж϶��ƫת������
*  �������أ�res������жϽ��
*  �޸�ʱ�䣺2021��4��3��
*  ��    ע����������жϣ�һ��Ҫʹ�õ����������ֵ�ɼ�������ʹ�����ƣ���
*************************************************************************/
void DirJudge()
{
    if(ad_VAL[0] + ad_VAL[5] >=160)
    {
        res = 3;         //ʮ��
    }

    else if(ad_VAL[0] - ad_VAL[5] >= 90 && res == 0  && ad_VAL[2]<=60)
    {
        res = 1;        //��ֱ��
        cnt = 0;
        dao_flag = 0;
        cnt_return = 0;
    }

    else  if(ad_VAL[5] - ad_VAL[0] >= 82 && res == 0 && ad_VAL[3] <= 75)
    {
        res = 2;        //��ֱ��
        cnt = 0;
        dao_flag = 0;
        cnt_return = 0;
    }

//    else if(ad_VAL[5] - ad_VAL[0] >= 63 && res == 0 && (ad_VAL[3] + ad_VAL[2]) < 130 && realpwm >= 3000)
//    {
//        res = 9;       //��С�Ƕ�
//    }
//    else if(ad_VAL[0] >= 88 && res == 0  && realpwm >= 2400 && ad_VAL[2] > 80) //48  2+3>165
//    {
//
//        res = 8;       //��С�Ƕ�
//    }

    if((ad_VAL[2] >= 60 && ad_VAL[3] >= 103) && (ad_VAL[5] >= 67 ) && res == 0 )
    {
        res = 4;
    }

    else if((ad_VAL[2] >= 95 && ad_VAL[3] >= 90) && ad_VAL[5]>60 && hd_in == 1)
    {
        res = 5;      //�ҳ���
        hd_in = 0;
    }
}

/*************************************************************************
*  �������ƣ�void DirJudge2()
*  ����˵����������ƫת������
*  �������أ�res������жϽ��
*  �޸�ʱ�䣺2021��4��3��
*  ��    ע����������жϣ�һ��Ҫʹ�õ����������ֵ�ɼ�������ʹ�����ƣ���
*************************************************************************/
void DirJudge2()
{
    if(ad_VAL[0] + ad_VAL[5] >=160)
    {
        res = 3;         //ʮ��
    }
    if((ad_VAL[2] >= 90 || ad_VAL[3] >=90) && (ad_VAL[1] >= 90 || ad_VAL[4] >= 90) && ad_VAL[0] - ad_VAL[5] > 90 && res == 0 )
    {
        hd_in = 1;     //���뻷
        res = 4;
    }
    else if((ad_VAL[2] >= 90 || ad_VAL[3] >= 90) && (ad_VAL[1] >= 90 || ad_VAL[4] >= 90) && hd_in == 1)
    {
        res = 5;      //�ҳ���
        hd_in = 0;
    }
}

/*************************************************************************
*  �������ƣ�void Loseline()
*  ����˵�������߼�⺯��
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2021��7��16��
*  ��    ע����
*************************************************************************/
void Loseline()
{
    if(abs(fjc_duty - 1280) <=120 && ad_VAL[2] + ad_VAL[3] < 5 && res == 0)
    {
        if(fjc_duty > 1280)
        {
            fjc_duty = 1840;
        }
        else
        {
            fjc_duty = 570;
        }
    }
}

void AddListTill(float a,unsigned char Rem)
{
    unsigned char i = 0;
    //ֱ�Ǿ��롢����ֵ
    distance[i] = a;
    direction[i]=Rem;
}

/*************************************************************************
*  �������ƣ�void SearchListTill(void)
*  ����˵�����������롢��������
*  �������أ���
*  �޸�ʱ�䣺2021��7��10��
*  ��    ע��������ʾ/ͨ�ŵ���Ļ�򴮿�
*************************************************************************/
void SearchListTill(void)
{
    unsigned char i = 0;
    unsigned char txtS[20];
    while(direction[i])
    {
        sprintf(txtS[i],"%.6f",distance[i]);


        UART_PutStr (UART0, txtS[i]);
        UART_PutStr (UART0, "\r\n");

        i++;
    }
}

/*************************************************************************
*  �������ƣ�void DJCrol2()
*  ����˵�����������ģʽ��
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2021��7��10��
*  ��    ע����
*************************************************************************/
void DJCrol2()
{
    static unsigned char i = 0;
    Get_ADC();
    Datadeal();
    DirJudge2();

    if(RemLength == 1)            //ֱ�ǵ���״̬�²���¼����
    {
        HPsum = encValue5/512.0;      //�ۼƾ���
        ZTsum += HPsum;
//            sprintf(txte, "Enc5: %05d;", encValue5);
//            sprintf(txts, "ZTsum: %.3f;", ZTsum);
//            UART_PutStr(UART0,txte);
//            UART_PutStr(UART0,txts);
//            UART_PutStr(UART0,"\r\n");

    }
    else if(RemLength == 3)
    {
        HPsum = encValue5/512.0;      //�ۼƾ���
        SZsum += HPsum;
    }
    else if(RemLength == 4)
    {
        HPsum = encValue5/512.0;      //�ۼƾ���
        HDsum += HPsum;
    }


    if((ZTsum >= (distance[i] - laternum)) && direction[i] != 0 && distance[i] > 0 && res == 0)
    {
        if(direction[i] == 1)
        {
            res = 5;
            i++;
        }
        if(direction[i] == 2)
        {
            res = 6;
            i++;
        }
        if(direction[i] == 3)
        {
            res = 8;
            i++;
        }
        if(direction[i] == 40)
        {
            res = 9;
            i++;
        }
    }

    /* ��������ֵ */
    if(res == 5)   //������ת
    {
        fjc_duty = 570;
        set_speed = 2200;
        if(ZTsum >= Remforward)
        {
            ZTsum = 0;
            res = 0;
        }
    }
    else if(res == 6)   //������ת
    {
        fjc_duty = 1840;
        set_speed = 2200;
        if(ZTsum >= Remforward)
       {
           ZTsum = 0;
           res = 0;
       }
    }
    else if(res == 3)           //ʮ��
    {
        RemLength = 3;
        fjc_duty = 1280;
        set_speed = 3200;
        if(SZsum >= szkeep)
        {
            res = 0;
            SZsum = 0;
        }
    }
    else if(res == 4)
    {
        RemLength = 4;
        fjc_duty = 1500;
        set_speed = 3200;
        if(HDsum >= hdkeep)
        {
            res = 0;
            HDsum = 0;
        }
    }
    else if(res == 5)
    {
        RemLength = 4;
        fjc_duty = 1300;
        set_speed = 3200;
        if(HDsum >= hdkeep)
        {
            res = 0;
            HDsum = 0;
        }
    }
    else if(res == 8)                    //������С
    {
        fjc_duty = 570;
        set_speed = 2200;
        if(ZTsum >= Remsforward)
        {
            ZTsum = 0;
            res = 0;
        }
    }
    else if(res == 9)                   //������С
    {
        fjc_duty = 1840;
        set_speed = 2200;
        if(ZTsum >= Remsforward)
        {
            ZTsum = 0;
            res = 0;
        }
    }
    else
    {
        RemLength = 1;
        fjc_duty = Servo_Center_Mid + Vag_pid(curr_val*45,delta_val*100);

        //ֱ��
        set_speed = set_speed + 30;

        // ���
        if(ad_VAL[0] > 15 || ad_VAL[5] > 35 && (ad_VAL[2]+ad_VAL[3] >= 10))
        {
            if(fjc_duty >= 565 && fjc_duty <= 630)
            {

                if(set_speed >= 3000)
                {
                    back_speed = 100;
                }
                else
                    set_speed = 1900;
            }
            else if(fjc_duty >= 1480 && fjc_duty <= 1510)
            {
                if(set_speed >= 3000)
                {
                    back_speed = 1700;
                }
                else
                    set_speed = 100;
            }
            else if(fjc_duty > 1600 && fjc_duty <= 1900)
            {
                set_speed = (unsigned int)(-1.3793*fjc_duty + 5020);
            }
            else if(fjc_duty >= 570 && fjc_duty <= 729)
            {
                set_speed = (unsigned int)(2.515*fjc_duty + 966);
            }
            else
                set_speed = 2200;
        }
    }


        fjc_duty = constrain_int(fjc_duty,570,1840);  // �޷�
        ATOM_PWM_SetDuty(ATOMSERVO2, fjc_duty , 100);



//        sprintf(txt_fz,"back_speed:%d\r\n",back_speed);
//        UART_PutStr (UART0, txt_fz);
//        sprintf(txt_fz,"set_speed:%d\r\n",set_speed);
//        UART_PutStr (UART0, txt_fz);

        set_speed = constrain_int(set_speed,2000,4000);

        if(back_speed == 0)
            Set_Speed(set_speed);
        else
        {
            Set_Speedback(back_speed);
            back_speed = 0;
        }


}

/*************************************************************************
*  �������ƣ�void Mode_Choose(void)
*  ����˵��������������ͣ
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2021��7��10��
*  ��    ע����
*************************************************************************/
void Mode_Choose(void)
{
//    if(!KEY_Read(KEY2))
//    {
//        goflag = 1;
//        ZTsum = 0;
//        UART_PutStr(UART0,"******key2");
//    }
    if(!KEY_Read(KEY0))
    {
        laternum += 0.1;
        UART_PutStr(UART0,"******key0");
    }
    if(!KEY_Read(KEY1))
    {
        goflag = 1;
        ZTsum = 0;
        UART_PutStr(UART0,"******key1");
    }
}

