#include "ReadPtsDts.h"
void readPtsDts(const char *in){
	AVFormatContext *fmt_ctx = NULL;
	int video_index, audio_index, ret, index;
	AVFrame *frame = NULL;
	AVPacket *pkt = NULL;

	printf("readptsdts in=%s \n", in);
	av_register_all();
	avcodec_register_all();
	ret = avformat_open_input(&fmt_ctx, in, NULL, NULL);
	avformat_find_stream_info(fmt_ctx, NULL);
	audio_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	video_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	avcodec_open2(fmt_ctx->streams[audio_index]->codec, avcodec_find_decoder(fmt_ctx->streams[audio_index]->codecpar->codec_id), NULL);
	pkt = av_packet_alloc();
	av_init_packet(pkt);
	while(1){
		ret = av_read_frame(fmt_ctx, pkt);
		if(ret < 0){
			break;
		}
		if(audio_index == pkt->stream_index){
			printf("audio pts=%ld, dts=%ld, stream.den=%d, codec.den=%d, index=%d, frame_size=%d \n",
					pkt->pts, pkt->dts, fmt_ctx->streams[audio_index]->time_base.den,
					fmt_ctx->streams[audio_index]->codec->time_base.den, index,
					fmt_ctx->streams[audio_index]->codec->frame_size);
			index++;
		}else{

		}
		av_packet_unref(pkt);
	}

	avcodec_close(fmt_ctx->streams[audio_index]->codec);
	avformat_close_input(&fmt_ctx);
	printf("end readPtsDts \n");
}
