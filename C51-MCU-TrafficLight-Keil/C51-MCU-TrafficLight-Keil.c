//头文件
#include<reg51.h>
#define uchar unsigned char
#define uint unsigned int

//共阴极字型码
uchar code table[11]={
	0x3f,  //--0
	0x06,  //--1
	0x5b,  //--2
	0x4f,  //--3
	0x66,  //--4
	0x6d,  //--5
	0x7d,  //--6
	0x07,  //--7
	0x7f,  //--8
	0x6f,  //--9
	0x00   //--NULL
};

uchar data buf[4];
// 按钮控制区域
sbit KEY_1 = P1^0;
sbit KEY_2 = P1^1;
sbit KEY_3 = P1^2;
sbit KEY_4 = P1^3;
sbit KEY_5 = P1^4;

// 定义变量
unsigned char i=0;
unsigned char j=0;
unsigned int sec;
unsigned int sec_g = 20;
unsigned int sec_ = 0;
unsigned int sec_y = 5;
unsigned int sec_r;
unsigned int sec_ns;

// 颜色指示灯
sbit we_g = P3^0;   //东西绿灯
sbit we_y = P3^1;
sbit we_r = P3^2;
sbit NS_g = P3^3;   //南奔绿灯
sbit NS_y = P3^4;
sbit NS_r = P3^5;

//数码管显示控制
sbit led_1=P2^0;
sbit led_2=P2^1;
sbit led_3=P2^2;
sbit led_4=P2^3;

//函数的声明部分
void delay(int t);
void Pause();
void display();	
void Sec_ADD();
void Sec_SUB();
void jixu();

//数码管显示函数
void display(void){
    buf[1] = sec / 10;  //第1位 东西秒十位
	buf[2] = sec % 10;  //第2位 东西秒个位
	buf[3] = sec_ns / 10;   //第3位 南北秒十位
	buf[0] = sec_ns % 10;   //第4位 南北秒个位

    P0=0x00;
    led_1=0;
    led_2=1;
    led_3=1;
    led_4=1;
    P0=table[buf[1]];
    delay(1);
    led_1=1;
    P0=0x00;
    led_1=1;
    led_2=0;
    led_3=1;
    led_4=1;
    P0=table[buf[2]];
    delay(1);
    led_2=1;
    P0=0x00;
    led_1=1;
    led_2=1;
    led_3=0;
    led_4=1;
    P0=table[buf[3]];
    delay(1);
    led_3=1;
    P0=0x00;
    led_1=1;
    led_2=1;
    led_3=1;
    led_4=0;
    P0=table[buf[0]];
    delay(1);
    led_4=1;
}

// 长按暂停函数
void Pause()
{
	if(KEY_5==0)
	{
        display();  //调用显示，用于延时消抖
		if(KEY_5==0)
		{
			TR0=0;
            we_g=0;we_r=1;we_y=0;
            NS_g=0;NS_r=1;NS_y=0;
            do
            {
				display();  //调用显示，用于延时
			}
			while(!KEY_5);  //等待按键释放
        }
	}
}

// 继续按钮函数
void jixu(){
    if(KEY_4==0)
	{
        display();  //调用显示，用于延时消抖
		if(KEY_4==0)
		{
            sec=0;
            j=0;
			TR0=1;
            do
            {
				display();  //调用显示，用于延时
			}
			while(!KEY_4);  //等待按键释放
        }
	}
}

// 时间加函数
void Sec_ADD()
{
	if(KEY_2==0)
	{
        display();       				//调用显示，用于延时消抖
        if(KEY_2==0)
        {
            TR0=0;
            sec_++;
            sec=sec_g+sec_;
            sec_ns=00;
            do
            {
				display();				 //调用显示，用于延时
			}
			while(!KEY_2);			 //等待按键释放
	}
    }
}

// 时间减函数
void Sec_SUB()
{
	if(KEY_3==0)
	{
        display();       				//调用显示，用于延时消抖
        if (KEY_3==0)
        {
            TR0=0;
            sec_--;
            sec=sec_g+sec_;
            sec_ns=00;
            do
            {
				display();				 //调用显示，用于延时
			}
			while(!KEY_3);			 //等待按键释放
        }
	}
}

//主程序
main(){
    TMOD=0x01;  //定时器方式为方式一
	EA=1;   //开总中断
	ET0=1;  //开T0中断
    TH0=0xee;
	TL0=0x00;
    i=0;
    while (1)
    {
        if (KEY_1==0)
        {
            TR0=1;
            while (KEY_1==0){
                Pause();
				Sec_ADD();
				Sec_SUB();
                jixu();
            }
        }
        display();
        Pause();
        Sec_ADD();
        Sec_SUB();
        jixu();
    }

}

// 定时中断函数
void time00(void) interrupt 1 using 0{
    TH0=0xee;
	TL0=0x00;
    sec_r=sec_g+sec_y+sec_;
	i++;
	if(i==200)
	{
        i=0;
        if (sec==0)
        {
            j++;
            if (j==1){
                sec=sec_g+sec_;
                sec_ns=sec_r;
                we_g=1;we_r=0;we_y=0;
                NS_g=0;NS_r=1;NS_y=0;
            }else if (j==2){
                sec=sec_y-1;
                sec_ns--;
                we_g=0;we_r=0;we_y=1;
                NS_g=0;NS_r=1;NS_y=0;
            }else if (j==3){
                sec=sec_r;
                sec_ns=sec_g+sec_;
                we_g=0;we_r=1;we_y=0;
                NS_g=1;NS_r=0;NS_y=0;
            }else if (j==4)
            {
                j=0;
            }
        }else if (sec>0)
        {
            if (j==1){
                sec--;
                sec_ns--;
                we_g=1;we_r=0;we_y=0;
                NS_g=0;NS_r=1;NS_y=0;
            }
            if (j==2){
                sec--;
                sec_ns--;
                we_g=0;we_r=0;we_y=1;
                NS_g=0;NS_r=1;NS_y=0;
            }
            if (j==3){
                if (sec_ns==0)
                {
                    sec_ns=sec_y-1;
                    sec--;
                    we_g=0;we_r=1;we_y=0;
                    NS_g=0;NS_r=0;NS_y=1;
                }else if (sec_ns>0)
                {
                    sec--;
                    sec_ns--;
                }
            }
        }
    }
}

//延时子程序
void delay(int t)
{
	uint x,k;
	for(x=0;x<t;x++){
        for(k=0;k<23;k++);
    }
}