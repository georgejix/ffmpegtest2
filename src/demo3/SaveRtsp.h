#ifndef SAVE_RTSP_H
#define SAVE_RTSP_H

#include <stdio.h>
#include <stdlib.h>
extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}
void saveRtsp(const char *in, const char *out);
#endif
