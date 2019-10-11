#include <stdio.h>
#include <stdlib.h>
extern "C"{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
}
void encodeH264(const char *filename);
