# C51-MCU-Liquid-level-keil

C51单片机湿度测量串口课设项目(Keil)

## 文件目录

- C51-MCU-Liquid-level-keil.c	项目源文件
- C51-MCU-Liquid-level-keil.pdsprj	项目仿真文件

## 软件版本

- Keil uVision5
- Proteus 8 Professional(8.15)

## 展示

![image](https://github.com/CH4019/C51-MCU-TrafficLight-Keil/blob/main/img/C51-MCU-Liquid-level-keil.png)

## 介绍

    传感器测量范围为20-250kpa，测量规则为：给定测量自带初始压力，并在开机后第一时间获取自带压力，用来校准（调零），在这个压力基础上再来测量液位高度，这里默认液体为水。我这里假定测量范围为0-200mm，当测量结果不再这个范围时在lcd上显示具体错误。
    ////注意////
    传感器有自带误差。

## License 
  
 [The MIT License (MIT)](https://github.com/CH4019/C51-MCU-Keil-Curriculum-design/blob/main/LICENSE)

## 最后请给个小⭐⭐
