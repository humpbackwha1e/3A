//本程序时钟采用内部RC振荡器。     DCO：8MHz,供CPU时钟;  SMCLK：1MHz,供定时器时钟
#include <msp430g2553.h>
#include <tm1638.h>  //与TM1638有关的变量及函数定义均在该H文件中

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
    P2SEL &= ~(BIT7+BIT6);       //P2.6、P2.7 设置为通用I/O端口
    P2DIR |= BIT7 + BIT6 + BIT5; //P2.5、P2.6、P2.7 设置为输出
    P1DIR &= ~BIT0;
    P1DIR &= ~BIT3;
 }

void ADC10_Init(void){
    BCSCTL1 = CALBC1_8MHZ;     // Set range
      DCOCTL = CALDCO_8MHZ;    // Set DCO step + modulation*/

      P1SEL|=BIT3+BIT0;//P1.1和P1.2设置为ADC功能A1和A2

      ADC10CTL1|=CONSEQ_3+INCH_1;//序列通道多次转换+最大通道为A2
      ADC10CTL0|=ADC10SHT_2+MSC+ADC10ON+ADC10IE;
      ADC10DTC1|=0xA0;//一共采样8次
      ADC10AE0|=BIT0+BIT3;//打开A0和A1的模拟输入通道
}

void Init_Devices(void)
{
    WDTCTL = WDTPW + WDTHOLD;     // Stop watchdog timer，停用看门狗
    if (CALBC1_8MHZ ==0xFF || CALDCO_8MHZ == 0xFF)
    {
        while(1);            // If calibration constants erased, trap CPU!!
    }

    Init_Ports();           //调用函数，初始化I/O口
    ADC10_Init();
    _BIS_SR(GIE);           //开全局中断
}

#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    LPM0_EXIT;//退出低功耗模式
}

int main(void)
{
    Init_Devices();

    init_TM1638();      //初始化TM1638
    //系统初始化结尾，调用一次gain_control()

    while(1)
    {
        //for ( i=0;i<10000;i++);
        ADC10CTL0&=~ENC;//关闭采样使能
            while(ADC10CTL1&BUSY);//检测AD是否繁忙
            ADC10CTL0|=ENC+ADC10SC;//启动ADC
            ADC10SA=(unsigned int)a;//获取a[]的首地址。首先对A1、A0采样，放入a[0]和a[1]中。再对A1、A0采样，放入a[2]和a[3]中，如此循环下去。
            _BIS_SR(LPM0_bits+GIE);//低功耗模式0，开中断

             for(i=0;i<2;i++)
            {
              b[i]=0;
              for(j=0;j<sampling_number;j++)
                b[i]+=a[i+j*2];//b[0]=a[偶数]相加之和，b[1]=a[奇数]相加之和，每个b有4组数
            }

            V_out =2.142*(3.3*b[1])/1023/sampling_number;//扩大1000倍
            I_out =1.066*(3.3*b[0])/1023/sampling_number;//扩大1000倍

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
