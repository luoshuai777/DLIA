#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math_func.h>
#include <math.h>
#include <time.h>
#include <i2s.h>
#include <int.h>
#include <timer.h>
#include <utils.h>
#include <wm8978.h>
#include <pwm.h>

#define MSG_OK         0
#define MSG_KO         1
#define NUM_I2SIN      1					//输入通道数
#define NUM_CHANNEL    1					//输入通道声道数

#define iRX	       0					//I2S输入通道号
#define iTX	       0					//I2S输出通道号
#define PDMDS          128					//PDM降采样率
#define PV	       4					//PWM输出音量大小设置
	
#define I2S_DMA_FRMCNT 256					//一帧数据采样点数	
#define I2S_DMA_WNDCNT 40					//帧窗口(帧个数)
#define BSIZE          (I2S_DMA_FRMCNT*I2S_DMA_WNDCNT)	
#define VSIZE          (NUM_CHANNEL*BSIZE)
#define BUFSIZE        (NUM_I2SIN * VSIZE)			//存储语音数据buffer
	
#define iSelR	       0				 	//输入通道左声道
#define iBits	       16				 	//采样数据位数
#define iBytes	       ((iBits == 24) ? 4 : (iBits >> 3))	//采样数据字节数
#define tmpstride      (NUM_CHANNEL * iBytes)		 	//DMA步幅大小
#define RX_PDM				　　　　　       	//PDM格式麦克风输入
#define TX_PWM						 	//PWM格式播放输出

#ifdef  RX_PDM
#define RX_FORMAT I2S_PDM_RXEN				//PDM格式麦克风输入使能
#else
#define RX_FORMAT 0					//I2S格式麦克风输入使能
#endif

#ifdef  TX_PWM						//PWM格式输出使能
#define TX_FORMAT I2S_PWM_TXEN
#else
#define TX_FORMAT 0					//I2S格式输出使能
#endif

#define RXEN(i)    ((i == 0) ? I2S_RX0EN     : (i == 1) ? I2S_RX1EN     : (i == 2) ? I2S_RX2EN     : I2S_RXALLEN    )  //I2S输入通道使能
#define TXEN(i)    ((i == 0) ? I2S_TX0EN     : (i == 1) ? I2S_TX1EN     : (i == 2) ? I2S_TX2EN     : I2S_TXALLEN    )  //I2S输出通道使能
#define RXDMAEN(i) ((i == 0) ? I2S_RX0_DMAEN : (i == 1) ? I2S_RX1_DMAEN : (i == 2) ? I2S_RX2_DMAEN : I2S_RXALL_DMAEN)  //DMA接收使能
#define TXDMAEN(i) ((i == 0) ? I2S_TX0_DMAEN : (i == 1) ? I2S_TX1_DMAEN : (i == 2) ? I2S_TX2_DMAEN : I2S_TXALL_DMAEN)  //DMA发送使能


#define CONFIG24_mono_R (I2S_CFG_TX_BIT24 | I2S_CFG_RX_BIT24 | I2S_CFG_TX_MONO | I2S_CFG_RX_MONO | I2S_CFG_TX_SELR | I2S_CFG_RX_SELR)  //24iBits,单通道,右声道数据采样
#define CONFIG24_mono_L (I2S_CFG_TX_BIT24 | I2S_CFG_RX_BIT24 | I2S_CFG_TX_MONO | I2S_CFG_RX_MONO | I2S_CFG_TX_SELL | I2S_CFG_RX_SELL)  //24iBits,单通道,左声道数据采样
#define CONFIG24_dual   (I2S_CFG_TX_BIT24 | I2S_CFG_RX_BIT24 | I2S_CFG_TX_DUAL | I2S_CFG_RX_DUAL | I2S_CFG_TX_SELL | I2S_CFG_RX_SELL)  //24iBits，双声道数据采样

#define CONFIG16_mono_R (I2S_CFG_TX_BIT16 | I2S_CFG_RX_BIT16 | I2S_CFG_TX_MONO | I2S_CFG_RX_MONO | I2S_CFG_TX_SELR | I2S_CFG_RX_SELR)
#define CONFIG16_mono_L (I2S_CFG_TX_BIT16 | I2S_CFG_RX_BIT16 | I2S_CFG_TX_MONO | I2S_CFG_RX_MONO | I2S_CFG_TX_SELL | I2S_CFG_RX_SELL)
#define CONFIG16_dual   (I2S_CFG_TX_BIT16 | I2S_CFG_RX_BIT16 | I2S_CFG_TX_DUAL | I2S_CFG_RX_DUAL | I2S_CFG_TX_SELL | I2S_CFG_RX_SELL)

#define CONFIG8_mono_R  (I2S_CFG_TX_BIT8 | I2S_CFG_RX_BIT8 | I2S_CFG_TX_MONO | I2S_CFG_RX_MONO | I2S_CFG_TX_SELR | I2S_CFG_RX_SELR)
#define CONFIG8_mono_L  (I2S_CFG_TX_BIT8 | I2S_CFG_RX_BIT8 | I2S_CFG_TX_MONO | I2S_CFG_RX_MONO | I2S_CFG_TX_SELL | I2S_CFG_RX_SELL)
#define CONFIG8_dual    (I2S_CFG_TX_BIT8 | I2S_CFG_RX_BIT8 | I2S_CFG_TX_DUAL | I2S_CFG_RX_DUAL | I2S_CFG_TX_SELL | I2S_CFG_RX_SELL)

#define CONFIG(NUM_CHANNEL, iBits, iSelR) ( \
(NUM_CHANNEL == 1 && iBits ==  8 &&  iSelR) ? CONFIG8_mono_R  : \
(NUM_CHANNEL == 1 && iBits ==  8 && ~iSelR) ? CONFIG8_mono_L  : \
(NUM_CHANNEL == 1 && iBits == 16 &&  iSelR) ? CONFIG16_mono_R : \
(NUM_CHANNEL == 1 && iBits == 16 && ~iSelR) ? CONFIG16_mono_L : \
(NUM_CHANNEL == 1 && iBits == 24 &&  iSelR) ? CONFIG24_mono_R : \
(NUM_CHANNEL == 1 && iBits == 24 && ~iSelR) ? CONFIG24_mono_L : \
(NUM_CHANNEL == 2 && iBits ==  8 ) ? CONFIG8_dual  :           \
(NUM_CHANNEL == 2 && iBits == 16 ) ? CONFIG16_dual : CONFIG24_dual)


#ifdef __riscv__

#define REST_TIMER() reset_timer();
#define SET_START_TIME() start_time = get_time();
#define SET_END_TIME() end_time = get_time(), user_time = (end_time - start_time);
#define SHOW_DIFF_TIME(a) dprintf("function(%10s): use time = %8f s\n", a, user_time);
#define SHOW_FRAME_TIME dprintf("              mfcc time / frame: %f ms\n", user_time * 1000.0f/(float)all_frame_num );

#else

#define REST_TIMER() 
#define SET_START_TIME() gettimeofday(&start_time,NULL);
#define SET_END_TIME() gettimeofday(&end_time,NULL), user_time = ((end_time.tv_sec - start_time.tv_sec)*1000000.0f + (end_time.tv_usec - start_time.tv_usec));
#define SHOW_DIFF_TIME(a) dprintf("function(%10s): use time = %f us\n", a,  user_time);
#define SHOW_FRAME_TIME dprintf("              mfcc time / frame: %f us\n", user_time/(float)all_frame_num );

#endif

#endif  // _DEFINE_H_
