#include <reg51.h>
#include "stdio.h"
#define uchar unsigned char
#define uint unsigned int // 宏定义

sbit DHT22_PIN = P1 ^ 0;
uchar value[5];
uint temp, hum;

void delay20us(void);
void delay1ms(void);
void read_value(void);


void delay20us()
{
    uchar a, b;
    for (b = 3; b > 0; b--)
    {
        for (a = 1; a > 0; a--)
        {
            /* code */
        }
    }
}
void delay1ms()
{
    uchar a, b, c;
    for (c = 1; c > 0; c--)
    {
        for (b = 142; b > 0; b--)
        {
            for (a = 2; a > 0; a--)
            {
                /* code */
            }
        }
    }
}

void read_value()
{
    uchar i, j = 0, mask;
    DHT22_PIN = 0;
    for (i = 0; i < 20; i++)
    {
        delay1ms();
    }
    DHT22_PIN = 1;
    while (DHT22_PIN)
        ;
    while (!DHT22_PIN)
        ;
    while (DHT22_PIN)
        ;
    while (j < 5)
    {
        mask = 0x80;
        for (i = 0; i < 8; i++)
        {
            while (!DHT22_PIN)
                ;
            delay20us();
            delay20us();
            if (DHT22_PIN == 0)
            {
                value[j] &= (~mask);
            }
            else
            {
                value[j] |= mask;
            }
            mask >>= 1;
            while (DHT22_PIN)
                ;
        }
        j++;
    }
}
// 串口初始化
void uart_init()
{
    TMOD |= 0x20; // 设置为工作方式1，8位数据，可变波特率
    SCON = 0x40;  // TH1 = 256 - 11.0592 * 1000 * 1000 / 12 / 32 / 9600;
    TCON |= 0x40; // tell putchar() the serial is ready to send
    SCON |= 0x02;
    TH1 = 0xFD; // 波特率9600
    TL1 = 0xFD;
    PCON &= 0x7F; // 波特率不加倍
    TR1 = 1;      // 启动定时器1
    SM0 = 0;      // 设置为8位数据模式
    SM1 = 1;
    EA = 1; // 允许总中断
    ES = 1; // 允许串口中断
}

void SendByte(unsigned char dat)
{
    SBUF = dat;
    while (!TI)
        ;   // 等待发送完成
    TI = 0; // 清除发送中断标志
}

void SendString(const char *str)
{
    while (*str)
    {
        SendByte(*str);
        str++;
    }
}


void main()
{
    uint i;
    char str[7]; // 用于存储转换后的 hum
    uart_init();
    while (1)
    {
        for (i = 0; i < 1000; i++)
        {
            delay1ms();
        }
        read_value();
        if (value[0] + value[1] + value[2] + value[3] == value[4])
        {
            hum = value[0] * 256 + value[1];
            sprintf(str, "%.1f", hum/10.0); // 将 hum 转换为字符串
            delay1ms();
            SendString(str); // 输出 hum 结果到串口
            delay1ms();
            SendString("\n");
        }
    }
}