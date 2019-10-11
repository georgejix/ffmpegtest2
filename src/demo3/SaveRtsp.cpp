#include "SaveRtsp.h"
void saveRtsp(const char *in, const char *out){
	printf("in: %s \nout: %s \n", in, out);

	AVFormatContext *fmt_ctx = NULL, *fmt_ctx_out = NULL;
	int ret, video_index, audio_index, i;
	AVPacket *pkt = NULL;

	av_register_all();
	avformat_network_init();
	ret = avformat_open_input(&fmt_ctx, in, NULL, NULL);
	if(ret < 0){
		printf("avformat_open_input failed \n");
		return;
	}else{
		printf("start save \n");
	}
	avformat_find_stream_info(fmt_ctx, NULL);
	video_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, -1);
	audio_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, -1);
	printf("video_index=%d, audio_index=%d\n", video_index, audio_index);

	ret = avformat_alloc_output_context2(&fmt_ctx_out, NULL, NULL, out);
	if(ret < 0){
		printf("avformat_alloc_output_context2 failed \n");
		return;
	}
	ret = avio_open2(&(fmt_ctx_out->pb), out, AVIO_FLAG_WRITE, NULL, NULL);
	if(ret < 0){
		printf("avio_open2 failed \n");
		return;
	}
	for(i = 0; i < fmt_ctx->nb_streams; i++){
		AVStream *stream = avformat_new_stream(fmt_ctx_out, fmt_ctx->streams[i]->codec->codec);
		//avcodec_copy_context(stream->codec, fmt_ctx->streams[i]->codec);
		avcodec_parameters_to_context(fmt_ctx_out->streams[i]->codec, fmt_ctx->streams[i]->codecpar);
	}
	ret = avformat_write_header(fmt_ctx_out, NULL);
	if(ret < 0){
			printf("avformat_write_header failed \n");
			return;
		}
	pkt = av_packet_alloc();
	av_init_packet(pkt);
	for(i = 0; i < 2000; i++){
		av_read_frame(fmt_ctx, pkt);
		if(pkt->dts != AV_NOPTS_VALUE)
			av_rescale_q(pkt->dts, fmt_ctx->streams[pkt->stream_index]->time_base, fmt_ctx_out->streams[pkt->stream_index]->time_base);
		if(pkt->pts != AV_NOPTS_VALUE)
			av_rescale_q(pkt->pts, fmt_ctx->streams[pkt->stream_index]->time_base, fmt_ctx_out->streams[pkt->stream_index]->time_base);
		if(pkt->duration > 0)
					av_rescale_q(pkt->duration, fmt_ctx->streams[pkt->stream_index]->time_base, fmt_ctx_out->streams[pkt->stream_index]->time_base);
		av_interleaved_write_frame(fmt_ctx_out, pkt);
		av_packet_unref(pkt);
	}
	av_write_trailer(fmt_ctx_out);

	avformat_close_input(&fmt_ctx);
	for(i = 0; i < fmt_ctx_out->nb_streams; i++){
		avcodec_close(fmt_ctx_out->streams[i]->codec);
	}
	avformat_close_input(&fmt_ctx_out);

	printf("end savertsp");
}
