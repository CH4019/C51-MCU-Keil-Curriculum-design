// 头文件
#include <reg51.h>
#include "stdio.h"
#define uchar unsigned char
#define uint unsigned int

// lcd1602部分

// 屏幕相关引脚接口定义
#define D P0
sbit E = P2 ^ 2;
sbit RW = P2 ^ 1;
sbit RS = P2 ^ 0;

// 函数声明
void LcdWriteCom(uchar com);  /* LCD1602写入8位命令子函数 */
void LcdWriteData(uchar dat); /* LCD1602写入8位数据子函数 */
void LcdInit();               /* LCD1602初始化子程序 */
void Show_string1(uchar a[]);           /* 第一行从头开始显示 字符   "" 双信号内直接加显示内容即可 */
void Show_string2(uchar a[]);           /* 第二行     "" 双信号内直接加显示内容即可 */
void SendString(const char *str);
void LCD1602_Clear_1LINE();
void LCD1602_Clear_2LINE();

void delay_us(int ms) // 延时子程序
{
    uint j, k;
    for (j = 0; j < ms; j++) // 延时ms
        for (k = 0; k < 124; k++)
            ; // 大约1毫秒的延时
}

// 函数名：ADC0832_ReadAD
// 形参变量：无
// 返回值return：AD_value：转化之后的数字量

sbit ADC0832_CS = P2 ^ 3;   // 片选信号段
sbit ADC0832_CLK = P2 ^ 4;  // 时钟信号端
sbit ADC0832_DIN = P2 ^ 5;  // 串行数据输入端
sbit ADC0832_DOUT = P2 ^ 6; // 串行数据输出端

void ADC0832_init(void)
{
    ADC0832_CS = 1; // 芯片不工作
    ADC0832_CLK = 0;
    ADC0832_DIN = 0;
}

unsigned char ADC0832_ReadAD(void)
{
    unsigned char i, AD_value1 = 0x00, AD_value2 = 0x00;
    ADC0832_CS = 0; // 芯片工作(初始cs信号为高电平)

    ADC0832_DIN = 1; // 起始信号S
    ADC0832_CLK = 1; // 上升沿将起始信号Start写入
    ADC0832_CLK = 0;

    //----------------通道地址设置----0通道（单端模式）---------------//

    ADC0832_DIN = 1; // mode状态位置1
    ADC0832_CLK = 1; // 将mode状态位写入
    ADC0832_CLK = 0;

    ADC0832_DIN = 0; // 通道选择0通道
    ADC0832_CLK = 1; // 将通道选择位写入
    ADC0832_CLK = 0; // 第4个脉冲下降沿后，ADC0832_DIN端口失效,无作用

    ADC0832_DIN = 1; // ADC0832_DIN转为改高阻状态（数据线拉高，等待下次接收数据）

    //---------------------读取相对应的通道0的AD数据--------------------------//
    ADC0832_DOUT = 1; // 数据线拉高，等待接收数据

    for (i = 0; i < 8; i++) // MSB
    {
        ADC0832_CLK = 1;
        ADC0832_CLK = 0;
        if (ADC0832_DOUT)
        {
            AD_value1 = AD_value1 | (0x80 >> i);
        } // 优先读取最高位        MSB
    }

    for (i = 0; i < 8; i++) // LSB
    {
        if (ADC0832_DOUT)
        {
            AD_value2 = AD_value2 | (0x01 << i);
        } // 优先读取最低位        LSB
        ADC0832_CLK = 1;
        ADC0832_CLK = 0;
    }

    ADC0832_DOUT = 1;                                // 数据线拉高，等待接收数据
    ADC0832_CS = 1;                                  // 芯片停止工作
    return (AD_value1 == AD_value2) ? AD_value1 : 0; // 判断前后两次读取数据是否相同(前后两个字节数据校验)
}

void main()
{
    char string[17] = {0};
    char string2[17] = {0};
    float temp_init,temp_init1,press_init;
    ADC0832_init();
    /* LCD1602初始化函数 */
    LcdInit();
    temp_init = ADC0832_ReadAD();
    temp_init1 = temp_init * 5.0 / 255;
    press_init = ((temp_init1 / 5.1) / 0.004 + 10 - 3.45);
    while (1) // 程序主逻辑
    {
        float temp = ADC0832_ReadAD();
        // 将字符串格式化，并存储在数组中，用于显示摄氏度
        float temp1 = temp * 5.0 / 255;
        float press = ((temp1 / 5.1) / 0.004 + 10 - 3.45);
        float height = (press-press_init) / 9.8 ;
        float height_mm = height*100.0;
        sprintf(string, "h:%.2f mm", height_mm);
        sprintf(string2, "H:%.2f M", height);
        if (((float)0.0 <= height_mm) && (height_mm <= (float)200.0))
        {
            Show_string1(string); // 显示在LCD第一行
            Show_string2(string2);
        }
        else
        {
            if (height_mm < ((float)0.0))
            {
                SendString("low of Range");
                delay_us(300);
                LCD1602_Clear_1LINE();
                LCD1602_Clear_2LINE();
            }
            else if (height_mm > ((float)200.0))
            {
                SendString("out of Range");
                delay_us(300);
                LCD1602_Clear_1LINE();
                LCD1602_Clear_2LINE();
            }
        }
    }
}

/* 向1602写命令函数
 * 参数:com 命令
 * 返回值 : 无
 */
void LcdWriteCom(uchar com)
{
    E = 0;
    RW = 0;
    RS = 0;
    D = com;
    delay_us(5);
    E = 1;
    delay_us(5);
    E = 0;
}
/* 向1602写数据命令
 * 参数: dat 数据
 * 返回值: 无
 */
void LcdWriteData(uchar dat)
{
    E = 0;
    RW = 0;
    RS = 1;
    D = dat;
    delay_us(5);
    E = 1;
    delay_us(5);
    E = 0;
}

/* LCD1602初始化函数
 */
void LcdInit()
{
    LcdWriteCom(0x38);
    LcdWriteCom(0x0c); // 0x0C光标不闪
    //	LcdWriteCom(0x0d);//0x0D光标闪烁
    LcdWriteCom(0x06);
    LcdWriteCom(0x01);
    LcdWriteCom(0x80);
}

/* 第一行显示字符串
 */
void Show_string1(uchar a[])
{
    uchar i;
    LcdWriteCom(0x80);
    for (i = 0; a[i] != '\0'; i++)
    {
        LcdWriteData(a[i]);
    }
    LcdWriteData(0x20);
    LcdWriteData(0x20);
    LcdWriteData(0x20);
}
/* 第二行显示字符串
 */
void Show_string2(uchar a[])
{
    uchar i;
    LcdWriteCom(0x80 + 0X40);
    for (i = 0; a[i] != '\0'; i++)
    {
        LcdWriteData(a[i]);
    }
}

void SendString(const char *str)
{
    LcdWriteCom(0x80);
    while (*str)
    {
        LcdWriteData(*str);
        str++;
    }
}

void LCD1602_Clear_1LINE()
{
    int i = 0;
    LcdWriteCom(0x80);
    for (i = 0; i < 16; i++)
    {
        LcdWriteData(0x20); // 无显示
    }
}
void LCD1602_Clear_2LINE()
{
    int i = 0;
    LcdWriteCom(0x80 + 0x40);
    for (i = 0; i < 16; i++)
    {
        LcdWriteData(0x20); // 无显示
    }
}