# 整理需要的外设以及相关的管脚信息
   ## SPI 管脚模拟
        MOSI----------P40
        MISO----------P41
        SCLK----------P43
        AD_CS---------P51
   ## 串口 
        RXD-----------P30
        TXD-----------P31
   ## 7279
        7279CS--------P42
        7279CLK-------P73
        DAT-----------P72
        INT3----------P37

        P40 OUTPUT
        P41 INPUT
        P42 OUTPUT
        P43 OUTPUT
        
        P30    INPUT
        P31    OUTPUT
        P37    INPUT

        P51    OUTPUT

        P72    INT/OUT
        P73    OUTPUT



     P40  P41  P42  P43
M1   0    1    0    0         |=0x02   P4M1=0x02
M0   1    0    1    1         |=0x0D   P4M0=0x0D

     P37  P36 P35 P34 P33 P32 P31 P30
M1   1                        0    1    P3M1=0X81
M0   0                        1    0    P3M0=0X02

     P57 P56 P55 P54 P53 P52 P51 P50
M1                            0         P5M1=0x00;
M0                            1         P5M0=0x02;

     P77 P76 P75 P74 P73 P72 P71 P70
M1                   0    0             P7M1=0x00;
M0                   1    0             P7m0=0x08;