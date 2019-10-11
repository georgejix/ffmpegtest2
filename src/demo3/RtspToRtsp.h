#ifndef RTSP_TO_RTSP_H
#define RTSP_TO_RTSP_H

#include <stdio.h>
#include <stdlib.h>
extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}
void rtspToRtsp(const char *in, const char *out);

#endif
