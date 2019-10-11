#ifndef READ_PTS_DTS_H
#define READ_PTS_DTS_H

#include <stdio.h>
#include <stdlib.h>
extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}
void readPtsDts(const char *in);

#endif
