//-----------------------------------------------
// 版权归    SivaYao   所有
// 第一次编程时间:2018年07月20日
//-----------------------------------------------
// 文件名：  MAIN.C
// 编写人：  Han(copy from SivaYao) 
// 编写时间：2021/04/09
// 目标器件: STC8H
// 编译工具: Keil uV5
//-----------------------------------------------
// 程序说明:
// 该程序基于FFT以及正弦波检测逻辑；实现可以检测插在电路板上的电器的作用。
//-----------------------------------------------
#include   "STC8A8K.h"   //芯片STC8A8K内部寄存器说明头文件
#include   "HD7279.h"    //HD7279按键显示驱动库 头文件
#include   "STC_HW.H"   //芯片 片上硬件驱动 头文件
#include   "FFT.H"       //FFT计算库 头文件
#include   "LCD_ALI.h"
#include   "math.h"
#include   "Photo.h"
//7323AD管脚定义
u8 AD7323(u8 CMD);
sbit SCK  = P4^3 ;
sbit MOSI = P4^0 ;
sbit MISO = P4^1 ;
sbit ADCS = P5^1;// //T1
sbit OUT0 = P6^2;//用来检测测量周期

void   InterFace(void) ;
extern int xdata Fft_Real[1024];
extern int xdata Fft_Image[1024];               // fft的虚部
extern code unsigned int LIST_TAB[1024];
//unsigned int IRms[1024];

//变量定义
u8  Mode = 0 ;//三种测量模式,1--直接测量,2--FFT低频测量,2--FFT高频测量
bit KeyFlag,T0Flag,MesOver,OneSec,S1SendOver;
u8  KeyNum,S1Num,DisNum; 
u16 T0Count,T_Acc,F_Count,F_Acc;
u16 DataMax,NumMax;
u16 DataMax1;
u8  S1Send[20];
u16 XBF[20],XBV[20],XBNum; //谐波频率,谐波幅度,谐波计数
int  ADRes=0,ADOld;    //AD7323采样结果
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
//0---仿真测试程序,现在不用
	GETSIN();
//	GETSQR();
//	SANJIAO();
	FFT();
	for ( i=0 ; i<512 ; i++ ) {
		Fft_Real[i] = sqrt((long int)Fft_Real[i]*Fft_Real[i]+(long int)Fft_Image[i]*Fft_Image[i]);
   }
//1---硬件初始化程序,把片上硬件初始化(端口,定时器,SPI,串口,中断等)
	SystemInit();   
//2---HD7279按键显示的初始化和测试显示(显示01234567)
	SEND7279_CMD(0xA4) ;
	for (i=0;i<8;i++) 
	{
    	SEND7279_CMD_DATA(0x80+i , 0);		
	}
	AD7323(0xA000);
	AD7323(0x8430);
	while(1){
		//1-----------------//如果一次测量完成
		if ( MesOver ) {   
			MesOver = 0 ;    //清除标志位
		 //(3)如果是FFT高频模式,采样频率2.56KHz,进行1024点FFT变换
		 //转换完成后频率间隔点是2.5Hz,扫描把所有幅值超过阈值的频率点和幅值显示出来
     		if ( Mode ==3 )   
			 { //(3)如果是FFT高频模式
				FFT(  );       //(3.1)采集结果放在Fft_Real数组中,进行1024点FFT变换
			  //XBNum = 0 ;
			  //(3.2)变换完成后,整合各频率点实部和虚部,计算幅值,幅值也放在Fft_Real数组中
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
				for ( i=0 ; i<400 ;i++ ) {   //串口数据输出程序,调试用
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
//2-----如果有按键按下----------------------
		if ( KeyFlag ) 
		{
			count++;
			KeyFlag = 0 ;  //清除标志位
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
			//(2.1)如果键值是1 ----设定模式为1
			if ( KeyNum == 3 ) 
			{ 
				Mode = 3 ;    //程序切换到FFT高频模式(测量谐波)
				TR0     = 0 ;
				ADCS=0;
				AD7323(0X80);
				AD7323(0X30);
				ADCS=1;
				TongDao=0;			 				
         		TH0	   = (65536 - 33000000/2560 )/256 ;
         		TL0	   = (65536 - 33000000/2560 )%256 ; //定时器0初值(1秒2560次中断0.4ms)	
         		TR0     = 1 ;
				T0Count = 0 ;	
			} //KeyNum == 3
	  } //KeyFlag == 1
  } //while(1)
}//


void INTTIMER0 (void) interrupt 1 //定时器0中断
{	
	 u8   i ;
	 OUT0  = 1 ;   //(1)中断开始时OUT置高,结束时置低,测试中断所需时间,调试用
	//(2)---利用SPI通讯,读取AD结果,并设置下次AD的参数

	 ADCS  = 0 ;
	 ADRes = AD7323(0x80) ;	 		 
	 ADRes = (ADRes*256 + AD7323(0x30))>>1;            //控制字:通道0对地,正常模式,二进制补码,同时读回通道0
	 ADCS = 1 ;	
   //ADRes = AD7323(0x8030);
		//GUI_ShowNum (120,120,ADRes,10,16)
   if ( ADRes > 4095 ) {                          //因为0-8191对应-10V-+10V,所以要进行数制调整
      ADMul = (long int)(ADRes-4096)*(ADRes-4096)/32; //同时,计算AD结果的平方和,因为考虑到溢出,所以除了32
      ADRes = ADRes - 4096 ; } 
	 else {
		  ADMul = (long int)(4096-ADRes)*(4096-ADRes)/32 ;
      ADRes = ADRes - 4096 ; }
	//(3)如果是正弦波测量
   if ( Mode == 1 ) {   
			AD_Acc = AD_Acc + ADMul; //(3.1)平方和计算
		  if (( ADOld < 16 ) && ( ADRes >= 16 )) { //(3.2)判断是否一个周期到了
        F_Count ++ ;           //(3.3)如果是,周期数加一
        if (OneSec)  {         //(3.4)如果一秒钟到了
          V_Acc = AD_Acc ; T_Acc = T0Count ; F_Acc = F_Count ; //记录结果
          AD_Acc = 0 ;     T0Count = 0 ;     F_Count= 0 ;
					MesOver = 1 ;OneSec = 0 ;  }
        }
      T0Count++;				      //(3.5)定时器0计数值加一
	    if ( T0Count == 4800 ) { OneSec = 1 ; } //(3.6)如果到4000次中断,表明一秒到了
		  ADOld = ADRes ;         //(3.7)记录当前AD值
	   }
	//(4)如果是Mode2和Mode3,进行AD结果放入数组中(LIST_TAB下标)
	//   采满1024点,设置标志位,准备计算
	if ( Mode == 2 )  {   //(2)如果是FFT低频测量
		  if ( T0Count > 0 ) {
        Fft_Real[LIST_TAB[T0Count-1]] = ADRes ;
		    Fft_Image[LIST_TAB[T0Count-1]] = 0 ;}
		  T0Count ++ ;
      if (T0Count ==	1025 ) {
        TR0 = 0 ; MesOver = 1 ;	T0Count = 0 ; }
      }	
	if ( Mode == 3 )  {   //(3)如果是FFT高频测量
		  if ( T0Count > 0 ) {
        Fft_Real[LIST_TAB[T0Count-1]] = ADRes ;
		    Fft_Image[LIST_TAB[T0Count-1]] = 0 ;}
		  T0Count ++ ;
      if (T0Count ==	1025 ) {
        TR0 = 0 ; MesOver = 1 ;	T0Count = 0 ; }
      }				
		OUT0 = 0 ;
	}
	
void INTUART1  (void) interrupt 4 //串口1中断
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

void INT3 ( void )  interrupt 11  //外部中断3--按键中断
{
  KeyFlag = 1 ;
}

void   InterFace(void) //初始界面显示
{
   FColor = RED ;
	 DisP_Photo  (104,0,104+272-1,0+36-1,TITLE,0);	//显示单色图像
	 FColor = BLACK;
	 GUI_Rectangle(0, 36, 479, 319);//(4)画四边形
	 GUI_Rectangle(2, 38, 477, 317);//
	 FColor = DARKBLUE ;
	 DisP_Photo  (120,40,120+240-1,40+36-1,CeLiang[2],0);	//"请按键选择功能"
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
