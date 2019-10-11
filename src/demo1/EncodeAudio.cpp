#include "EncodeAudio.h"

//check that a given sample format is supported by the encoder
int check_sample_fmt(const AVCodec * codec, enum AVSampleFormat sample_fmt){
	const enum AVSampleFormat *p = codec->sample_fmts;
	while(*p != AV_SAMPLE_FMT_NONE){
		if(*p == sample_fmt){
			return 1;
		}
		p++;
	}
	return 0;
}

//just pick the highest supported samplerate
int select_sample_rate(const AVCodec *codec){
	const int *p;
	int best_samplerate = 0;

	if(!codec->supported_samplerates)
		return 44100;

	p = codec->supported_samplerates;
	while(*p){
		if(!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate)){
			best_samplerate = *p;
		}
		p++;
	}
	return best_samplerate;
}

//select layout with the highest channel count
int select_channel_layout(const AVCodec *codec){
	const uint64_t *p;
	uint64_t best_ch_layout = 0;
	int best_nb_channels = 0;

	if(!codec->channel_layouts)
		return AV_CH_LAYOUT_STEREO;

	p = codec->channel_layouts;
	while(*p){
		int nb_channels = av_get_channel_layout_nb_channels(*p);
		if(nb_channels > best_nb_channels){
			best_ch_layout = *p;
			best_nb_channels = nb_channels;
		}
		p++;
	}
	return best_ch_layout;
}


void encode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt, FILE *output){
	int ret;

	ret = avcodec_send_frame(ctx, frame);
	if(ret < 0){
		fprintf(stderr, "Error sending the frame to the encoder");
		exit(1);
	}

	while(ret >= 0){
		ret = avcodec_receive_packet(ctx, pkt);
		if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if(ret < 0){
			fprintf(stderr, "Error encoding audio frame");
			exit(1);
		}

		fwrite(pkt->data, 1, pkt->size, output);
		av_packet_unref(pkt);
	}
}

int encodeAudio(const char *outputFile){
	printf("begin encodeAudio\n");
	printf("%s\n", outputFile);

	AVCodec *avCodec;
	AVCodecContext *avCodecContext;
	FILE *outFile;
	AVPacket *avPacket;
	AVFrame *avFrame;
	int ret, i, j, k;
	float t, tincr;
	uint16_t *samples;

	av_register_all();
	avCodec = avcodec_find_encoder(AV_CODEC_ID_MP2);
	avCodecContext = avcodec_alloc_context3(avCodec);
	avCodecContext->bit_rate = 64000;
	avCodecContext->sample_fmt = AV_SAMPLE_FMT_S16;
	if(!check_sample_fmt(avCodec, avCodecContext->sample_fmt)){
		fprintf(stderr, "Encoder does not support sample format %s", av_get_sample_fmt_name(avCodecContext->sample_fmt));
		exit(1);
	}
	avCodecContext->sample_rate = select_sample_rate(avCodec);
	avCodecContext->channel_layout = select_channel_layout(avCodec);
	avCodecContext->channels = av_get_channel_layout_nb_channels(avCodecContext->channel_layout);
	avcodec_open2(avCodecContext, avCodec, NULL);
	outFile = fopen(outputFile, "wb");
	avPacket = av_packet_alloc();
	avFrame = av_frame_alloc();
	avFrame->nb_samples = avCodecContext->frame_size;
	avFrame->format = avCodecContext->sample_fmt;
	avFrame->channel_layout = avCodecContext->channel_layout;
	ret = av_frame_get_buffer(avFrame, 0);
	if(ret < 0){
		fprintf(stderr, "Could not allocate audio data buffers\n");
		exit(1);
	}

	t = 0;
	tincr = 2 * M_PI * 440.0 / avCodecContext->sample_rate;
	for(i = 0; i < 200; i++){
		ret = av_frame_make_writable(avFrame);
		if(ret < 0){
			printf("av_frame_make_writable error \n");
			exit(1);
		}
		samples = (uint16_t*)avFrame->data[0];
		for(j = 0; j < avCodecContext->frame_size; j++){
			samples[2*j] = (int)(sin(t) * 10000);

			for(k = 1; k < avCodecContext->channels; k++){
				samples[2*j + k] = samples[2*j];
			}
			t += tincr;
		}
		encode(avCodecContext, avFrame, avPacket, outFile);
	}
	encode(avCodecContext, avFrame, avPacket, outFile);

	fclose(outFile);
	av_frame_free(&avFrame);
	av_packet_free(&avPacket);
	avcodec_free_context(&avCodecContext);

	printf("end encodeAudio\n");
	return 0;
}




















