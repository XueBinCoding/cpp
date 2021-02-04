# PCM数据格式

## 音频参数

> 44100HZ 16bit stereo:
>  每秒钟有 44100 次采样, 采样数据用 16 位(2字节)记录, 双声道(立体声);
>  \- 22050HZ 8bit  mono:
>  每秒钟有 22050 次采样, 采样数据用 8 位(1字节)记录, 单声道;

## PCM是什么以及音频数据的一些基本概念

- PCM:(Pulse-code-modulation):脉冲编码调制，是模拟信号以固定的采样频率转化为数字信号后的表现形式。
- Sign:音频数据是否是有符号的。
- Sample Size:采样数据的大小
- Byte Ordering：字节序，即little-endian 还是big-endia
- Number of channels:标识声音是单通道（mono）的还是双通道的（strreo）

## PCM读取文件的例子

```C++ 
严重性	代码	说明	项目	文件	行	禁止显示状态
错误	C4996	'fopen': This function or variable may be unsafe. Consider using fopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.	SpeechRecord	d:\iflytek\sdkdemotest\audiorecord\speechrecord\src\main.cpp	14	

```

