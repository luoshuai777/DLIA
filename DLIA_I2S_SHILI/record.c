
#include <string_lib.h>
#include "define.h"
#include "record.h" 

//Record Param
short __attribute__((aligned(2))) g_nSrcRecData[BUFSIZE];	//存放接收的数据
short __attribute__((aligned(2))) g_nSrcOutData[BUFSIZE];	//存放发送的数据
volatile int g_iCurrRecFrm = 0;					//当前接收帧的个数
volatile int g_TxDMASent   = 1;					//DMA数据发送完成的标志位

#define RECODER_SECOND (4)
#define RECODER_LEN    (16000*RECODER_SECOND)

#define RECORDBUFSAMPLENUMMAX 64000
short recordBuf[RECORDBUFSAMPLENUMMAX] = { 0 };
int recordBufSampleNum = 0;

//DMA配置
struct i2s_param_dma dma=
{
	I2S_DMA_FRMCNT,						//DMA接收缓冲区帧长(一帧的采样点个数) 
	I2S_DMA_WNDCNT, 					//DMA接收缓冲区帧窗口设置(帧的个数)
	tmpstride,	      				      	//DMA接收缓冲区步幅大小
	I2S_DMA_FRMCNT, 					//DMA发送缓冲区帧长
	I2S_DMA_WNDCNT, 					//DMA发送冲区帧窗口设置
	tmpstride,  					      	//DMA发送缓冲区步幅大小
	{(unsigned int )&g_nSrcRecData[0], 0,0},		//DMA接收缓冲区起始地址
	{(unsigned int )&g_nSrcOutData[0], 0,0}		        //DMA发送缓冲区起始地址
};

//VAD用于语音的唤醒，根据I2S输入输出的通道、采样率设置VAD
struct i2s_param_vad vad_8khz={/*{{{*/
	160,							      //vad帧长
	7,							      //vad长块的帧数
	4,							      //vad短块的帧数
	8000,							      //语音能量绝对值门限
	8,							      //语音帧数门限(静音到语音段)
	1<<5,							      //噪声门限系数
	(1<<4)+(1<<1),					      	      //噪声更新系数
	5,							      //噪声帧数门限(语音到静音段)
	2,							      //唤醒时间(时钟和电压稳定恢复时间)
      	iRX,							      //麦克风唤醒通道
};/*}}}*/

//16kHZ采样率
struct i2s_param_vad vad_16khz={/*{{{*/
	320,		
	7,			
	4,				
	8000,		
	8,		
	1<<5,		
	(1<<4)+(1<<1),			
	5,		
	2,				
	iRX,				
};/*{{{*/

#define ENABLE(rxen, txen, rxdmaen, txdmaen, pdm_rxen, pwm_txen) (rxen | txen | rxdmaen | txdmaen | pdm_rxen | pwm_txen)
	
struct i2s_param_global global = 
{	
	ENABLE(RXEN(iRX),TXEN(iTX),RXDMAEN(iRX),TXDMAEN(iTX),RX_FORMAT,TX_FORMAT),//I2S通道的输入输出使能，DMA的输入输出使能，硅麦输入和播放格式使能
	PDMDS, 								      	  //PDM降采样率
	PV,			      						  //音量
        CONFIG(NUM_CHANNEL,iBits,iSelR),		                          //I2S的数据位宽、单声道、左声道的设置	
	CLK_4096KHZ, 			   				          //I2S输出主时钟
	CLK_2048KHZ, 							          //音频输入解码时钟(输入采样时钟)
	CLK_2048KHZ			  			                  //音频输出编码时钟(输出采样时钟)
};

struct i2s_param param = {&global, &dma, &vad_16khz};		//dma、vad、global结构体
struct i2s_param *p = &param;

//I2S中断服务程序
void i2s_intr_recorder(void)
{
	int i;
	int status;
	status=i2s_get_dma_status();				//获取DMA状态寄存器
	i2s_dma_clearall_done(status);				//清除输入输出通道的dma所有发送和完成的标志位
	if(status & I2S_DMA_RXDONE) 				
	{
		g_iCurrRecFrm += 1;				//DMA数据接收完成、g_iCurrRecFrm加1
	}
	else 
		g_TxDMASent = 1;				//DMA数据发送完成，g_TxDMAsent标志位置1
	clear_eintr(I2S_INTR);					//清除I2S中断
}

int InitAudio()
{
	int iRetCode = MSG_OK;
	bind_intr(I2S_INTR_NO, &i2s_intr_recorder);  	        //绑定I2S中断
	return iRetCode;
}

int DealAudio()
{
	int iRetCode = MSG_OK;
	unsigned int currecdata, curoutdata;					
	int recfrm_sizeofbytes, outfrm_sizeofbytes;
	int g_iCurrOutFrm, iCurrWndIdx;

	i2s_open_mclk();
      	set_gpio_pin_value(2,1,1);			//设置I2S Speaker输出时钟									

      	g_iCurrRecFrm = 0;				//当前接收帧数
      	g_iCurrOutFrm = 0;				//当前发送帧数
      	iCurrWndIdx   = 0;				//当前帧窗口计数
	outfrm_sizeofbytes = (recfrm_sizeofbytes = I2S_DMA_FRMCNT*tmpstride);		//输出数据大小

	int_enable(); 					//开启全局中断
	inc_eintr_en(I2S_INTR); 			//使能I2S中断
	i2s_setup(p);					//设置global、dma、vad三个结构体，检查参数是否合法
      	i2s_dma_rxstart();				//开启dma输入

      	g_TxDMASent = 1;				
      	printf("\n---deep sleep now---\n");
      	deep_sleep();					//开启深度休眠
      	printf("\n---woke up  now---\n");
	while(g_iCurrRecFrm < RECODER_LEN/I2S_DMA_FRMCNT) 
//	while(1) 
	{
		if(g_iCurrOutFrm < g_iCurrRecFrm)	//发送的帧数小于接收的帧数
		{  
			currecdata = p->dma->rxdma_sa[iRX] + iCurrWndIdx*recfrm_sizeofbytes;		//音频输入数据  
		   	curoutdata = p->dma->txdma_sa[iTX] + iCurrWndIdx*outfrm_sizeofbytes;		//音频输出数据

		   	nmemcpy((void *)curoutdata, (void *)currecdata, recfrm_sizeofbytes);		//把接收到的音频数据拷贝给DMA发送缓冲区	 
			nmemcpy((void *)(recordBuf + recordBufSampleNum), (void*)currecdata, recfrm_sizeofbytes);
       		        recordBufSampleNum += recfrm_sizeofbytes/2;
			iCurrWndIdx=(iCurrWndIdx+1) % (I2S_DMA_WNDCNT-1);				//帧窗口计数

			while (g_TxDMASent == 0) ;		//等待DMA发送数据
			i2s_dma_txfire();			//开启DMA输出
			g_TxDMASent = 0;			//DMA发送完数据标志位置0
			g_iCurrOutFrm += 1;			//发送帧加1
		}

		else 
		{
		 sleep();					//浅度休眠
		}
	}

	i2s_close();						//关闭I2S编码和解码时钟
	clear_eintr_en(I2S_INTR);				//清除I2S中断使能
	int_disable();						//关闭全局中断
	return iRetCode;
}

int FreeAudio()
{
	int iRetCode = MSG_OK;
	return iRetCode;
}
