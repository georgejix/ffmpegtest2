#ifndef DECODE_AUDIO_H
#define DECODE_AUDIO_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern "C"{
#include "libavutil/frame.h"
#include "libavutil/mem.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"	//用于封装与解封装操作
}
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
using namespace std;

void decodeAudio(const char *inputFile, const char *outputFile);

#endif
