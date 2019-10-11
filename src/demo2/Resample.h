/*
 * Resample.h
 *
 *  Created on: 2019年9月5日
 *      Author: georgejx
 */

#ifndef SRC_RESAMPLE_H_
#define SRC_RESAMPLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>

extern "C"{
#include "libavutil/opt.h"
#include "libavutil/channel_layout.h"
#include "libavutil/samplefmt.h"
#include "libswresample/swresample.h"
}

void resampleAudio(const char *outfile);

#endif /* SRC_RESAMPLE_H_ */
