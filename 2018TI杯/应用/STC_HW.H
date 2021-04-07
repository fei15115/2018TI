//-----------------------------------------------
// 版权归    SivaYao   所有
// 第一次编程时间:2014年07月23日
//-----------------------------------------------
// 文件名：  STC_HW.H
// 编写人：  SivaYao 
// 编写时间：2013/05/26
// 目标器件: STC15F2K60S2或者IAP15F2K61S2
// 编译工具: Keil uV3
//-----------------------------------------------
// 程序说明:
// 此函数库用于STC8K小车上硬件(AD,
//     CCP,SPI,定时器,外部中断,串口等)控制程序的头文件
//-----------------------------------------------/

#ifndef  __STCHW
#define  __STCHW

#include      "STC8A8K.H"  //60S2特殊寄存器声明
#include      <intrins.h>

//***********************************************
//            一/系统设定
//***********************************************
#define    u8        unsigned char		//
#define    u16       unsigned int		//
#define    u32       unsigned long int   //
#define    s8        signed char		    //
#define    s16       signed int		    //
#define    s32       signed long int     //
//1.1--------最小系统参数设定--------------------
#define    SYSTEM_OSC     24000000  //定义系统时钟
#define    Bond           38400     //定义串口波特率


//***********************************************
//            二/端口定义
//***********************************************
/*2.3---片上硬件端口定义(片上定义)-------------*/
sbit     Tmr0      = P3^4 ;	 //定时器0   端口定义
sbit     Int_X0    = P3^2 ;	 //外部中断0 端口定义
sbit     Int_X1    = P3^3 ;	 //外部中断1 端口定义
sbit     RXD       = P3^0 ;	 //串口1RXD线端口定义
sbit     TXD       = P3^1 ;	 //串口1TXD线端口定义
sbit     RXD2      = P1^0 ;	 //串口2RXD线端口定义
sbit     TXD2      = P1^1 ;	 //串口2TXD线端口定义
sbit     CCP0      = P2^5 ;	 //CPA模块CCP0  定义
sbit     CCP1      = P2^6 ;	 //CPA模块CCP1  定义
sbit     CCP2      = P2^7 ;	 //CPA模块CCP2  定义
sbit     Ain2      = P1^2 ;	 //AD输入线2端口定义
sbit     Ain3      = P1^3 ;	 //AD输入线2端口定义
sbit     Ain4      = P1^4 ;	 //AD输入线2端口定义
sbit     Ain5      = P1^5 ;	 //AD输入线2端口定义
sbit     SS        = P4^3 ;  //SS选通线
sbit     DACS1       = P4^0 ;
sbit     DACS2       = P6^4 ;
sbit     DACS3       = P4^5 ;



sbit     SFCS      = P4^3 ;  //SFlash选通线

//***********************************************
//            三/函数声明
//***********************************************
/*4.1---系统硬件初始化子程序声明----------*/
void     SystemInit (void) ;//STC8A8K硬件设置

/*4.4---SPI相关子程序声明-----------------*/

void     SPI_Conf  ( void );      //SPI模块配置程序
u8       SPI_Send  ( u8 SPI_DATA);//SPI字节发送接收程序
u8       SPI_Recv  ( void )		 ;//SPI字节接收程序

/*4.5---SPI附属相关硬件子程序声明---------*/





#endif