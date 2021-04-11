//-----------------------------------------------
// 版权归    SivaYao   所有
// 第一次编程时间:2014年07月23日
//-----------------------------------------------
// 文件名：  HD7279.H
// 编写人：  SivaYao 
// 编写时间：2013/05/26
// 目标器件: STC15F2K60S2或者IAP15F2K61S2
// 编译工具: Keil uV3
//-----------------------------------------------
// 程序说明:
// 此函数库用于STC15F2K60S2最小系统板上HD7279所驱
//     动按键显示的控制程序的头文件
//-----------------------------------------------/


#include      "STC8A8K.H"  //STC15W4K特殊寄存器声明
#include      <intrins.h>
//***********************************************
//            一/系统设定
//***********************************************
#define       u8        unsigned char		//
#define       u16       unsigned int		//
#define       u32       unsigned long int   //
#define       s8        signed char		    //
#define       s16       signed int		    //
#define       s32       signed long int     //
//1.1--------最小系统参数设定--------------------
#define       SYSTEM_OSC     33000000  //定义系统时钟
#define       Bond           38400     //定义串口波特率

#define       CMD_RESET      0xa4
#define       CMD_TEST       0xbf
#define       DECODE0        0x80
#define       DECODE1        0xc8
#define       CMD_READ       0x15
#define       UNDECODE       0x90
#define       RTL_CYCLE      0xa3
#define       RTR_CYCLE      0xa2
#define       RTL_UNCYL      0xa1
#define       RTR_UNCYL      0xa0
#define       ACTCTL         0x98
#define       SEGON          0xe0
#define       SEGOFF         0xc0
#define       BLINKCTL       0x88
//***********************************************
//            二/端口定义
//***********************************************
/*2.1---HD7279端口定义--------------------*/
sbit     CS7279    = P4^2 ;	 //HD7279选通线定义
sbit     CLK       = P7^3 ;	 //HD7279时钟线定义
sbit     DAT       = P7^2 ;	 //HD7279数据线定义

//***********************************************
//            三/函数说明
//***********************************************
/*4.1---HD7279相关子程序说明--------------*/

void     SEND7279B      (u8 DATAOUT);  //HD7279发送字节
u8       RECEIVE7279B   (void);	       //HD7279接收字节
void     DELAY7279      (u8 num);      //HD7279延时子程序
void     SEND7279_CMD   (u8 CMD);      //HD7279发送单字节指令子程序
void     SEND7279_CMD_DATA(u8 CMD ,u8 DATA);//HD7279发送带数据指令子程序
u8       READ7279_KEY   (void);        //从HD7279读取按键键码
u8       READ7279_NUM   (void);		   //从HD7279读取按键键值

