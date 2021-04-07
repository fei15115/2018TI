//-----------------------------------------------
// ��Ȩ��    SivaYao   ����
// ��һ�α��ʱ��:2018��07��20��
//-----------------------------------------------
// �ļ�����  MAIN.C
// ��д�ˣ�  SivaYao 
// ��дʱ�䣺2018/07/20
// Ŀ������: STC8A8K
// ���빤��: Keil uV3
//-----------------------------------------------
// ����˵��:
// 2018��A��:�����źż��װ��
//-----------------------------------------------

#include   "STC8A8K.h"   //оƬSTC8A8K�ڲ��Ĵ���˵��ͷ�ļ�
#include   "HD7279.h"    //HD7279������ʾ������ ͷ�ļ�
#include   "STC_HW.H"   //оƬ Ƭ��Ӳ������ ͷ�ļ�
#include   "FFT.H"       //FFT����� ͷ�ļ�
#include   "LCD_ALI.h"
#include   "math.h"
#include   "Photo.h"
u8 AD7323(u8 CMD);
      sbit SCK  = P7^7 ;
      sbit MOSI = P7^5 ;
			sbit MISO = P7^6 ;
      sbit ADCS = P6^3;// //T1
void   InterFace(void) ;
extern int xdata Fft_Real[1024];
extern int xdata Fft_Image[1024];               // fft���鲿
extern code unsigned int LIST_TAB[1024];
//unsigned int IRms[1024];

			sbit OUT0 = P6^2;
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
  long int 	AD_Acc,V_Acc,VPP,VRMS,FRMS,ADMul;		 
extern u16 FColor,BColor;
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
	for (i=0;i<8;i++) {
    SEND7279_CMD_DATA(0x80+i , i) ;		}
//3---Һ��ģ���ʼ������ʼ������ʾ
	LCD_Init();        //Һ��ģ���ʼ������
	LCD_Clear(0xFFFF); //Һ������
  InterFace( ) ;     //��ʼ������ʾ
//4---AD7321��ʼ������
/*	SPI_Conf  (  ) ;   //4.1--SPI����
	ADCS  = 0 ;        //4.2--����AD7323����ģʽ
	SPI_Send (0xA0) ;
	SPI_Send (0x00) ;  //-----����4��ͨ����������10������
	ADCS  = 1 ;
	ADCS  = 0 ;        //4.3--�趨����ͨ��0��AD�任
	SPI_Send (0x84) ;  //Coding����λ -10V---0
	SPI_Send (0x10) ;  //               0V---4096
	ADCS  = 1 ;        //              10V---8191  */
//	AD7323(0xA000);
//	AD7323(0x8430);
//5---������While(1)��������.
//5.1--���һ�β������(Mode1���Լ1�����,Mode2��Mode3���1024��Ĳ���)
//-----��ô���ݹ���ģʽ�Ĳ�ͬ������Ӧ�ļ������ʾ
//-----Mode=1  ���Ҳ�Ƶ�ʺͷ��ֵ����������ʾ
//-----Mode=2  ���Ⲩ��ƵFFT������ʾ(����)
//-----Mode=3  ���Ⲩ��ƵFFT������ʾ(�����͸ߴ�г��),����Ľ�
//5.2--�������һ�ΰ���,����ݼ������ù���ģʽ,Ӳ�����úͽ�����ʾ��
	while(1){
		//1-----------------//���һ�β������
		if ( MesOver ) {   
			MesOver = 0 ;    //�����־λ
			//(1)�����ֱ�Ӳ���ģʽ(���Ҳ�����)
			if ( Mode == 1 )  {
				
				//��Чֵ���㹫ʽVRMS=sqrt(�ۼ�(Ui)^2 / N);NΪ��������
				//һ��N=4000(��Ҫ����),���ڵļ������Ŵ���1000��(�Է�Ϊ��λ)
				VPP  = sqrt(V_Acc/T_Acc*800+V_Acc%T_Acc*800/T_Acc)*707/512 ;  //(1.1)���ֵ����
				VRMS = sqrt(V_Acc/T_Acc*800+V_Acc%T_Acc*800/T_Acc)*250/512 ;  //(1.2)��Чֵ����
				//VPP  = sqrt(V_Acc/5)*707/512 ;  //(1.1)���ֵ����
				//VRMS = sqrt(V_Acc/5)*250/512 ;  //(1.2)��Чֵ����
				//Ƶ�ʼ�����ö�����ͬ��������˼�����
        FRMS = 480000*F_Acc/T_Acc ;       //(1.3)Ƶ�ʼ���
				if (FRMS >=100000) { FRMS = 99999 ;} //��ֹ��ʾ���     //1k
				//(1.4)��Һ��������ʾƵ��ֵ
				DisP_Photo  ( 64,80, 64+12-1,80+24-1,Dig1224[FRMS/10000],0);
				DisP_Photo  ( 76,80, 76+12-1,80+24-1,Dig1224[FRMS%10000/1000],0);
        DisP_Photo  ( 88,80, 88+12-1,80+24-1,Dig1224[FRMS%1000/100],0);
        DisP_Photo  (100,80,100+12-1,80+24-1,Dig1224[10],0);
        DisP_Photo  (112,80,112+12-1,80+24-1,Dig1224[FRMS%100/10],0);
				//(1.5)��Һ��������ʾ���ֵ
//				if(TongDao==1)
//					vrms1=VRMS/10;
//				else
					vrms1=VRMS;
					VPP1=vrms1/6;
				if(TongDao==1)
				{
				DisP_Photo  (238-50,80+40,238+12-1-50,80+64-1,Dig1224[VPP1%100000/10000],0);	
				DisP_Photo  (238-50,80+40,238+12-1-50,80+64-1,Dig1224[VPP1%10000/1000],0);
				DisP_Photo  (250-50,80+40,250+12-1-50,80+64-1,Dig1224[VPP1%1000/100],0);
        DisP_Photo  (262-50,80+40,262+12-1-50,80+64-1,Dig1224[VPP1%100/10],0);
        DisP_Photo  (274-50,80+40,274+12-1-50,80+64-1,Dig1224[10],0);
        DisP_Photo  (286-50,80+40,286+12-1-50,80+64-1,Dig1224[VPP1%10],0);
				}
				else
					{
						DisP_Photo  (238-50,80+40,238+12-1-50,80+64-1,Dig1224[VPP1%100000/10000],0);	
						DisP_Photo  (238-50,80+40,238+12-1-50,80+64-1,Dig1224[VPP1%10000/1000],0);
						DisP_Photo  (250-50,80+40,250+12-1-50,80+64-1,Dig1224[VPP1%1000/100],0);
						DisP_Photo  (262-50,80+40,262+12-1-50,80+64-1,Dig1224[VPP1%100/10],0);
						DisP_Photo  (274-50,80+40,274+12-1-50,80+64-1,Dig1224[VPP1%10],0);
						DisP_Photo  (286-50,80+40,286+12-1-50,80+64-1,Dig1224[10],0);
          }
				GUI_ShowStr (285,120,16,"mA",0);
	      //(1.6)��Һ��������ʾ��Чֵ
					vrms1=(VPP1*1414/1000)/4;
				DisP_Photo  (395,80,395+12-1,80+24-1,Dig1224[vrms1%10000/1000],0);
				DisP_Photo  (407,80,407+12-1,80+24-1,Dig1224[10],0);
        DisP_Photo  (419,80,419+12-1,80+24-1,Dig1224[vrms1%1000/100],0);
        DisP_Photo  (431,80,431+12-1,80+24-1,Dig1224[vrms1%100/10],0);
        DisP_Photo  (443,80,443+12-1,80+24-1,Dig1224[vrms1%10],0);
				if (TongDao) {
				    GUI_ShowChar(50,40,0x0000,0xffff,'1',16,0); }
				else {
				    GUI_ShowChar(50,40,0x0000,0xffff,'0',16,0); }
				if ((TongDao==0)&&(VPP1<100))   {TongDao = 1 ;}
				if ((TongDao==1)&&(VPP1>800))  {TongDao = 0 ;}
        }  //end of Mode ==1 .Mode1������ʾ���
			//(2)�����FFT��Ƶģʽ,����Ƶ��512Hz,����1024��FFT�任
			//ת����ɺ�Ƶ�ʼ������0.5Hz,ͨ��ɨ������ֵ��ȷ������Ƶ�ʺͻ�����ֵ
			if ( Mode ==2 )   { //(2)�����FFT��Ƶģʽ
				FFT(  ); //(2.1)�ɼ��������Fft_Real������,����1024��FFT�任
				//(2.2)�任��ɺ�,���ϸ�Ƶ�ʵ�ʵ�����鲿,�����ֵ,��ֵҲ����Fft_Real������
				for ( i=0 ; i<401 ; i++ ) {
					 Fft_Real[i] = sqrt((long int)Fft_Real[i]*Fft_Real[i]+(long int)Fft_Image[i]*Fft_Image[i]);
           }
				//(2.3)ɨ������,�ҵ�����ֵ�����±�(�±�*0.5����Ƶ��),����,����Ҫ����
				DataMax = 0 ;NumMax = 0 ;
				for ( i = 1; i <401 ; i++ ) {
					if ( Fft_Real[i]> DataMax ) {
						 NumMax = i ;DataMax = Fft_Real[i] ;}
					 }
				//�������Ҫ�޸�(���ֵ����)
        //(2.4)����Ƶ��NumMax(�Ŵ�10��)�ͷ�ֵDataMax(�Ŵ�1000��)					 
				NumMax = NumMax*5 ; DataMax = (u32)DataMax*625/128;
				//(2.5)��ʾƵ��
				DisP_Photo  (112,80,112+12-1,80+24-1,Dig1224[NumMax/1000],0);
				DisP_Photo  (124,80,124+12-1,80+24-1,Dig1224[NumMax%1000/100],0);
        DisP_Photo  (136,80,136+12-1,80+24-1,Dig1224[NumMax%100/10],0);
        DisP_Photo  (148,80,148+12-1,80+24-1,Dig1224[10],0);
        DisP_Photo  (160,80,160+12-1,80+24-1,Dig1224[NumMax%10],0);
				//(2.6)��ʾ��ֵ
					 DataMax1=(long int)DataMax/24*1414/1000;
				DisP_Photo  (296,80,296+12-1,80+24-1,Dig1224[DataMax1/1000],0);
				DisP_Photo  (308,80,308+12-1,80+24-1,Dig1224[10],0);
        DisP_Photo  (320,80,320+12-1,80+24-1,Dig1224[DataMax1%1000/100],0);
        DisP_Photo  (332,80,332+12-1,80+24-1,Dig1224[DataMax1%100/10],0);
        DisP_Photo  (344,80,344+12-1,80+24-1,Dig1224[DataMax1%10],0);
        //(2.7)�Զ��л���FFT����ģʽ��������,��Ҫ�޸�,��ʱ����					 
        /*Mode = 3 ;    //�л���FFT����ģʽ
				TH0	   = (65536 - 24576000/2560) /256 ;
        TL0	   = (65536 - 24576000/2560) %256 ; //��ʱ��0��ֵ(1��2560���ж�0.4ms)	
        TR0     = 1 ;T0Count = 0 ; */
				 }  //end of Mode==2  Mode2������ʾ���
		 //(3)�����FFT��Ƶģʽ,����Ƶ��2.56KHz,����1024��FFT�任
		 //ת����ɺ�Ƶ�ʼ������2.5Hz,ɨ������з�ֵ������ֵ��Ƶ�ʵ�ͷ�ֵ��ʾ����
     if ( Mode ==3 )   { //(3)�����FFT��Ƶģʽ
				FFT(  );       //(3.1)�ɼ��������Fft_Real������,����1024��FFT�任
			  //XBNum = 0 ;
			  //(3.2)�任��ɺ�,���ϸ�Ƶ�ʵ�ʵ�����鲿,�����ֵ,��ֵҲ����Fft_Real������
				for ( i= 1 ; i<401 ; i++ ) {
					 Fft_Real[i] = sqrt((long int)Fft_Real[i]*Fft_Real[i]+(long int)Fft_Image[i]*Fft_Image[i]);					 
           } 
				/*for ( i=0 ; i<400 ;i++ ) {   //���������������,������
				  S1Send[0]= i/256 ;
					S1Send[1]= i     ;
					S1Send[2]= Fft_Real[i]>>8;
					S1Send[3]= Fft_Real[i]   ;
					S1Send[4]= Fft_Image[i]>>8;
					S1Send[5]= Fft_Image[i]   ;					
					S1Num = 0 ;SBUF = S1Send[0];
					S1SendOver = 0 ;					
					while ( S1SendOver == 0 ) {;}
					}*/
				//(3.3)��ʾ�ַ���
				GUI_ShowStr (12,108,16,"fn(Hz)",0);
				GUI_ShowStr (12,124,16,"IP(0.1mA)",0);
				//(3.4)�趨��ʼ����
				X =60;Y =108;
        //(3.5)ɨ����������,�����ֵ������ֵ(��ʱȡ60)��Ƶ�ʵ�ͷ�ֵ					 
        for ( i=0 ; i<=400 ;i++ ) {
          if((i*5/2)%(NumMax/10)<=1)					
          if ( Fft_Real[i] > 60 ) { 
             GUI_ShowNum ( X , Y , i*5/2 , 5 , 16);                      //5λ������ʾƵ��ֵ
						 GUI_ShowNum ( X , Y+16, (u32)Fft_Real[i]*625/128/24*1414/100 , 5 , 16); //5λ������ʾ��ֵ
             X = X+40;}   //ÿ��ʾһ��,�������40
				}					
        //(3.6)---�Զ��л�����FFT�ݲ�����				
        /*Mode = 2 ;    //�л���FFT����ģʽ
				TH0	   = (65536 - 24576000/512) /256 ;
        TL0	   = (65536 - 24576000/512) %256 ; //��ʱ��0��ֵ(1��512���ж�2ms)	
        TR0     = 1 ;T0Count = 0 ;*/
				 }  //end of Mode==3
		  }  //end of MesOver==1
//2-----����а�������----------------------
		if ( KeyFlag ) {
			KeyFlag = 0 ;  //�����־λ
		  KeyNum = READ7279_NUM( );
      GUI_ShowChar(40,20,0x0000,0xFFFF,KeyNum+0x30,16,0);
			//(2.1)�����ֵ��1 ----�趨ģʽΪ1
      if ( KeyNum == 1 ) {
         Mode = 1 ; //�����л���ֱ�Ӳ���ģʽ(�������Ҳ�)
				 TR0  = 0 ; //��ͣ��ʱ��
				 ADCS=0;
				AD7323(0X80);
				AD7323(0X30);
				ADCS=1;
				TongDao=0;
				 GUI_FillRec(4,39,476,316,WHITE);  //�����޸�
	       FColor = DARKBLUE ;
				 DisP_Photo  (120,40,120+240-1,40+36-1,CeLiang[0],0);	//��ʾ"(1)���Ҳ�����"
				 DisP_Photo  (8,80,8+464-1,80+24-1,PinLv,0);          //��ʾ"Ƶ�ʷ��ֵ��Чֵ"
         TH0	   = (65536 - 24576000/4800) /256 ; //
         TL0	   = (65536 - 24576000/4800) %256 ; //�趨��ʱ��0��ֵ(1��4800���ж�0.25ms)	
         TR0     = 1 ; //������ʱ��0
				 T0Count = 0 ; //��ʱ��0����ֵ����				
         }  //KeyNum == 1
			if ( KeyNum == 2 ) { 
				 Mode = 2 ;   //�����л���FFT��Ƶģʽ(��������)
				 TR0     = 0 ;
				 ADCS=0;
				AD7323(0X80);
				AD7323(0X30);
				ADCS=1;
				TongDao=0;
				 GUI_FillRec(4,39,476,316,WHITE);  //�����޸�
				 FColor = DARKBLUE ;
				 DisP_Photo  (120,40,120+240-1,40+36-1,CeLiang[1],0);	//��ʾ"(2)���Ⲩ����"
				 DisP_Photo  (8,80,8+376-1,80+24-1,JiBo[0],0);        //��ʾ"����Ƶ��  ��ֵ"				
         TH0	   = (65536 - 24576000/512) /256 ;
         TL0	   = (65536 - 24576000/512) %256 ; //��ʱ��0��ֵ(1��512���ж�2ms)	
         TR0     = 1 ;   //������ʱ��0 
				 T0Count = 0 ;	  //��ʱ��0����ֵ����	
         } //KeyNum == 2
			if ( KeyNum == 3 ) { 
				 Mode = 3 ;    //�����л���FFT��Ƶģʽ(����г��)
				 TR0     = 0 ;
								 ADCS=0;
				AD7323(0X80);
				AD7323(0X30);
				ADCS=1;
				TongDao=0;			 				
         TH0	   = (65536 - 24576000/2560 )/256 ;
         TL0	   = (65536 - 24576000/2560 )%256 ; //��ʱ��0��ֵ(1��2560���ж�0.4ms)	
         TR0     = 1 ;
				 T0Count = 0 ;	} //KeyNum == 3
			//����F1��F2��Ļ��ʾ�ڲ�����,������
			if ( KeyNum == 0xF1 ) {//��ʾFFT���1
				for ( i = 0 ; i<200; i++ ) {
					GUI_ShowNum ( i%10*48 ,i/10*16,Fft_Real[i],5,16);}
				}//KeyNum == 0xF1
			if ( KeyNum == 0xF2 ) {//��ʾFFT���2
				for ( i = 0 ; i<200; i++ ) {
					GUI_ShowNum ( i%10*48 ,i/10*16,Fft_Image[i],5,16);}
			} //KeyNum == 0xF2
	  } //KeyFlag == 1
  } //while(1)
}//


void INTTIMER0 (void) interrupt 1 //��ʱ��0�ж�
{	
	 u8   i ;
	 OUT0  = 1 ;   //(1)�жϿ�ʼʱOUT�ø�,����ʱ�õ�,�����ж�����ʱ��,������
	//(2)---����SPIͨѶ,��ȡAD���,�������´�AD�Ĳ���
	if (TongDao) {
	 ADCS  = 0 ;
	 ADRes = AD7323(0x84) ;	 		 
	 ADRes = ((ADRes*256 + AD7323(0x30))>>1) -0x2000;            //������:ͨ��0�Ե�,����ģʽ,�����Ʋ���,ͬʱ����ͨ��0
	 //ADRes = ((ADRes*256 + AD7323(0x30))>>1);	
	 ADCS = 1 ;	 
	//ADRes = AD7323(0x8430)-0x2000;
   //GUI_ShowNum (120,120,ADRes,10,16);		
   }
	else {
	 ADCS  = 0 ;
	 ADRes = AD7323(0x80) ;	 		 
	 ADRes = (ADRes*256 + AD7323(0x30))>>1;            //������:ͨ��0�Ե�,����ģʽ,�����Ʋ���,ͬʱ����ͨ��0
	 ADCS = 1 ;	
   //ADRes = AD7323(0x8030);
		//GUI_ShowNum (120,120,ADRes,10,16);	
		}
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
