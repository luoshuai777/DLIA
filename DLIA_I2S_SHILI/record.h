#ifndef RECODER_AUDIO_RISCV_H
#define RECODER_AUDIO_RISCV_H

//设备初始化
int InitAudio();
//获取音频数据并处理
//int DealAudio(short* buf, int bufSampleNum);
int DealAudio();
//释放设备资源
int FreeAudio();

#endif
