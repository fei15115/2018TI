//-----------------------------------------------
// 版权归    SivaYao   所有
// 第一次编程时间:2018年07月20日
//-----------------------------------------------
// 文件名：  MAIN.C
// 编写人：  SivaYao 
// 编写时间：2018/07/20
// 目标器件: STC8A8K
// 编译工具: Keil uV3
//-----------------------------------------------
// 程序说明:
// 2018年A题:电流信号检测装置
//-----------------------------------------------

#include   "STC8A8K.h"   //芯片STC8A8K内部寄存器说明头文件
#include   "HD7279.h"    //HD7279按键显示驱动库 头文件
#include   "STC_HW.H"   //芯片 片上硬件驱动 头文件
#include   "FFT.H"       //FFT计算库 头文件
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
extern int xdata Fft_Image[1024];               // fft的虚部
extern code unsigned int LIST_TAB[1024];
//unsigned int IRms[1024];

			sbit OUT0 = P6^2;
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
	for (i=0;i<8;i++) {
    SEND7279_CMD_DATA(0x80+i , i) ;		}
//3---液晶模块初始化及初始界面显示
	LCD_Init();        //液晶模块初始化函数
	LCD_Clear(0xFFFF); //液晶清屏
  InterFace( ) ;     //初始界面显示
//4---AD7321初始化设置
/*	SPI_Conf  (  ) ;   //4.1--SPI配置
	ADCS  = 0 ;        //4.2--配置AD7323工作模式
	SPI_Send (0xA0) ;
	SPI_Send (0x00) ;  //-----设置4个通道都是正负10伏量程
	ADCS  = 1 ;
	ADCS  = 0 ;        //4.3--设定启动通道0的AD变换
	SPI_Send (0x84) ;  //Coding设置位 -10V---0
	SPI_Send (0x10) ;  //               0V---4096
	ADCS  = 1 ;        //              10V---8191  */
//	AD7323(0xA000);
//	AD7323(0x8430);
//5---主程序While(1)完成两项工作.
//5.1--如果一次测试完成(Mode1完成约1秒测量,Mode2和Mode3完成1024点的测量)
//-----那么根据工作模式的不同进行相应的计算和显示
//-----Mode=1  正弦波频率和峰峰值测量计算显示
//-----Mode=2  任意波低频FFT计算显示(基波)
//-----Mode=3  任意波高频FFT计算显示(基波和高次谐波),还需改进
//5.2--如果是有一次按键,则根据键码设置工作模式,硬件设置和界面显示等
	while(1){
		//1-----------------//如果一次测量完成
		if ( MesOver ) {   
			MesOver = 0 ;    //清除标志位
			//(1)如果是直接测量模式(正弦波测量)
			if ( Mode == 1 )  {
				
				//有效值计算公式VRMS=sqrt(累加(Ui)^2 / N);N为测量点数
				//一般N=4000(需要修正),现在的计算结果放大了1000倍(以伏为单位)
				VPP  = sqrt(V_Acc/T_Acc*800+V_Acc%T_Acc*800/T_Acc)*707/512 ;  //(1.1)峰峰值计算
				VRMS = sqrt(V_Acc/T_Acc*800+V_Acc%T_Acc*800/T_Acc)*250/512 ;  //(1.2)有效值计算
				//VPP  = sqrt(V_Acc/5)*707/512 ;  //(1.1)峰峰值计算
				//VRMS = sqrt(V_Acc/5)*250/512 ;  //(1.2)有效值计算
				//频率计算采用多周期同步测量的思想计算
        FRMS = 480000*F_Acc/T_Acc ;       //(1.3)频率计算
				if (FRMS >=100000) { FRMS = 99999 ;} //防止显示溢出     //1k
				//(1.4)在液晶屏上显示频率值
				DisP_Photo  ( 64,80, 64+12-1,80+24-1,Dig1224[FRMS/10000],0);
				DisP_Photo  ( 76,80, 76+12-1,80+24-1,Dig1224[FRMS%10000/1000],0);
        DisP_Photo  ( 88,80, 88+12-1,80+24-1,Dig1224[FRMS%1000/100],0);
        DisP_Photo  (100,80,100+12-1,80+24-1,Dig1224[10],0);
        DisP_Photo  (112,80,112+12-1,80+24-1,Dig1224[FRMS%100/10],0);
				//(1.5)在液晶屏上显示峰峰值
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
	      //(1.6)在液晶屏上显示有效值
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
        }  //end of Mode ==1 .Mode1计算显示完成
			//(2)如果是FFT低频模式,采样频率512Hz,进行1024点FFT变换
			//转换完成后频率间隔点是0.5Hz,通过扫描最大幅值点确定基波频率和基波幅值
			if ( Mode ==2 )   { //(2)如果是FFT低频模式
				FFT(  ); //(2.1)采集结果放在Fft_Real数组中,进行1024点FFT变换
				//(2.2)变换完成后,整合各频率点实部和虚部,计算幅值,幅值也放在Fft_Real数组中
				for ( i=0 ; i<401 ; i++ ) {
					 Fft_Real[i] = sqrt((long int)Fft_Real[i]*Fft_Real[i]+(long int)Fft_Image[i]*Fft_Image[i]);
           }
				//(2.3)扫描数组,找到最大幅值及其下标(下标*0.5就是频率),简化了,可能要修正
				DataMax = 0 ;NumMax = 0 ;
				for ( i = 1; i <401 ; i++ ) {
					if ( Fft_Real[i]> DataMax ) {
						 NumMax = i ;DataMax = Fft_Real[i] ;}
					 }
				//这里简化了要修改(最大值计算)
        //(2.4)计算频率NumMax(放大10倍)和幅值DataMax(放大1000倍)					 
				NumMax = NumMax*5 ; DataMax = (u32)DataMax*625/128;
				//(2.5)显示频率
				DisP_Photo  (112,80,112+12-1,80+24-1,Dig1224[NumMax/1000],0);
				DisP_Photo  (124,80,124+12-1,80+24-1,Dig1224[NumMax%1000/100],0);
        DisP_Photo  (136,80,136+12-1,80+24-1,Dig1224[NumMax%100/10],0);
        DisP_Photo  (148,80,148+12-1,80+24-1,Dig1224[10],0);
        DisP_Photo  (160,80,160+12-1,80+24-1,Dig1224[NumMax%10],0);
				//(2.6)显示幅值
					 DataMax1=(long int)DataMax/24*1414/1000;
				DisP_Photo  (296,80,296+12-1,80+24-1,Dig1224[DataMax1/1000],0);
				DisP_Photo  (308,80,308+12-1,80+24-1,Dig1224[10],0);
        DisP_Photo  (320,80,320+12-1,80+24-1,Dig1224[DataMax1%1000/100],0);
        DisP_Photo  (332,80,332+12-1,80+24-1,Dig1224[DataMax1%100/10],0);
        DisP_Photo  (344,80,344+12-1,80+24-1,Dig1224[DataMax1%10],0);
        //(2.7)自动切换到FFT高速模式还有问题,需要修改,暂时不做					 
        /*Mode = 3 ;    //切换成FFT高速模式
				TH0	   = (65536 - 24576000/2560) /256 ;
        TL0	   = (65536 - 24576000/2560) %256 ; //定时器0初值(1秒2560次中断0.4ms)	
        TR0     = 1 ;T0Count = 0 ; */
				 }  //end of Mode==2  Mode2计算显示完成
		 //(3)如果是FFT高频模式,采样频率2.56KHz,进行1024点FFT变换
		 //转换完成后频率间隔点是2.5Hz,扫描把所有幅值超过阈值的频率点和幅值显示出来
     if ( Mode ==3 )   { //(3)如果是FFT高频模式
				FFT(  );       //(3.1)采集结果放在Fft_Real数组中,进行1024点FFT变换
			  //XBNum = 0 ;
			  //(3.2)变换完成后,整合各频率点实部和虚部,计算幅值,幅值也放在Fft_Real数组中
				for ( i= 1 ; i<401 ; i++ ) {
					 Fft_Real[i] = sqrt((long int)Fft_Real[i]*Fft_Real[i]+(long int)Fft_Image[i]*Fft_Image[i]);					 
           } 
				/*for ( i=0 ; i<400 ;i++ ) {   //串口数据输出程序,调试用
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
				//(3.3)显示字符串
				GUI_ShowStr (12,108,16,"fn(Hz)",0);
				GUI_ShowStr (12,124,16,"IP(0.1mA)",0);
				//(3.4)设定初始坐标
				X =60;Y =108;
        //(3.5)扫描整个数组,输出幅值超过阈值(暂时取60)的频率点和幅值					 
        for ( i=0 ; i<=400 ;i++ ) {
          if((i*5/2)%(NumMax/10)<=1)					
          if ( Fft_Real[i] > 60 ) { 
             GUI_ShowNum ( X , Y , i*5/2 , 5 , 16);                      //5位数字显示频率值
						 GUI_ShowNum ( X , Y+16, (u32)Fft_Real[i]*625/128/24*1414/100 , 5 , 16); //5位数字显示幅值
             X = X+40;}   //每显示一点,横坐标加40
				}					
        //(3.6)---自动切换低速FFT暂不进行				
        /*Mode = 2 ;    //切换成FFT低速模式
				TH0	   = (65536 - 24576000/512) /256 ;
        TL0	   = (65536 - 24576000/512) %256 ; //定时器0初值(1秒512次中断2ms)	
        TR0     = 1 ;T0Count = 0 ;*/
				 }  //end of Mode==3
		  }  //end of MesOver==1
//2-----如果有按键按下----------------------
		if ( KeyFlag ) {
			KeyFlag = 0 ;  //清除标志位
		  KeyNum = READ7279_NUM( );
      GUI_ShowChar(40,20,0x0000,0xFFFF,KeyNum+0x30,16,0);
			//(2.1)如果键值是1 ----设定模式为1
      if ( KeyNum == 1 ) {
         Mode = 1 ; //程序切换到直接测量模式(测量正弦波)
				 TR0  = 0 ; //暂停定时器
				 ADCS=0;
				AD7323(0X80);
				AD7323(0X30);
				ADCS=1;
				TongDao=0;
				 GUI_FillRec(4,39,476,316,WHITE);  //界面修改
	       FColor = DARKBLUE ;
				 DisP_Photo  (120,40,120+240-1,40+36-1,CeLiang[0],0);	//显示"(1)正弦波测量"
				 DisP_Photo  (8,80,8+464-1,80+24-1,PinLv,0);          //显示"频率峰峰值有效值"
         TH0	   = (65536 - 24576000/4800) /256 ; //
         TL0	   = (65536 - 24576000/4800) %256 ; //设定定时器0初值(1秒4800次中断0.25ms)	
         TR0     = 1 ; //启动定时器0
				 T0Count = 0 ; //定时器0计数值清零				
         }  //KeyNum == 1
			if ( KeyNum == 2 ) { 
				 Mode = 2 ;   //程序切换到FFT低频模式(测量基波)
				 TR0     = 0 ;
				 ADCS=0;
				AD7323(0X80);
				AD7323(0X30);
				ADCS=1;
				TongDao=0;
				 GUI_FillRec(4,39,476,316,WHITE);  //界面修改
				 FColor = DARKBLUE ;
				 DisP_Photo  (120,40,120+240-1,40+36-1,CeLiang[1],0);	//显示"(2)任意波测量"
				 DisP_Photo  (8,80,8+376-1,80+24-1,JiBo[0],0);        //显示"基波频率  幅值"				
         TH0	   = (65536 - 24576000/512) /256 ;
         TL0	   = (65536 - 24576000/512) %256 ; //定时器0初值(1秒512次中断2ms)	
         TR0     = 1 ;   //启动定时器0 
				 T0Count = 0 ;	  //定时器0计数值清零	
         } //KeyNum == 2
			if ( KeyNum == 3 ) { 
				 Mode = 3 ;    //程序切换到FFT高频模式(测量谐波)
				 TR0     = 0 ;
								 ADCS=0;
				AD7323(0X80);
				AD7323(0X30);
				ADCS=1;
				TongDao=0;			 				
         TH0	   = (65536 - 24576000/2560 )/256 ;
         TL0	   = (65536 - 24576000/2560 )%256 ; //定时器0初值(1秒2560次中断0.4ms)	
         TR0     = 1 ;
				 T0Count = 0 ;	} //KeyNum == 3
			//键码F1和F2屏幕显示内部数组,调试用
			if ( KeyNum == 0xF1 ) {//显示FFT结果1
				for ( i = 0 ; i<200; i++ ) {
					GUI_ShowNum ( i%10*48 ,i/10*16,Fft_Real[i],5,16);}
				}//KeyNum == 0xF1
			if ( KeyNum == 0xF2 ) {//显示FFT结果2
				for ( i = 0 ; i<200; i++ ) {
					GUI_ShowNum ( i%10*48 ,i/10*16,Fft_Image[i],5,16);}
			} //KeyNum == 0xF2
	  } //KeyFlag == 1
  } //while(1)
}//


void INTTIMER0 (void) interrupt 1 //定时器0中断
{	
	 u8   i ;
	 OUT0  = 1 ;   //(1)中断开始时OUT置高,结束时置低,测试中断所需时间,调试用
	//(2)---利用SPI通讯,读取AD结果,并设置下次AD的参数
	if (TongDao) {
	 ADCS  = 0 ;
	 ADRes = AD7323(0x84) ;	 		 
	 ADRes = ((ADRes*256 + AD7323(0x30))>>1) -0x2000;            //控制字:通道0对地,正常模式,二进制补码,同时读回通道0
	 //ADRes = ((ADRes*256 + AD7323(0x30))>>1);	
	 ADCS = 1 ;	 
	//ADRes = AD7323(0x8430)-0x2000;
   //GUI_ShowNum (120,120,ADRes,10,16);		
   }
	else {
	 ADCS  = 0 ;
	 ADRes = AD7323(0x80) ;	 		 
	 ADRes = (ADRes*256 + AD7323(0x30))>>1;            //控制字:通道0对地,正常模式,二进制补码,同时读回通道0
	 ADCS = 1 ;	
   //ADRes = AD7323(0x8030);
		//GUI_ShowNum (120,120,ADRes,10,16);	
		}
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
