#ifndef RESCALE_H
#define RESCALE_H

#include <stdio.h>
#include <stdlib.h>
extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include "libavutil/opt.h"
}
void rescale(const char *in, const char *out);

#endif
