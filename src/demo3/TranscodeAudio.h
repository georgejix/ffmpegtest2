#ifndef TRANSCODE_AUDIO_H
#define TRANSCODE_AUDIO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavutil/opt.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
}
void transcodeAudio2(const char *in, const char *out);

#endif
