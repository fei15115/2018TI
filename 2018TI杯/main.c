//-----------------------------------------------
// ��Ȩ��    SivaYao   ����
// ��һ�α��ʱ��:2018��07��20��
//-----------------------------------------------
// �ļ�����  MAIN.C
// ��д�ˣ�  Han(copy from SivaYao) 
// ��дʱ�䣺2021/04/09
// Ŀ������: STC8H
// ���빤��: Keil uV5
//-----------------------------------------------
// ����˵��:
// �ó������FFT�Լ����Ҳ�����߼���ʵ�ֿ��Լ����ڵ�·���ϵĵ��������á�
//-----------------------------------------------
#include   "STC8A8K.h"   //оƬSTC8A8K�ڲ��Ĵ���˵��ͷ�ļ�
#include   "HD7279.h"    //HD7279������ʾ������ ͷ�ļ�
#include   "STC_HW.H"   //оƬ Ƭ��Ӳ������ ͷ�ļ�
#include   "FFT.H"       //FFT����� ͷ�ļ�
#include   "LCD_ALI.h"
#include   "math.h"
#include   "Photo.h"
//7323AD�ܽŶ���
u8 AD7323(u8 CMD);
sbit SCK  = P4^3 ;
sbit MOSI = P4^0 ;
sbit MISO = P4^1 ;
sbit ADCS = P5^1;// //T1
sbit OUT0 = P6^2;//��������������

void   InterFace(void) ;
extern int xdata Fft_Real[1024];
extern int xdata Fft_Image[1024];               // fft���鲿
extern code unsigned int LIST_TAB[1024];
//unsigned int IRms[1024];

//��������
u8  Mode = 0 ;//���ֲ���ģʽ,1--ֱ�Ӳ���,2--FFT��Ƶ����,2--FFT��Ƶ����
bit KeyFlag,T0Flag,MesOver,OneSec,S1SendOver;
u8  KeyNum,S1Num,DisNum; 
u16 T0Count,T_Acc,F_Count,F_Acc;
u16 DataMax,NumMax;
u16 DataMax1;
u8  S1Send[20];
u16 XBF[20],XBV[20],XBNum; //г��Ƶ��,г������,г������
int  ADRes=0,ADOld;    //AD7323�������
bit  TongDao = 1 ;
long int AD_Acc,V_Acc,VPP,VRMS,FRMS,ADMul;		 
extern u16 FColor,BColor;
u8 count=0;
u8 find_flag = 0;

void Delay1ms()		//@33.000MHz
{
	unsigned char i, j;

	i = 33;
	j = 22;
	do
	{
		while (--j);
	} while (--i);
}


void GETSIN( void )
{
	int i;
	for(i=0;i<1024;i++)
	{
		Fft_Real[LIST_TAB[i]] = (int)(1024*sin(3.1415926*2*i*50.1/512));
		Fft_Image[LIST_TAB[i]] = 0;
	}
}

void GETSQR( void )
{
	int i;
	for(i=0;i<1024;i++)
	{
		if(i/256%2)
			Fft_Real[LIST_TAB[i]]=(int)(-512);
      //Fft_Real[LIST_TAB[i]]=0;		
		else
			Fft_Real[LIST_TAB[i]]=(int)(512);
	}
}

void SANJIAO( void )
{
	int i;
	for(i=0;i<1024;i++)
	{
		if(i<512)
			Fft_Real[LIST_TAB[i]]=(int)(-512+i*2);	
		else
			Fft_Real[LIST_TAB[i]]=(int)(1536-(i*2));
	}
}

  u16 X  ,Y  ;
void main()
{
	long int VPP1,vrms1;
	int i;
//0---������Գ���,���ڲ���
	GETSIN();
//	GETSQR();
//	SANJIAO();
	FFT();
	for ( i=0 ; i<512 ; i++ ) {
		Fft_Real[i] = sqrt((long int)Fft_Real[i]*Fft_Real[i]+(long int)Fft_Image[i]*Fft_Image[i]);
   }
//1---Ӳ����ʼ������,��Ƭ��Ӳ����ʼ��(�˿�,��ʱ��,SPI,����,�жϵ�)
	SystemInit();   
//2---HD7279������ʾ�ĳ�ʼ���Ͳ�����ʾ(��ʾ01234567)
	SEND7279_CMD(0xA4) ;
	for (i=0;i<8;i++) 
	{
    	SEND7279_CMD_DATA(0x80+i , 0);		
	}
	AD7323(0xA000);
	AD7323(0x8430);
	while(1){
		//1-----------------//���һ�β������
		if ( MesOver ) {   
			MesOver = 0 ;    //�����־λ
		 //(3)�����FFT��Ƶģʽ,����Ƶ��2.56KHz,����1024��FFT�任
		 //ת����ɺ�Ƶ�ʼ������2.5Hz,ɨ������з�ֵ������ֵ��Ƶ�ʵ�ͷ�ֵ��ʾ����
     		if ( Mode ==3 )   
			 { //(3)�����FFT��Ƶģʽ
				FFT(  );       //(3.1)�ɼ��������Fft_Real������,����1024��FFT�任
			  //XBNum = 0 ;
			  //(3.2)�任��ɺ�,���ϸ�Ƶ�ʵ�ʵ�����鲿,�����ֵ,��ֵҲ����Fft_Real������
				for ( i= 1 ; i<401 ; i++ ) 
				{
					 Fft_Real[i] = sqrt((long int)Fft_Real[i]*Fft_Real[i]+(long int)Fft_Image[i]*Fft_Image[i]);					 
           		}
				if((u32)Fft_Real[20]/10*625/128 > 0x50)
				{
					SEND7279_CMD(0xA4);
					SEND7279_CMD_DATA(0x87 , 1);
					find_flag = 1;
				}else{
					for(i=23 ; i<400 ;i++ )
					{
						if(Fft_Real[i]*625/128/24*1414/100 > 10)
						{
							SEND7279_CMD(0xA4);
							SEND7279_CMD_DATA(0x87 , 2);
							find_flag = 1;
						}
					}
					if(find_flag == 0)
					{
						SEND7279_CMD(0xA4);
						SEND7279_CMD_DATA(0x87 , 3);
					}
				}
				for ( i=0 ; i<400 ;i++ ) {   //���������������,������
				    S1Send[0]= 0xaa ;
					S1Send[1]= 0xaa;
					S1Send[2]= i/256;
					S1Send[3]= i   ;
					S1Send[4]= (u32)Fft_Real[i]*625/1280>>8;
					S1Send[5]= (u32)Fft_Real[i]*625/1280;					
					S1Num = 0 ;SBUF = S1Send[0];
					S1SendOver = 0 ;					
					while ( S1SendOver == 0 ) {;}
					}					 				
				 }  //end of Mode==3
		  }  //end of MesOver==1
//2-----����а�������----------------------
		if ( KeyFlag ) 
		{
			count++;
			KeyFlag = 0 ;  //�����־λ
		  	KeyNum = READ7279_NUM( );
      		GUI_ShowChar(40,20,0x0000,0xFFFF,KeyNum+0x30,16,0);
			
			SEND7279_CMD(0xA4) ;
			Delay1ms();
			SEND7279_CMD_DATA(0x80 , KeyNum/10);
			Delay1ms();
			SEND7279_CMD_DATA(0x81 , KeyNum%10);
			Delay1ms();
			SEND7279_CMD_DATA(0x82 , count/10);
			Delay1ms();
			SEND7279_CMD_DATA(0x83 , count%10);
			//(2.1)�����ֵ��1 ----�趨ģʽΪ1
			if ( KeyNum == 3 ) 
			{ 
				Mode = 3 ;    //�����л���FFT��Ƶģʽ(����г��)
				TR0     = 0 ;
				ADCS=0;
				AD7323(0X80);
				AD7323(0X30);
				ADCS=1;
				TongDao=0;			 				
         		TH0	   = (65536 - 33000000/2560 )/256 ;
         		TL0	   = (65536 - 33000000/2560 )%256 ; //��ʱ��0��ֵ(1��2560���ж�0.4ms)	
         		TR0     = 1 ;
				T0Count = 0 ;	
			} //KeyNum == 3
	  } //KeyFlag == 1
  } //while(1)
}//


void INTTIMER0 (void) interrupt 1 //��ʱ��0�ж�
{	
	 u8   i ;
	 OUT0  = 1 ;   //(1)�жϿ�ʼʱOUT�ø�,����ʱ�õ�,�����ж�����ʱ��,������
	//(2)---����SPIͨѶ,��ȡAD���,�������´�AD�Ĳ���

	 ADCS  = 0 ;
	 ADRes = AD7323(0x80) ;	 		 
	 ADRes = (ADRes*256 + AD7323(0x30))>>1;            //������:ͨ��0�Ե�,����ģʽ,�����Ʋ���,ͬʱ����ͨ��0
	 ADCS = 1 ;	
   //ADRes = AD7323(0x8030);
		//GUI_ShowNum (120,120,ADRes,10,16)
   if ( ADRes > 4095 ) {                          //��Ϊ0-8191��Ӧ-10V-+10V,����Ҫ�������Ƶ���
      ADMul = (long int)(ADRes-4096)*(ADRes-4096)/32; //ͬʱ,����AD�����ƽ����,��Ϊ���ǵ����,���Գ���32
      ADRes = ADRes - 4096 ; } 
	 else {
		  ADMul = (long int)(4096-ADRes)*(4096-ADRes)/32 ;
      ADRes = ADRes - 4096 ; }
	//(3)��������Ҳ�����
   if ( Mode == 1 ) {   
			AD_Acc = AD_Acc + ADMul; //(3.1)ƽ���ͼ���
		  if (( ADOld < 16 ) && ( ADRes >= 16 )) { //(3.2)�ж��Ƿ�һ�����ڵ���
        F_Count ++ ;           //(3.3)�����,��������һ
        if (OneSec)  {         //(3.4)���һ���ӵ���
          V_Acc = AD_Acc ; T_Acc = T0Count ; F_Acc = F_Count ; //��¼���
          AD_Acc = 0 ;     T0Count = 0 ;     F_Count= 0 ;
					MesOver = 1 ;OneSec = 0 ;  }
        }
      T0Count++;				      //(3.5)��ʱ��0����ֵ��һ
	    if ( T0Count == 4800 ) { OneSec = 1 ; } //(3.6)�����4000���ж�,����һ�뵽��
		  ADOld = ADRes ;         //(3.7)��¼��ǰADֵ
	   }
	//(4)�����Mode2��Mode3,����AD�������������(LIST_TAB�±�)
	//   ����1024��,���ñ�־λ,׼������
	if ( Mode == 2 )  {   //(2)�����FFT��Ƶ����
		  if ( T0Count > 0 ) {
        Fft_Real[LIST_TAB[T0Count-1]] = ADRes ;
		    Fft_Image[LIST_TAB[T0Count-1]] = 0 ;}
		  T0Count ++ ;
      if (T0Count ==	1025 ) {
        TR0 = 0 ; MesOver = 1 ;	T0Count = 0 ; }
      }	
	if ( Mode == 3 )  {   //(3)�����FFT��Ƶ����
		  if ( T0Count > 0 ) {
        Fft_Real[LIST_TAB[T0Count-1]] = ADRes ;
		    Fft_Image[LIST_TAB[T0Count-1]] = 0 ;}
		  T0Count ++ ;
      if (T0Count ==	1025 ) {
        TR0 = 0 ; MesOver = 1 ;	T0Count = 0 ; }
      }				
		OUT0 = 0 ;
	}
	
void INTUART1  (void) interrupt 4 //����1�ж�
{
	 u8 DAT;
   if (RI) {
     RI = 0 ;
		 DAT = SBUF ;
		 SBUF = DAT ;
     }
	 if (TI) {
		 TI = 0 ;
		 S1Num++ ;
		 if ( S1Num < 6 ) 
        { SBUF = S1Send[S1Num] ;}
		 else             
        { S1Num = 0 ; S1SendOver = 1;} 
	   }
   }

void INT3 ( void )  interrupt 11  //�ⲿ�ж�3--�����ж�
{
  KeyFlag = 1 ;
}

void   InterFace(void) //��ʼ������ʾ
{
   FColor = RED ;
	 DisP_Photo  (104,0,104+272-1,0+36-1,TITLE,0);	//��ʾ��ɫͼ��
	 FColor = BLACK;
	 GUI_Rectangle(0, 36, 479, 319);//(4)���ı���
	 GUI_Rectangle(2, 38, 477, 317);//
	 FColor = DARKBLUE ;
	 DisP_Photo  (120,40,120+240-1,40+36-1,CeLiang[2],0);	//"�밴��ѡ����"
   }

u8 AD7323(u8 CMD)
{  u8  i ;
	 u8 k=0x80,DataIn=0 ;
   SCK = 1 ;
	 _nop_();_nop_();
	 for (i=0 ; i<8 ; i++) {
		 DataIn = DataIn <<1;
		 if ( CMD & k ) { MOSI = 1 ; }
		 else       { MOSI = 0 ; }
		 SCK = 0 ;		 _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
		 SCK = 1 ;    
		 if ( MISO ) { DataIn = DataIn|0x01 ;}
		 k = k >>1;
     }
	 return (DataIn) ;
	 }
