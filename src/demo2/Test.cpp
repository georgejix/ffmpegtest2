#include "Test.h"
void test(const char *infile){
	AVFormatContext *avformat_ctx = NULL;
	AVPacket *pkt = NULL;
	AVCodec *codec = NULL;
	AVCodecContext *codec_ctx = NULL;
	AVFrame *frame = NULL;
	int ret, best_index;


	av_log_set_level(AV_LOG_INFO);
	av_register_all();
	avformat_open_input(&avformat_ctx, infile, NULL, NULL);
	av_dump_format(avformat_ctx, 0, infile, 0);

	avformat_find_stream_info(avformat_ctx, NULL);
	best_index = av_find_best_stream(avformat_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
	codec_ctx = avcodec_alloc_context3(codec);
	avcodec_parameters_to_context(codec_ctx, avformat_ctx->streams[best_index]->codecpar);
	avcodec_open2(codec_ctx, codec, NULL);

	pkt = av_packet_alloc();
	av_init_packet(pkt);
	frame = av_frame_alloc();
	while(av_read_frame(avformat_ctx, pkt) >= 0){
		if(best_index == pkt->stream_index){
			avcodec_send_packet(codec_ctx, pkt);
			avcodec_receive_frame(codec_ctx, frame);
			printf("size=%d, frame channels= %d %0x, %0x %d %s \n", pkt->size, frame->channels, frame->data[0], frame->data[1],
					av_get_bytes_per_sample(codec_ctx->sample_fmt), av_get_sample_fmt_name(codec_ctx->sample_fmt));
			printf("%0x %0x %0x %0x %0x %0x %0x %0x \n", *frame->data[0], *(frame->data[0] + 1), *(frame->data[0] + 2), *(frame->data[0] + 3),
					*(frame->data[1]), *(frame->data[1] + 1), *(frame->data[1] + 2), *(frame->data[1] + 3));
			av_packet_unref(pkt);
		}
	}

	avformat_close_input(&avformat_ctx);

	AVFrame *aframe = NULL;
	aframe = av_frame_alloc();
	aframe->format = AV_SAMPLE_FMT_FLT;
	aframe->nb_samples = 1024;
	aframe->channel_layout = AV_CH_LAYOUT_STEREO;
	aframe->channels = 2;
	av_frame_get_buffer(aframe, 0);
	printf("%0x, %0x \n", aframe->data[0], aframe->data[1]);

	printf("%d \n", ~5);

	int x = 1279;
	int a;
	a = 0;
	printf("x=%0x a=%0x res=%0x \n", x, a, FFALIGN(x, a));
	a = 1;
		printf("x=%0x a=%0x res=%0x \n", x, a, FFALIGN(x, a));
		a = 2;
			printf("x=%0x a=%0x res=%0x \n", x, a, FFALIGN(x, a));
			a = 3;
				printf("x=%0x a=%0x res=%0x \n", x, a, FFALIGN(x, a));
				a = 16;
					printf("x=%0x a=%0x res=%0x \n", x, a, FFALIGN(x, a));
}
