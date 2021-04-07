//############################################################################################################################//
//-------------------------------------LCD������������������ԭ����Ļ��------------------------------------------------------//
//����:SivaYao----------------------------------------------------------------------------------------------------------------//
//ʱ��:2016��7��23��----------------------------------------------------------------------------------------------------------//
//˵��:ʹ��ʱ����Ķ˿�-------------------------------------------------------------------------------------------------------//
//�汾�޸������--------------------------------------------------------------------------------------------------------------//
//-------2016.7.23------------�����������------------------------------------------------------------------------------------//
//############################################################################################################################//
//=====================================================================================start====================================================================================================//
#include "LCD_ALI.h"
#include "font.h" 
#include "string.h"

//����LCD��Ҫ����//Ĭ��Ϊ����
_lcd_dev lcddev;



//--------------------------------------------------------------//
//����:delay_ms     ����:��ʱ
//����:count   ����ֵ:  ��
//--------------------------------------------------------------//
void delay_ms(int count)  // /* X1ms */
{
   int i,j;
   for(i=0;i<count;i++)
      for(j=0;j<1000;j++);
}  
//--------------------------------------------------------------//
//����:LCD_WIR     ����:д��16λָ��
//����:Regָ��ֵ   ����ֵ:  ��
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
//����:LCD_WDR     ����:д��16λ����
//����:Data����ֵ  ����ֵ:  ��
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
//����:LCD_WriteRAM_Prepare     ����:д�뻭��ָ��
//����:ָ��ֵ0x2C   ����ֵ:  ��
//--------------------------------------------------------------//
void LCD_WriteRAM_Prepare(void)
{
 	 LCD_WIR(lcddev.wramcmd);	  
}
//--------------------------------------------------------------//
//����:LCD_SetWindows   ����:������ʾ����
//����:xStar,yStar,xEnd,yEnd�Ľ�����  ����ֵ:  ��
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

	 LCD_WriteRAM_Prepare();	//��ʼд��GRAM			
} 
//--------------------------------------------------------------//
//����:LCD_SetCursor    ����:���ù��λ��
//����:Xpos,Ypos�������  ����ֵ:  ��
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
	 LCD_WriteRAM_Prepare();	//��ʼд��GRAM		
} 
//--------------------------------------------------------------//
//��������  LCD_SetParam
//���ܣ�    ����LCD������������к�����ģʽ�л�
//--------------------------------------------------------------//
void LCD_SetParam(void)
{ 
	 lcddev.setxcmd=0x2A;
	 lcddev.setycmd=0x2B;
	 lcddev.wramcmd=0x2C;
#if USE_HORIZONTAL==1	//ʹ�ú���	  
	 lcddev.dir=1;//����
	 lcddev.width=480;
	 lcddev.height=320;			
	 LCD_WIR(0x36);
	 LCD_WDR((1<<3)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
#else//����
	 lcddev.dir=0;//����				 	 		
	 lcddev.width=320;
	 lcddev.height=480;	
	 LCD_WIR(0x36);
	 LCD_WDR((1<<3)|(1<<6));//BGR==1,MY==0,MX==0,MV==0
#endif
}
//--------------------------------------------------------------//
//��������  LCD_Init    ���ܣ�    LCD��ʼ��
//--------------------------------------------------------------//
void LCD_Init(void)
{
	LCD_RESET=1;
	delay_ms(50);	
	LCD_RESET=0;
	delay_ms(50);
	LCD_RESET=1;
	delay_ms(50);
//	LCD_RD=1;	   //RD����û���õ������ߴ���
//=========================Һ������ʼ��=============================//
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
//00 00 00 00 //ɫ��
	LCD_WIR(0x00Fb);	LCD_WDR(0x0057);	LCD_WDR(0x0022);	LCD_WDR(0x000e);
	LCD_WDR(0x002f);	LCD_WDR(0x0030);	LCD_WDR(0x001f);	LCD_WDR(0x0016);
	LCD_WDR(0x0014);	LCD_WDR(0x0030);	LCD_WDR(0x002a);	LCD_WDR(0x0018);
	LCD_WDR(0x001d);	LCD_WDR(0x0019);	LCD_WDR(0x0000);	LCD_WDR(0x0000);
	LCD_WDR(0x0000);
	delay_ms(120);
	LCD_WIR(0x0029);		

	//����LCD���Բ���
	lcddev.wramcmd=0x2C;
#if USE_HORIZONTAL==1	//ʹ�ú���	  
	lcddev.dir=1;//����
	lcddev.width=480;
	lcddev.height=320;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
  LCD_WIR(0x36);
  LCD_WDR(0xE8);	
#else//����
	lcddev.dir=0;//����				 	 		
	lcddev.width=320;
	lcddev.height=480;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
  LCD_WIR(0x36);
  LCD_WDR(0x48);	
#endif 
//	LCD_BL=1;//��������	 
}
//--------------------------------------------------------------//
//����:LCD_Clear   ����:�����ӳ���
//����:Color������ɫ  ����ֵ:  ��
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
//������:GUI_DrawPoint    ����:���һ����
//������x,y:����color:��ɫ  ���ڲ������� 
//--------------------------------------------------------------//
void GUI_DrawPoint(u16 X,u16 Y,u16 Color)
{
	 LCD_SetCursor(X,Y);//���ù��λ�� 
	 LCD_WDR(Color);  
}
//--------------------------------------------------------------//
//������:GUI_Horz     ����:��ˮƽ��
//������X,Y:�������Width:���� ���ڲ������� 
//--------------------------------------------------------------//
void GUI_Horz(u16 X ,u16 Y ,u16 Width)    //��ˮƽ��
{
	 LCD_SetWindows(X,Y,X+Width-1,Y);  //���û�ͼ����
	 while ( Width-- )
	    LCD_WDR(FColor);				   //���ǰ��ɫ
}
//--------------------------------------------------------------//
//������:GUI_Verz     ����:����ֱ��
//������X,Y:�������Height:�߶� ���ڲ������� 
//--------------------------------------------------------------//
void GUI_Vert(u16 X ,u16 Y ,u16 Height)   //����ֱ��
{
	 LCD_SetWindows(X,Y,X,Y+Height-1);   //���û�ͼ����
	 while ( Height-- )
	    LCD_WDR(FColor);				   //���ǰ��ɫ
}
//--------------------------------------------------------------//
//����:GUI_Line   ����:��ǰɫ��һ��ֱ��
//������x1,y1,x2,y2:����յ�����  ���ڲ������� 
//--------------------------------------------------------------//
void GUI_Line(u16 x1, u16 y1, u16 x2, u16 y2)
{
	 u16 t; 
	 int xerr=0,yerr=0,delta_x,delta_y,distance; 
	 int incx,incy,uRow,uCol; 

	 delta_x=x2-x1; //������������ 
	 delta_y=y2-y1; 
	 uRow=x1; 
	 uCol=y1; 
	 if (delta_x>0)
		  incx=1; //���õ������� 
	 else
		  if (delta_x==0)
			   incx=0;//��ֱ�� 
	   else {
         incx=-1;delta_x=-delta_x;} 
	 if (delta_y>0)
		  incy=1; 
	 else 
		  if (delta_y==0 )
				 incy=0;//ˮƽ�� 
	    else {
         incy=-1;delta_y=-delta_y;} 
	 if ( delta_x > delta_y ) 
		  distance=delta_x; //ѡȡ�������������� 
	 else
  	  distance=delta_y; 
	 for ( t=0 ; t<=distance+1 ; t++ ) {  
		  GUI_DrawPoint(uRow,uCol,FColor);//���� 
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
//������:GUI_Rectangle     ����:���ı���
//������X1,Y1,X2,Y3:�Ķ�������      ���ڲ������� 
//--------------------------------------------------------------//
void GUI_Rectangle(u16 X1, u16 Y1, u16 X2, u16 Y2)
{
	 GUI_Horz(X1,Y1,X2-X1+1);
	 GUI_Horz(X1,Y2,X2-X1+1);
	 GUI_Vert(X1,Y1,Y2-Y1+1);
	 GUI_Vert(X2,Y1,Y2-Y1+1);
} 
//--------------------------------------------------------------//
//����:GUI_FillRec   ����:���һ����������
//������SX,SY,EX,EY:�Ľ�����Color:��ɫ  ���ڲ������� 
//--------------------------------------------------------------//
void GUI_FillRec(u16 SX,u16 SY,u16 EX,u16 EY,u16 Color)
{  	
	 u16 i,j;			
	 u16 width =EX-SX+1; 		//�õ����Ŀ��
	 u16 height=EY-SY+1;		//�߶�
	 LCD_SetWindows(SX,SY,EX,EY);//������ʾ����
	 //LCD_SetWindows(SX,SY,EX-1,EY-1);//������ʾ���ڣ�����
	 for ( i=0 ; i<height ; i++ ) {
		  for ( j=0 ; j<width ; j++)
		     LCD_WDR(Color);}	//д������ 	 	
	 LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ���������Ϊȫ��
}
//--------------------------------------------------------------//
//*****�ڲ�����:�Գ��Ի�����********************* 
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
//����:GUI_Circle   ����:��Բ�ӳ���
//������xc,yc:Բ����r:�뾶Color:ɫ��Fill:���
//--------------------------------------------------------------//   
void GUI_Circle(u16 xc, u16 yc,u16 Color,u16 r, u8 fill)
{
	 int x = 0, y = r, yi, d;
	 d = 3 - 2 * r;
	 if (fill) 	{		// �����䣨��ʵ��Բ��
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
	 else {  // �������䣨������Բ��
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
//����:GUI_Bmp16   ����:ͼƬ����
//������X1,Y1:���X2,Y2:�յ�*P:ͼƬָ��
//--------------------------------------------------------------//
void GUI_Bmp16(u16 X1,u16 Y1,u16 X2,u16 Y2,const u8 *p) //��ʾͼƬ
{
   u16 i; 
	 u8 picH,picL; 
	 LCD_SetWindows(X1,Y1,X2,Y2);//��������
   for ( i=0 ; i<(X2-X1+1)*(Y2-Y1+1) ; i++ ){	
	 	  picL=*(p+i*2)  ;	//���ݵ�λ��ǰ
		  picH=*(p+i*2+1);				
		  LCD_WDR(picH<<8|picL);  						}	
	 LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ���ʾ����Ϊȫ��	
}
//15/----------------------------------------------------------LCD�ڰ�ͼ���----------------------------------------------------//
//˵����LCD���ڰ�ͼ���ӳ���
//��ڣ�X1,X2,Y1,Y2Ϊ��ʼ������ַ����Ҫ���ͼƬ�Ĵ�С��pΪָ�룬ָ����Ҫ������ݵ����飬INVERT=0Ϊ�׵׺��֣�1Ϊ�ڵװ���
//���ڣ���
//-----------------------------------------------------------------------------------------------------------------------------//
void     DisP_Photo  (unsigned int X1,unsigned int Y1,unsigned int X2,unsigned int Y2,unsigned char *P,unsigned char INVERT)
{   //Ҫ��X1<X2 , Y1 < Y2;
    unsigned char    j,k,LCDDATA;
    unsigned int   Capacity,i;
    
    LCD_SetWindows( X1, Y1, X2, Y2)	 ;
        
    Capacity = (X2 - X1 +1)*(Y2 - Y1 +1) ;
//	SET_FOCUS  (X1,Y1);   
    for (i=0 ; i<Capacity/8; i++)
    {
        LCDDATA = *P ;
        P++ ;
        k       = 0x80 ;	 //��ȡһ���ֽ�����,��8����
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
//21/--------------------------------LCD��ɫͼ���,����Ϊ��ʼλ�ü�ͼƬ��С----------------------------------------------------//
//˵����LCD����ɫͼ���ӳ���,����Ϊ��ʼλ�ü�ͼƬ��С
//��ڣ�X1,Y1Ϊ��ʼ��ַ����Ҫ���ͼƬ�����Ͻ���Ҫ��λ�ã�X_len,Y_lenΪͼƬ����pΪָ�룬ָ����Ҫ������ݵ�����
//���ڣ���
//-----------------------------------------------------------------------------------------------------------------------------//
void DisP_Photo16_pro(unsigned int X1,unsigned int Y1,unsigned int X_len,unsigned int Y_len,unsigned char *P)
{
	GUI_Bmp16(X1,Y1,X1+X_len-1,Y1+Y_len-1,P);
}
//20/----------------------------------------------------------LCD������ʾ����------------------------------------------------//
//˵������ʾ����
//��ڣ�x,yΪ��ʾ�ַ���λ�ã������ַ���ΪͼƬ����X1,Y1Ϊ���Ͻǵĵ㣩��PΪ����ʾ�ĺ����������������numΪ��Ҫ��ʾ�ĺ��ָ���
//���ڣ���
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
//����:GUI_ShowChar  ����:�ַ�����
//������X,Y:���  FC,BC:ǰ��ɫ�ͱ���ɫ
//	    ASCII:ASCII��,Size:��С16(0816)��12(0612)
//	    Mode:0�ǵ���1����
//--------------------------------------------------------------//
void GUI_ShowChar(u16 X,u16 Y,u16 FC, u16 BC, u8 ASCII,u8 Size,u8 Mode)
{  
   u8 temp;
   u8 pos,t;
	 u16 colortemp=FColor;      
		   
	 ASCII = ASCII -' '  ;//�õ�ƫ�ƺ��ֵ
	 LCD_SetWindows ( X , Y , X+Size/2-1,Y+Size-1);//���õ����ַ���ʾ����
	 if(!Mode) {//�ǵ��ӷ�ʽ		
		  for ( pos=0 ; pos<Size ; pos++ ) {
			   if ( Size == 12 )
					  temp=asc2_1206[ASCII][pos];//����1206����
			   else
 	          temp=asc2_1608[ASCII][pos];//����1608����
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
					  temp=asc2_1206[ASCII][pos];  //����1206����
			   else
					  temp=asc2_1608[ASCII][pos];	 //����1608����
			   for ( t=0 ; t<Size/2 ; t++ ){   
				    FColor=FC;              
		        if(temp&0x01)
							  GUI_DrawPoint( X+t , Y+pos ,FColor);//��һ����    
		        temp>>=1; }
		     }
	    }
	 FColor=colortemp;	
	 LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��    	   	 	  
}
//--------------------------------------------------------------//
//����:GUI_ShowStr  ����:��ʾ�ַ���(Ӣ��)
//������X,Y:��� ,Size:��С16(0816)��12(0612) 
//	    *P:�ַ���ָ��	    Mode:0�ǵ���1����
//--------------------------------------------------------------//	 
void GUI_ShowStr(u16 X,u16 Y,u8 Size,u8 *p,u8 Mode)
{         
   while((*p<='~')&&(*p>=' ')) {   //�ж��ǲ��ǷǷ��ַ�!      
		  if ( X > (lcddev.width-1)|| Y > (lcddev.height-1)) //�ж�λ��
		     return;     
      GUI_ShowChar(X,Y,FColor,BColor,*p,Size,Mode);//��ʾ�����ַ�
      X+=Size/2; //����X����λ��
      p++;}  
} 
//--------------------------------------------------------------//
//�ڲ�����:����
//--------------------------------------------------------------//
u32 mypow(u8 m,u8 n)
{
	 u32   result = 1;	 
	 while ( n-- )
		  result *= m ;    
	 return result;
}
//--------------------------------------------------------------//
//����:GUI_ShowNum  ����:��ʾ����
//������X,Y:��� ,Num:��ʾ������,Len:��ʾλ��
//     Size:��С16(0816)��12(0612) 
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
//��ʾ�ַ�
//--------------------------------------------------------------//
void GUI_Font16( u16 X,u16 Y,u16 FC, u16 BC, u8 *Str,u8 Mode)
{
   u8      i,j ;
	 u8      FontB ;
	 LCD_SetWindows( X,Y,X+15,Y+15 ) ; //(1)������ʾ����
	 for ( i = 0 ; i<32 ; i++ ) {      //(2)�ܹ�32�ֽ�
		  FontB = *Str ; Str ++ ;
		  for ( j = 0 ; j < 8 ; j++ ) {  //(3)8����
				 if  ( !Mode )  {     //�������䷽ʽ
            if ( FontB&0x80 ) { 
							 LCD_WDR (FC);  }
						else {
							 LCD_WDR (BC);}	
						}							 
				 else {               //�������䷽ʽ
            if	( FontB&0x80 ) {
							 GUI_DrawPoint(X+i%2*8+j,Y+i/2,FC);}
						}
				 FontB = FontB << 1 ;
				 }
			}				 
   }
//--------------------------------------------------------------//
//��ʾ�ַ�
//--------------------------------------------------------------//
void GUI_Font24( u16 X,u16 Y,u16 FC, u16 BC, u8 *Str,u8 Mode)
{
   u8      i,j ;
	 u8      FontB ;
	 LCD_SetWindows( X,Y,X+23,Y+23 ) ; //(1)������ʾ����
	 for ( i = 0 ; i<72 ; i++ ) {      //(2)�ܹ�72�ֽ�
		  FontB = *Str ; Str ++ ;
		  for ( j = 0 ; j < 8 ; j++ ) {  //(3)8����
				 if  ( !Mode )  {     //�������䷽ʽ
            if ( FontB&0x80 ) { 
							 LCD_WDR (FC);  }
						else {
							 LCD_WDR (BC);}	
						}							 
				 else {               //�������䷽ʽ
            if	( FontB&0x80 ) {
							 GUI_DrawPoint(X+i%3*8+j,Y+i/3,FC);}
						}
				 FontB = FontB << 1 ;
				 }
			}				 
   }




 





