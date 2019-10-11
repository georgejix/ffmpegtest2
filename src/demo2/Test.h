#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
extern "C"{
#include "libavutil/log.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/frame.h"
#include "libavformat/avio.h"
}

void test(const char *infile);

#endif
