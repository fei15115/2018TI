//############################################################################################################################//
//-------------------------------------LCD驱动程序（适用于正点原子屏幕）------------------------------------------------------//
//作者:SivaYao----------------------------------------------------------------------------------------------------------------//
//时间:2016年7月23日----------------------------------------------------------------------------------------------------------//
//说明:使用时请更改端口-------------------------------------------------------------------------------------------------------//
//版本修改情况：--------------------------------------------------------------------------------------------------------------//
//-------2016.7.23------------添加驱动程序------------------------------------------------------------------------------------//
//############################################################################################################################//
//=====================================================================================start====================================================================================================//
#ifndef __LCD_ALI_H__
#define __LCD_ALI_H__
//--------------------------------------------------------------------------头文件-------------------------------------------------------------------------------------------//
#include "STC8A8K.h"
//-------------------------------------------------------------------------端口声明------------------------------------------------------------------------------------------//                     
//一/------------------液晶端口定义----------------------------------
#define  LCD_DataPortH P0    //高8位数据口,8位模式下只使用高8位 
#define  LCD_DataPortL P2    //低8位数据口,8位模式下低8位可以不接线,请确认P0口已经上拉10K电阻,不宜太小，最小4.7K,推荐10K.
sbit     LCD_CS = P4^1;		   //液晶片选	
sbit     LCD_RS = P7^3;  		 //数据/命令切换
sbit     LCD_WR = P4^2;		   //写控制
sbit     LCD_RESET = P7^4;	 //复位 
//二/--------------------用户操作定义--------------------------------
//支持横竖屏快速定义切换，支持8/16位模式切换
#define USE_HORIZONTAL  	1	//定义是否使用横屏 		0,不使用.1,使用.
//三/--------------------类型定义------------------------------------	 
#define    u8        unsigned char		//
#define    u16       unsigned int		//
#define    u32       unsigned long int   //
#define    s8        signed char		    //
#define    s16       signed int		    //
#define    s32       signed long int     //
//-------------------------------------------------------------------------类型定义------------------------------------------------------------------------------------------//
typedef struct typFNT_GB16                 // 汉字字模数据结构
{
       signed char Index[2];               // 汉字内码索引
       char Msk[32];                       // 点阵码数据
};
typedef struct  
{										    
	u16 width;			  //LCD 宽度
	u16 height;			  //LCD 高度
	u16 id;				    //LCD ID
	u8  dir;			    //横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16	 wramcmd;		  //开始写gram指令
	u16  setxcmd;		  //设置x坐标指令
	u16  setycmd;		  //设置y坐标指令	 
}_lcd_dev; 
//----------------------------------------------------------------------全局变量声明-----------------------------------------------------------------------------------------//
extern		unsigned int FColor  ;	// = 0x0000 ;//当前色(缺省为黑)
extern		unsigned int BColor  ;	// = 0xFFFF ;//背景色(缺省为白)
extern code unsigned int COLOR[] ; 			// = {0xFFFF ,0xF800 ,0x07E0 ,0x001F ,0xFFE0 ,0xF81F ,0x07FF ,0x0000};//白,红,绿,深蓝,黄,紫,天蓝,黑		//颜色数组
extern _lcd_dev lcddev;	     //管理LCD重要参数
//-------------------------------------------------------------------------函数声明-----------------------------------------------------------------------------------------//
void  delay_ms(int count);  // /* X1ms */
//---------------------------------------------------液晶相关子程序说明-----------------------------------------------------------//
void  LCD_WIR(int Reg);     //写入指令寄存器IR
void  LCD_WDR(int Data);    //写入数据寄存器DR
void  LCD_WriteRAM_Prepare(void);//0x2C指令
void  LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);//设置显示窗口
void  LCD_SetCursor(u16 Xpos, u16 Ypos);//设置光标位置
void  LCD_SetParam(void);   //液晶参数设置
void  LCD_Init(void);       //液晶初始化程序
void  LCD_Clear(u16 Color); //液晶清屏
void  SystemInit (void);
//------------------------------------------------------内部函数------------------------------------------------------------------//
void _draw_circle_8(u16 xc, u16 yc, u16 x, u16 y, u16 Color);
u32   mypow(u8 m,u8 n);
//---------------------------------------------------绘图相关子程序说明-----------------------------------------------------------//
void  GUI_DrawPoint(u16 X ,u16 Y,u16 Color);  //(1)画点子程序
void  GUI_Horz     (u16 X ,u16 Y ,u16 Width); //(2)LCD画水平线
void  GUI_Vert     (u16 X ,u16 Y ,u16 Height);//(3)LCD画垂直线
void  GUI_Line(u16 X1, u16 Y1, u16 X2, u16 Y2);   //(6)画线子程序   
void  GUI_Rectangle(u16 x1, u16 y1, u16 x2, u16 y2);//(4)画四边形
void  GUI_FillRec(u16 SX,u16 SY,u16 EX,u16 EY,u16 Color);//(5)方形填充子程序
void  GUI_Circle(u16 xc, u16 yc,u16 Color,u16 r, u8 Fill);//(7)画圆子程序(Fill:0不填充:1填充)
void  GUI_Bmp16 (u16 X1, u16 Y1,u16 X2, u16 Y2,const u8 *p); //(8)//显示图片(16位色)
void  DisP_Photo16_pro(unsigned int X1,unsigned int Y1,unsigned int X_len,unsigned int Y_len,unsigned char *P);			  //显示彩色图像
void  DisP_Photo  (unsigned int X1,unsigned int Y1,unsigned int X2,unsigned int Y2,unsigned char *P,unsigned char INVERT);	//显示黑白图像
//----------------------------------------------------字符和字符串显示------------------------------------------------------------//
void  GUI_ShowChar(u16 X,u16 Y,u16 fc, u16 bc, u8 num,u8 Size,u8 mode);//(9)显示字符
void  GUI_ShowStr (u16 X,u16 Y,u8 Size,u8 *p,u8 mode);  //(10)显示字符串
void  GUI_ShowNum (u16 X,u16 Y,u32 Num,u8 Len,u8 Size); //(11)显示数字
//-----------------------------------------------------------中文显示-------------------------------------------------------------//
void  GUI_Font16( u16 X,u16 Y,u16 FC, u16 BC, u8 *Str,u8 Mode); 
void  GUI_Font24( u16 X,u16 Y,u16 FC, u16 BC, u8 *Str,u8 Mode);
void  DisP_Hanzi(unsigned int x,unsigned int y,struct typFNT_GB16 *p,unsigned char num,unsigned char INVERT);	 //显示汉字
//-------------------------------------------------------------------------颜色定义-----------------------------------------------------------------------------------------//
//一/画笔颜色
#define WHITE      0xFFFF
#define BLACK      0x0000	  
#define BLUE       0x001F  
#define BRED       0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED        0xF800
#define MAGENTA    0xF81F
#define GREEN      0x07E0
#define CYAN       0x7FFF
#define YELLOW     0xFFE0
#define BROWN 		 0XBC40 //棕色
#define BRRED 		 0XFC07 //棕红色
#define GRAY  		 0X8430 //灰色
//二/GUI颜色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//三/以上三色为PANEL的颜色
#define LIGHTGREEN   0X841F //浅绿色
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE    0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE       0X2B12 //浅棕蓝色(选择条目的反色)
//=====================================================================================end====================================================================================================//
#endif  
	 
	 



