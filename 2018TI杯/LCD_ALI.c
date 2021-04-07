//############################################################################################################################//
//-------------------------------------LCD驱动程序（适用于正点原子屏幕）------------------------------------------------------//
//作者:SivaYao----------------------------------------------------------------------------------------------------------------//
//时间:2016年7月23日----------------------------------------------------------------------------------------------------------//
//说明:使用时请更改端口-------------------------------------------------------------------------------------------------------//
//版本修改情况：--------------------------------------------------------------------------------------------------------------//
//-------2016.7.23------------添加驱动程序------------------------------------------------------------------------------------//
//############################################################################################################################//
//=====================================================================================start====================================================================================================//
#include "LCD_ALI.h"
#include "font.h" 
#include "string.h"

//管理LCD重要参数//默认为竖屏
_lcd_dev lcddev;



//--------------------------------------------------------------//
//函数:delay_ms     功能:延时
//输入:count   返回值:  无
//--------------------------------------------------------------//
void delay_ms(int count)  // /* X1ms */
{
   int i,j;
   for(i=0;i<count;i++)
      for(j=0;j<1000;j++);
}  
//--------------------------------------------------------------//
//函数:LCD_WIR     功能:写入16位指令
//输入:Reg指令值   返回值:  无
//--------------------------------------------------------------//
void LCD_WIR(int Reg)	 
{  EA=0 ;	
	 LCD_RS=0;
	 LCD_CS=0;
	 LCD_DataPortH=Reg>>8;
	 LCD_DataPortL=Reg;
	 LCD_WR=0;
	 LCD_WR=1;
	 LCD_CS=1;
	 EA =1 ;
} 
//--------------------------------------------------------------//
//函数:LCD_WDR     功能:写入16位数据
//输入:Data数据值  返回值:  无
//--------------------------------------------------------------//
void LCD_WDR(int Data)
{  EA = 0 ;
	 LCD_RS=1;
	 LCD_CS=0;
	 LCD_DataPortH=(Data)>>8;
	 LCD_DataPortL=(Data);
	 LCD_WR=0;
	 LCD_WR=1;
	 LCD_CS=1;
	 EA = 1 ;
}
//--------------------------------------------------------------//
//函数:LCD_WriteRAM_Prepare     功能:写入画点指令
//输入:指令值0x2C   返回值:  无
//--------------------------------------------------------------//
void LCD_WriteRAM_Prepare(void)
{
 	 LCD_WIR(lcddev.wramcmd);	  
}
//--------------------------------------------------------------//
//函数:LCD_SetWindows   功能:设置显示窗口
//输入:xStar,yStar,xEnd,yEnd四角坐标  返回值:  无
//--------------------------------------------------------------//
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
	 LCD_WIR(lcddev.setxcmd);	
	 LCD_WDR(xStar>>8);
	 LCD_WDR(0x00FF&xStar);		
	 LCD_WDR(xEnd>>8);
	 LCD_WDR(0x00FF&xEnd);

	 LCD_WIR(lcddev.setycmd);	
	 LCD_WDR(yStar>>8);
	 LCD_WDR(0x00FF&yStar);		
	 LCD_WDR(yEnd>>8);
	 LCD_WDR(0x00FF&yEnd);

	 LCD_WriteRAM_Prepare();	//开始写入GRAM			
} 
//--------------------------------------------------------------//
//函数:LCD_SetCursor    功能:设置光标位置
//输入:Xpos,Ypos光标坐标  返回值:  无
//--------------------------------------------------------------//
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	 LCD_WIR(lcddev.setxcmd);	
	 LCD_WDR(Xpos>>8);
	 LCD_WDR(0x00FF&Xpos);		
	 LCD_WDR((Xpos+1)>>8);
	 LCD_WDR((Xpos+1));
	
	 LCD_WIR(lcddev.setycmd);	
	 LCD_WDR(Ypos>>8);
	 LCD_WDR(0x00FF&Ypos);		
	 LCD_WDR((Ypos+1)>>8);
	 LCD_WDR((Ypos+1));
	 LCD_WriteRAM_Prepare();	//开始写入GRAM		
} 
//--------------------------------------------------------------//
//函数名：  LCD_SetParam
//功能：    设置LCD参数，方便进行横竖屏模式切换
//--------------------------------------------------------------//
void LCD_SetParam(void)
{ 
	 lcddev.setxcmd=0x2A;
	 lcddev.setycmd=0x2B;
	 lcddev.wramcmd=0x2C;
#if USE_HORIZONTAL==1	//使用横屏	  
	 lcddev.dir=1;//横屏
	 lcddev.width=480;
	 lcddev.height=320;			
	 LCD_WIR(0x36);
	 LCD_WDR((1<<3)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
#else//竖屏
	 lcddev.dir=0;//竖屏				 	 		
	 lcddev.width=320;
	 lcddev.height=480;	
	 LCD_WIR(0x36);
	 LCD_WDR((1<<3)|(1<<6));//BGR==1,MY==0,MX==0,MV==0
#endif
}
//--------------------------------------------------------------//
//函数名：  LCD_Init    功能：    LCD初始化
//--------------------------------------------------------------//
void LCD_Init(void)
{
	LCD_RESET=1;
	delay_ms(50);	
	LCD_RESET=0;
	delay_ms(50);
	LCD_RESET=1;
	delay_ms(50);
//	LCD_RD=1;	   //RD引脚没有用到，拉高处理
//=========================液晶屏初始化=============================//
	LCD_WIR(0x0011);
	delay_ms(100);
	LCD_WIR(0x0013);
	delay_ms(100);

	LCD_WIR(0x0036);	LCD_WDR(0x0002);//02);
  
	LCD_WIR(0x003A);	LCD_WDR(0x0005);
//  reg F0 5A 5A
	LCD_WIR(0x00F0);	LCD_WDR(0x005A);	LCD_WDR(0x005A);
//reg F1 5A 5A
	LCD_WIR(0x00F1);	LCD_WDR(0x005A);	LCD_WDR(0x005A);
//reg F7 00 00 10 02 00
	LCD_WIR(0x00F7);	LCD_WDR(0x0000);	LCD_WDR(0x0000);	LCD_WDR(0x0010);	LCD_WDR(0x0002);	LCD_WDR(0x0000);
//reg F8 11 00
	LCD_WIR(0x00F8);	LCD_WDR(0x0011);	LCD_WDR(0x0000);
//reg F9 17
	LCD_WIR(0x00F9);	LCD_WDR(0x0017);
//reg 44 00 01
	LCD_WIR(0x0044);	LCD_WDR(0x0000);	LCD_WDR(0x0001);
//reg F2 
//3B 30 03 08 
//08 08 08 00 
//08 08 00 00 
//00 00 30 08 
//08 08 08
	LCD_WIR(0x00F2);	LCD_WDR(0x003b);	LCD_WDR(0x0030);	LCD_WDR(0x0003);
	LCD_WDR(0x0008);	LCD_WDR(0x0008);	LCD_WDR(0x0008);	LCD_WDR(0x0008);
	LCD_WDR(0x0000);	LCD_WDR(0x0008);	LCD_WDR(0x0008);	LCD_WDR(0x0000);
	LCD_WDR(0x0000);	LCD_WDR(0x0000);	LCD_WDR(0x0000);	LCD_WDR(0x0030);
	LCD_WDR(0x0008);	LCD_WDR(0x0008);	LCD_WDR(0x0008);	LCD_WDR(0x0008);
//reg F4 
//06 00 00 00 
//00 00 00 00 
//00 52 02 00 
//52 02
	LCD_WIR(0x00F4);	LCD_WDR(0x0006);	LCD_WDR(0x0000);	LCD_WDR(0x0000);
	LCD_WDR(0x0000);	LCD_WDR(0x0000);	LCD_WDR(0x0000);	LCD_WDR(0x0000);
	LCD_WDR(0x0000);	LCD_WDR(0x0000);	LCD_WDR(0x0052);	LCD_WDR(0x0002);
	LCD_WDR(0x0000);	LCD_WDR(0x0052);	LCD_WDR(0x0002);
//reg F5 00 31 54 00 00 04 00 00 31 54?????
	LCD_WIR(0x00F5);	LCD_WDR(0x0000);	LCD_WDR(0x0031);	LCD_WDR(0x0054);
	LCD_WDR(0x0000);	LCD_WDR(0x0000);	LCD_WDR(0x0004);	LCD_WDR(0x0000);
	LCD_WDR(0x0000);	LCD_WDR(0x0031);	LCD_WDR(0x0054);
//reg F6 00 00 08 03 01 00 01 00 
	LCD_WIR(0x00F6);	LCD_WDR(0x0000);	LCD_WDR(0x0000);	LCD_WDR(0x0008);
	LCD_WDR(0x0003);	LCD_WDR(0x0001);	LCD_WDR(0x0000);	LCD_WDR(0x0001);
	LCD_WDR(0x0000);
//reg FA 
//04 37 00 20 
//21 1F 2A 19 
//28 21 2E 2B 
//19 00 00 00
	LCD_WIR(0x00Fa);	LCD_WDR(0x0004);	LCD_WDR(0x0037);	LCD_WDR(0x0000);
	LCD_WDR(0x0020);	LCD_WDR(0x0021);	LCD_WDR(0x001f);	LCD_WDR(0x002a);
	LCD_WDR(0x0019);	LCD_WDR(0x0028);	LCD_WDR(0x0021);	LCD_WDR(0x002e);
	LCD_WDR(0x002b);	LCD_WDR(0x0019);	LCD_WDR(0x0000);	LCD_WDR(0x0000);
	LCD_WDR(0x0000);
//reg FB 
//37 00 0E 2F 
//30 1F 16 14 
//30 2A 18 1D 
//00 00 00 00 //色调
	LCD_WIR(0x00Fb);	LCD_WDR(0x0057);	LCD_WDR(0x0022);	LCD_WDR(0x000e);
	LCD_WDR(0x002f);	LCD_WDR(0x0030);	LCD_WDR(0x001f);	LCD_WDR(0x0016);
	LCD_WDR(0x0014);	LCD_WDR(0x0030);	LCD_WDR(0x002a);	LCD_WDR(0x0018);
	LCD_WDR(0x001d);	LCD_WDR(0x0019);	LCD_WDR(0x0000);	LCD_WDR(0x0000);
	LCD_WDR(0x0000);
	delay_ms(120);
	LCD_WIR(0x0029);		

	//设置LCD属性参数
	lcddev.wramcmd=0x2C;
#if USE_HORIZONTAL==1	//使用横屏	  
	lcddev.dir=1;//横屏
	lcddev.width=480;
	lcddev.height=320;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
  LCD_WIR(0x36);
  LCD_WDR(0xE8);	
#else//竖屏
	lcddev.dir=0;//竖屏				 	 		
	lcddev.width=320;
	lcddev.height=480;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
  LCD_WIR(0x36);
  LCD_WDR(0x48);	
#endif 
//	LCD_BL=1;//点亮背光	 
}
//--------------------------------------------------------------//
//函数:LCD_Clear   功能:清屏子程序
//输入:Color清屏颜色  返回值:  无
//--------------------------------------------------------------//
void LCD_Clear(u16 Color)
{
	 u32 i,POW=0;
	 POW = (u32)lcddev.width*lcddev.height ;
	 LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
	 for(i=0;i<POW;i++){
		  LCD_WDR(Color);		  }	
   } 
//--------------------------------------------------------------//
//函数名:GUI_DrawPoint    功能:描绘一个点
//参数：x,y:坐标color:颜色  出口参数：无 
//--------------------------------------------------------------//
void GUI_DrawPoint(u16 X,u16 Y,u16 Color)
{
	 LCD_SetCursor(X,Y);//设置光标位置 
	 LCD_WDR(Color);  
}
//--------------------------------------------------------------//
//函数名:GUI_Horz     功能:画水平线
//参数：X,Y:起点坐标Width:长度 出口参数：无 
//--------------------------------------------------------------//
void GUI_Horz(u16 X ,u16 Y ,u16 Width)    //画水平线
{
	 LCD_SetWindows(X,Y,X+Width-1,Y);  //设置绘图区域
	 while ( Width-- )
	    LCD_WDR(FColor);				   //填充前景色
}
//--------------------------------------------------------------//
//函数名:GUI_Verz     功能:画垂直线
//参数：X,Y:起点坐标Height:高度 出口参数：无 
//--------------------------------------------------------------//
void GUI_Vert(u16 X ,u16 Y ,u16 Height)   //画垂直线
{
	 LCD_SetWindows(X,Y,X,Y+Height-1);   //设置绘图区域
	 while ( Height-- )
	    LCD_WDR(FColor);				   //填充前景色
}
//--------------------------------------------------------------//
//函数:GUI_Line   功能:当前色画一条直线
//参数：x1,y1,x2,y2:起点终点坐标  出口参数：无 
//--------------------------------------------------------------//
void GUI_Line(u16 x1, u16 y1, u16 x2, u16 y2)
{
	 u16 t; 
	 int xerr=0,yerr=0,delta_x,delta_y,distance; 
	 int incx,incy,uRow,uCol; 

	 delta_x=x2-x1; //计算坐标增量 
	 delta_y=y2-y1; 
	 uRow=x1; 
	 uCol=y1; 
	 if (delta_x>0)
		  incx=1; //设置单步方向 
	 else
		  if (delta_x==0)
			   incx=0;//垂直线 
	   else {
         incx=-1;delta_x=-delta_x;} 
	 if (delta_y>0)
		  incy=1; 
	 else 
		  if (delta_y==0 )
				 incy=0;//水平线 
	    else {
         incy=-1;delta_y=-delta_y;} 
	 if ( delta_x > delta_y ) 
		  distance=delta_x; //选取基本增量坐标轴 
	 else
  	  distance=delta_y; 
	 for ( t=0 ; t<=distance+1 ; t++ ) {  
		  GUI_DrawPoint(uRow,uCol,FColor);//画点 
		  xerr+=delta_x ; 
		  yerr+=delta_y ; 
		  if ( xerr > distance ) { 
			   xerr-=distance; 
			   uRow+=incx; } 
		  if ( yerr > distance ) { 
			   yerr-=distance; 
			   uCol+=incy; } 
	    }  
} 
//--------------------------------------------------------------//
//函数名:GUI_Rectangle     功能:画四边形
//参数：X1,Y1,X2,Y3:四顶点坐标      出口参数：无 
//--------------------------------------------------------------//
void GUI_Rectangle(u16 X1, u16 Y1, u16 X2, u16 Y2)
{
	 GUI_Horz(X1,Y1,X2-X1+1);
	 GUI_Horz(X1,Y2,X2-X1+1);
	 GUI_Vert(X1,Y1,Y2-Y1+1);
	 GUI_Vert(X2,Y1,Y2-Y1+1);
} 
//--------------------------------------------------------------//
//函数:GUI_FillRec   功能:填充一个方形区域
//参数：SX,SY,EX,EY:四角坐标Color:颜色  出口参数：无 
//--------------------------------------------------------------//
void GUI_FillRec(u16 SX,u16 SY,u16 EX,u16 EY,u16 Color)
{  	
	 u16 i,j;			
	 u16 width =EX-SX+1; 		//得到填充的宽度
	 u16 height=EY-SY+1;		//高度
	 LCD_SetWindows(SX,SY,EX,EY);//设置显示窗口
	 //LCD_SetWindows(SX,SY,EX-1,EY-1);//设置显示窗口？？？
	 for ( i=0 ; i<height ; i++ ) {
		  for ( j=0 ; j<width ; j++)
		     LCD_WDR(Color);}	//写入数据 	 	
	 LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口设置为全屏
}
//--------------------------------------------------------------//
//*****内部程序:对称性画点用********************* 
//--------------------------------------------------------------//
void _draw_circle_8(u16 xc, u16 yc, u16 x, u16 y, u16 Color)
{
	 GUI_DrawPoint(xc + x, yc + y, Color);
	 GUI_DrawPoint(xc - x, yc + y, Color);
	 GUI_DrawPoint(xc + x, yc - y, Color);
	 GUI_DrawPoint(xc - x, yc - y, Color);
	 GUI_DrawPoint(xc + y, yc + x, Color);
	 GUI_DrawPoint(xc - y, yc + x, Color);
	 GUI_DrawPoint(xc + y, yc - x, Color);
	 GUI_DrawPoint(xc - y, yc - x, Color);
   }
//--------------------------------------------------------------//
//函数:GUI_Circle   功能:画圆子程序
//参数：xc,yc:圆中心r:半径Color:色彩Fill:填充
//--------------------------------------------------------------//   
void GUI_Circle(u16 xc, u16 yc,u16 Color,u16 r, u8 fill)
{
	 int x = 0, y = r, yi, d;
	 d = 3 - 2 * r;
	 if (fill) 	{		// 如果填充（画实心圆）
		  while (x <= y) {
			   for (yi = x; yi <= y; yi++)
				    _draw_circle_8(xc, yc, x, yi, Color);
			   if (d < 0) {
				    d = d + 4 * x + 6;}
 	       else {
				    d = d + 4 * (x - y) + 10;
            y--;}
			   x++;}
	    }
	 else {  // 如果不填充（画空心圆）
		  while (x <= y) {
			   _draw_circle_8(xc, yc, x, y, Color);
			   if (d < 0) {
				    d = d + 4 * x + 6;}
        else {
				    d = d + 4 * (x - y) + 10;
				    y--;}
			  x++;}
	    }
}
//--------------------------------------------------------------//
//函数:GUI_Bmp16   功能:图片绘制
//参数：X1,Y1:起点X2,Y2:终点*P:图片指针
//--------------------------------------------------------------//
void GUI_Bmp16(u16 X1,u16 Y1,u16 X2,u16 Y2,const u8 *p) //显示图片
{
   u16 i; 
	 u8 picH,picL; 
	 LCD_SetWindows(X1,Y1,X2,Y2);//窗口设置
   for ( i=0 ; i<(X2-X1+1)*(Y2-Y1+1) ; i++ ){	
	 	  picL=*(p+i*2)  ;	//数据低位在前
		  picH=*(p+i*2+1);				
		  LCD_WDR(picH<<8|picL);  						}	
	 LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复显示窗口为全屏	
}
//15/----------------------------------------------------------LCD黑白图填充----------------------------------------------------//
//说明：LCD画黑白图像子程序
//入口：X1,X2,Y1,Y2为起始结束地址，即要填充图片的大小，p为指针，指向需要填充内容的数组，INVERT=0为白底黑字，1为黑底白字
//出口：无
//-----------------------------------------------------------------------------------------------------------------------------//
void     DisP_Photo  (unsigned int X1,unsigned int Y1,unsigned int X2,unsigned int Y2,unsigned char *P,unsigned char INVERT)
{   //要求X1<X2 , Y1 < Y2;
    unsigned char    j,k,LCDDATA;
    unsigned int   Capacity,i;
    
    LCD_SetWindows( X1, Y1, X2, Y2)	 ;
        
    Capacity = (X2 - X1 +1)*(Y2 - Y1 +1) ;
//	SET_FOCUS  (X1,Y1);   
    for (i=0 ; i<Capacity/8; i++)
    {
        LCDDATA = *P ;
        P++ ;
        k       = 0x80 ;	 //读取一个字节数据,画8个点
        for (j=0 ; j<8 ; j++)
        {
            if (LCDDATA & k)
            {
                if (INVERT)
                    { LCD_WDR (BColor) ;}
                else
                    { LCD_WDR (FColor) ;}         
            }
            else
            {
                if (INVERT)
                    { LCD_WDR (FColor) ;}
                else
                    { LCD_WDR (BColor) ;}                   
			}
            k = k / 2 ;            
		}
	}   
}
//21/--------------------------------LCD彩色图填充,参数为初始位置及图片大小----------------------------------------------------//
//说明：LCD画彩色图像子程序,参数为初始位置及图片大小
//入口：X1,Y1为起始地址，即要填充图片的左上角需要的位置，X_len,Y_len为图片长宽，p为指针，指向需要填充内容的数组
//出口：无
//-----------------------------------------------------------------------------------------------------------------------------//
void DisP_Photo16_pro(unsigned int X1,unsigned int Y1,unsigned int X_len,unsigned int Y_len,unsigned char *P)
{
	GUI_Bmp16(X1,Y1,X1+X_len-1,Y1+Y_len-1,P);
}
//20/----------------------------------------------------------LCD汉字显示程序------------------------------------------------//
//说明：显示汉字
//入口：x,y为显示字符的位置（若将字符视为图片，则X1,Y1为左上角的点），P为待显示的汉字数组的数组名，num为需要显示的汉字个数
//出口：无
//-----------------------------------------------------------------------------------------------------------------------------//
void DisP_Hanzi(unsigned int x,unsigned int y,struct typFNT_GB16 *p,unsigned char num,unsigned char INVERT)
{
	unsigned char *pt,i=0,size;
	size=num*34;
	//show_float_5(size);
	while(size)
	{
		size-=34;
		pt=(p+i)->Msk;
		DisP_Photo(x+17*i,y,x+17*i+16-1,y+16-1,pt,INVERT);
		i++;
	}		
}
//--------------------------------------------------------------//
//函数:GUI_ShowChar  功能:字符绘制
//参数：X,Y:起点  FC,BC:前景色和背景色
//	    ASCII:ASCII码,Size:大小16(0816)和12(0612)
//	    Mode:0非叠加1叠加
//--------------------------------------------------------------//
void GUI_ShowChar(u16 X,u16 Y,u16 FC, u16 BC, u8 ASCII,u8 Size,u8 Mode)
{  
   u8 temp;
   u8 pos,t;
	 u16 colortemp=FColor;      
		   
	 ASCII = ASCII -' '  ;//得到偏移后的值
	 LCD_SetWindows ( X , Y , X+Size/2-1,Y+Size-1);//设置单个字符显示窗口
	 if(!Mode) {//非叠加方式		
		  for ( pos=0 ; pos<Size ; pos++ ) {
			   if ( Size == 12 )
					  temp=asc2_1206[ASCII][pos];//调用1206字体
			   else
 	          temp=asc2_1608[ASCII][pos];//调用1608字体
			   for ( t=0 ; t<Size/2 ; t++ ){                 
		        if ( temp&0x01 ) 
							  LCD_WDR ( FC ); 
				    else
  							LCD_WDR ( BC ); 
				    temp>>=1; }
			   }	
	    }
	 else  {
		  for ( pos=0 ; pos<Size ; pos++){
			   if ( Size == 12 ) 
					  temp=asc2_1206[ASCII][pos];  //调用1206字体
			   else
					  temp=asc2_1608[ASCII][pos];	 //调用1608字体
			   for ( t=0 ; t<Size/2 ; t++ ){   
				    FColor=FC;              
		        if(temp&0x01)
							  GUI_DrawPoint( X+t , Y+pos ,FColor);//画一个点    
		        temp>>=1; }
		     }
	    }
	 FColor=colortemp;	
	 LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏    	   	 	  
}
//--------------------------------------------------------------//
//函数:GUI_ShowStr  功能:显示字符串(英文)
//参数：X,Y:起点 ,Size:大小16(0816)和12(0612) 
//	    *P:字符串指针	    Mode:0非叠加1叠加
//--------------------------------------------------------------//	 
void GUI_ShowStr(u16 X,u16 Y,u8 Size,u8 *p,u8 Mode)
{         
   while((*p<='~')&&(*p>=' ')) {   //判断是不是非法字符!      
		  if ( X > (lcddev.width-1)|| Y > (lcddev.height-1)) //判断位置
		     return;     
      GUI_ShowChar(X,Y,FColor,BColor,*p,Size,Mode);//显示单个字符
      X+=Size/2; //调整X坐标位置
      p++;}  
} 
//--------------------------------------------------------------//
//内部函数:求幂
//--------------------------------------------------------------//
u32 mypow(u8 m,u8 n)
{
	 u32   result = 1;	 
	 while ( n-- )
		  result *= m ;    
	 return result;
}
//--------------------------------------------------------------//
//函数:GUI_ShowNum  功能:显示数字
//参数：X,Y:起点 ,Num:显示的数字,Len:显示位数
//     Size:大小16(0816)和12(0612) 
//--------------------------------------------------------------//			 
void GUI_ShowNum(u16 X,u16 Y,u32 Num,u8 Len,u8 Size)
{         	
	 u8 t,temp;
	 u8 enshow=0;						   
	 for ( t=0 ; t<Len ; t++ ){
		  temp = ( Num/mypow(10,Len-t-1) ) %10 ;
		  if( enshow==0 && t<(Len-1) ) {
			   if( temp==0 ){
				    GUI_ShowChar ( X+(Size/2)*t,Y,FColor,BColor,' ',Size,0 );
				    continue;}
				 else
            enshow=1; 		 	 }
	 GUI_ShowChar (X+(Size/2)*t,Y,FColor,BColor,temp+'0',Size,0); }
} 
//--------------------------------------------------------------//
//显示字符
//--------------------------------------------------------------//
void GUI_Font16( u16 X,u16 Y,u16 FC, u16 BC, u8 *Str,u8 Mode)
{
   u8      i,j ;
	 u8      FontB ;
	 LCD_SetWindows( X,Y,X+15,Y+15 ) ; //(1)设置显示区域
	 for ( i = 0 ; i<32 ; i++ ) {      //(2)总共32字节
		  FontB = *Str ; Str ++ ;
		  for ( j = 0 ; j < 8 ; j++ ) {  //(3)8个点
				 if  ( !Mode )  {     //如果非填充方式
            if ( FontB&0x80 ) { 
							 LCD_WDR (FC);  }
						else {
							 LCD_WDR (BC);}	
						}							 
				 else {               //如果是填充方式
            if	( FontB&0x80 ) {
							 GUI_DrawPoint(X+i%2*8+j,Y+i/2,FC);}
						}
				 FontB = FontB << 1 ;
				 }
			}				 
   }
//--------------------------------------------------------------//
//显示字符
//--------------------------------------------------------------//
void GUI_Font24( u16 X,u16 Y,u16 FC, u16 BC, u8 *Str,u8 Mode)
{
   u8      i,j ;
	 u8      FontB ;
	 LCD_SetWindows( X,Y,X+23,Y+23 ) ; //(1)设置显示区域
	 for ( i = 0 ; i<72 ; i++ ) {      //(2)总共72字节
		  FontB = *Str ; Str ++ ;
		  for ( j = 0 ; j < 8 ; j++ ) {  //(3)8个点
				 if  ( !Mode )  {     //如果非填充方式
            if ( FontB&0x80 ) { 
							 LCD_WDR (FC);  }
						else {
							 LCD_WDR (BC);}	
						}							 
				 else {               //如果是填充方式
            if	( FontB&0x80 ) {
							 GUI_DrawPoint(X+i%3*8+j,Y+i/3,FC);}
						}
				 FontB = FontB << 1 ;
				 }
			}				 
   }




 





