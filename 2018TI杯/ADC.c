//############################################################################################################################//
//----------------------------------------ADC驱动文件-------------------------------------------------------------------------//
//作者:JERRY------------------------------------------------------------------------------------------------------------------//
//时间:2014年7月31日----------------------------------------------------------------------------------------------------------//
//说明:使用时请根据实际需要更改“控制命令”内的内容-----------------------------------------------------------------------------//
//版本修改情况：--------------------------------------------------------------------------------------------------------------//
//-------2014.7.31------------写基础驱动函数----------------------------------------------------------------------------------//
//-------2015.8.05------------初始化函数将端口配置为高阻模式------------------------------------------------------------------//
//############################################################################################################################//
#include"ADC.h"
//=====================================================================================start====================================================================================================//
//1/-------------ADC上电延时函数---------------------------//
//入口：unsigned char n，n为需要延时的时间
//出口：无
//---------------------------------------------------------//
void ADC_Delay(unsigned char n)
{
    unsigned char x;

    while (n--)
    {
        x = 5000;
        while (x--);
    }
}
//3/-------------ADC启动函数-------------------------------//
//入口：无
//出口：无
//---------------------------------------------------------//
void ADC_start()
{
	ADC_CONTR |=ADC_START;	//启动ADC
}
//4/-------------ADC转换完成标志位清除函数-----------------//
//入口：无
//出口：无
//---------------------------------------------------------//
void ADC_flagclean()
{
	ADC_CONTR&=0Xef;	//清除标志位
}
//5/-------------ADC转换完成结果清除函数-------------------//
//入口：无
//出口：无
//---------------------------------------------------------//
void ADC_valueclean()
{
	ADC_RES = 0; 
	ADC_RESL = 0;
}
//6/----------------读取ADC转换结果函数--------------------//
//入口：无
//出口：unsigned long int类型，为ADC转换结果
//---------------------------------------------------------//
unsigned long int ADC_value()
{
	unsigned long int result=0;
	if(!VAGUE && ADRJ)				//0 1
		result=ADC_RES*256+ADC_RESL;		//高2位低8位
	else if(!VAGUE && !ADRJ)		//0 0
		result=ADC_RES*4+ADC_RESL;			//高8位低2位
	else if(VAGUE && !ADRJ)			//1 0
		result=ADC_RES;
	return result;
}
//7/----------------更改通道口-----------------------------//
//入口：unsigned char ch，ch为需要使用的通道口
//出口：无
//---------------------------------------------------------//
void change_ch(unsigned char ch)
{
	ADC_CONTR =	 ADC_CONTR & 0xf8;
	ADC_CONTR |= ch;
}
//8/----------------根据AD值转化为电压---------------------//
//入口：unsigned long int x，x为AD值
//出口：无
//---------------------------------------------------------//
unsigned long int trans_ad(unsigned long int x)
{
	unsigned long int result=0;
	if(x>=1)
		result=0.0000008*x*x*x*x-0.0003*x*x*x+0.0328*x*x+16.407*x+14.402;
	return result;	
}
//=====================================================================================end====================================================================================================//
