//������ʱ�Ӳ����ڲ�RC������     DCO��8MHz,��CPUʱ��;  SMCLK��1MHz,����ʱ��ʱ��
#include <msp430g2553.h>
#include <tm1638.h>  //��TM1638�йصı���������������ڸ�H�ļ���

#define sampling_number 80

double V0 = 0;
double V1 = 0;
double V_out = 0;
double I_out = 0;

unsigned int a[sampling_number*2],b[2];
unsigned int ad1,ad0,real;
int i,j,k;
unsigned char digit[8]={'0','0','0','0','0','0','0','0'};

unsigned char pnt=0x11;

unsigned char led[]={0,0,0,0,0,0,0,0};
void Init_Ports(void)
{
    P2SEL &= ~(BIT7+BIT6);       //P2.6��P2.7 ����Ϊͨ��I/O�˿�
    P2DIR |= BIT7 + BIT6 + BIT5; //P2.5��P2.6��P2.7 ����Ϊ���
    P1DIR &= ~BIT0;
    P1DIR &= ~BIT3;
 }

void ADC10_Init(void){
    BCSCTL1 = CALBC1_8MHZ;     // Set range
      DCOCTL = CALDCO_8MHZ;    // Set DCO step + modulation*/

      P1SEL|=BIT3+BIT0;//P1.1��P1.2����ΪADC����A1��A2

      ADC10CTL1|=CONSEQ_3+INCH_1;//����ͨ�����ת��+���ͨ��ΪA2
      ADC10CTL0|=ADC10SHT_2+MSC+ADC10ON+ADC10IE;
      ADC10DTC1|=0xA0;//һ������8��
      ADC10AE0|=BIT0+BIT3;//��A0��A1��ģ������ͨ��
}

void Init_Devices(void)
{
    WDTCTL = WDTPW + WDTHOLD;     // Stop watchdog timer��ͣ�ÿ��Ź�
    if (CALBC1_8MHZ ==0xFF || CALDCO_8MHZ == 0xFF)
    {
        while(1);            // If calibration constants erased, trap CPU!!
    }

    Init_Ports();           //���ú�������ʼ��I/O��
    ADC10_Init();
    _BIS_SR(GIE);           //��ȫ���ж�
}

#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    LPM0_EXIT;//�˳��͹���ģʽ
}

int main(void)
{
    Init_Devices();

    init_TM1638();      //��ʼ��TM1638
    //ϵͳ��ʼ����β������һ��gain_control()

    while(1)
    {
        //for ( i=0;i<10000;i++);
        ADC10CTL0&=~ENC;//�رղ���ʹ��
            while(ADC10CTL1&BUSY);//���AD�Ƿ�æ
            ADC10CTL0|=ENC+ADC10SC;//����ADC
            ADC10SA=(unsigned int)a;//��ȡa[]���׵�ַ�����ȶ�A1��A0����������a[0]��a[1]�С��ٶ�A1��A0����������a[2]��a[3]�У����ѭ����ȥ��
            _BIS_SR(LPM0_bits+GIE);//�͹���ģʽ0�����ж�

             for(i=0;i<2;i++)
            {
              b[i]=0;
              for(j=0;j<sampling_number;j++)
                b[i]+=a[i+j*2];//b[0]=a[ż��]���֮�ͣ�b[1]=a[����]���֮�ͣ�ÿ��b��4����
            }

            V_out =2.142*(3.3*b[1])/1023/sampling_number;//����1000��
            I_out =1.066*(3.3*b[0])/1023/sampling_number;//����1000��

           // V_out=2.0;
            digit[4] = (int)V_out;
            digit[5] = (int)(10 * V_out) % 10;
            digit[6] = (int)(100 * V_out) % 10;
            digit[7] = (int)(1000 * V_out) % 10;

            digit[0] = (int)(I_out);
            digit[1] = (int)(10 * I_out) % 10;
            digit[2] = (int)(100 * I_out) % 10;
            digit[3] = (int)(1000 * I_out) % 10;
            for(k=0;k<=20000;++k);
            TM1638_RefreshDIGIandLED(digit,pnt,led);
    }
}
