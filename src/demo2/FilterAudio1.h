#ifndef FILTER_AUDIO_1_H
#define FILTER_AUDIO_1_H
#include <unistd.h>
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
}
int filterAudio1(const char *infile);
#endif
