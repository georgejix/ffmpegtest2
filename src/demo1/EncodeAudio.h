#ifndef ENCODE_AUDIO_H
#define ENCODE_AUDIO_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavutil/frame.h"
#include "libavutil/samplefmt.h"
#include "libavformat/avformat.h"
}

int encodeAudio(const char *outputFile);

#endif
