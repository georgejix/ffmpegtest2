#include "RtspToRtsp.h"
void rtspToRtsp(const char *in, const char *out){
	printf("in=%s \nout=%s \n", in, out);

	AVFormatContext *fmt_ctx = NULL, *fmt_ctx_out = NULL;
	AVDictionary *options = NULL;
	int ret, i;
	AVPacket *pkt = NULL;

	av_register_all();
	avformat_network_init();
	av_dict_set(&options, "rtsp_transport", "udp", 0);
	ret = avformat_open_input(&fmt_ctx, in, NULL, &options);
	if(ret < 0){
		printf("avformat_open_input failed \n");
		return;
	}
	avformat_find_stream_info(fmt_ctx, NULL);
	ret = avformat_alloc_output_context2(&fmt_ctx_out, NULL, "flv", out);
	if(ret < 0){
		printf("avformat_alloc_output_context2 failed \n");
		return;
	}
	avio_open2(&fmt_ctx_out->pb, out, AVIO_FLAG_READ_WRITE, NULL, NULL);
	for(i = 0; i < fmt_ctx->nb_streams; i++){
		avformat_new_stream(fmt_ctx_out, fmt_ctx->streams[i]->codec->codec);
		avcodec_parameters_to_context(fmt_ctx_out->streams[i]->codec, fmt_ctx->streams[i]->codecpar);
	}
	pkt = av_packet_alloc();
	av_init_packet(pkt);
	avformat_write_header(fmt_ctx_out, NULL);
	do{
		av_read_frame(fmt_ctx, pkt);
		if(pkt->dts != AV_NOPTS_VALUE)
			av_rescale_q(pkt->dts, fmt_ctx->streams[pkt->stream_index]->time_base, fmt_ctx_out->streams[pkt->stream_index]->time_base);
		if(pkt->pts != AV_NOPTS_VALUE)
					av_rescale_q(pkt->pts, fmt_ctx->streams[pkt->stream_index]->time_base, fmt_ctx_out->streams[pkt->stream_index]->time_base);
		if(pkt->duration > 0)
					av_rescale_q(pkt->duration, fmt_ctx->streams[pkt->stream_index]->time_base, fmt_ctx_out->streams[pkt->stream_index]->time_base);
		av_interleaved_write_frame(fmt_ctx_out, pkt);
		av_packet_unref(pkt);
	}while(pkt);
	av_write_trailer(fmt_ctx_out);

	avformat_close_input(&fmt_ctx);
	for(i = 0; i < fmt_ctx_out->nb_streams; i++){
		avcodec_close(fmt_ctx_out->streams[i]->codec);
	}
	avformat_close_input(&fmt_ctx_out);

	printf("end rtsptortsp \n");
}
