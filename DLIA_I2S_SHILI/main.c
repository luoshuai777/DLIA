#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dw_i2c.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "record.h"

extern int recordBufSampleNum;
extern short recordBuf[];

int TestThread()
{
        int i = 0,j = 0;
//      while(1)
        {
                ///////////////recording audio//////////////////dd
                if(DealAudio() != 0)
                {
                        printf("=============DealAudio Error=================\n");
                        return -1;
                }
                printf("\n=======================start audio=================\n");
                for(i = 0; i < recordBufSampleNum; i++)
                {
                        printf("%d ", recordBuf[i]);
                }
                printf("\n===================end audio==============\nrecordBufSampleNum = %d\n", recordBufSampleNum);
                printf("-----------------done record audio, start send audio to ASR-------------------------\n");
        }

        return 0;
}


int main(int argc, char *argv[])
{
	int iRetCode = 0;
	if(InitAudio() != 0)
	{
		printf("==========InitAudio Error==================\n");
		iRetCode = 0;
		goto end;
	}
	TestThread(); //录音并打印接收到的语音数据

end:
	if(FreeAudio() != 0)
	{
	  printf("FreeAudio Error!\n");
  	  iRetCode = 0;	  
	}

	return iRetCode;
}

