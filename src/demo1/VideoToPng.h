#ifndef VIDEO_TO_PNG_H
#define VIDEO_TO_PHG_H

#include <stdio.h>
#include <stdlib.h>
extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include <libswscale/swscale.h>
}
void videoToPng(const char *in, const char *out);

//注意结构体对齐，大端小端模式！！！
#endif
