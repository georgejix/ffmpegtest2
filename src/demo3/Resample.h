#ifndef RESAMPLE_H
#define RESAMPLE_H

#include <stdio.h>
#include <stdlib.h>
extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
}
void resample(const char *in, const char *out);

#endif
