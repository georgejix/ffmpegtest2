#include "TranscodeAudio.h"
void transcodeAudio2(const char *in, const char *out){
	printf("in=%s\nout=%s\n", in, out);

	AVFormatContext *fmt_ctx = NULL, *fmt_ctx_out = NULL;
	int audio_index, ret, audioCount;
	AVCodec *codec = NULL, *encodec = NULL;
	AVCodecContext *encodec_ctx = NULL;
	AVFilterContext *srcfilter_ctx = NULL, *sinkfilter_ctx = NULL;
	AVFilter *srcfilter = NULL, *sinkfilter = NULL;
	char args[512];
	AVFilterGraph *filtergraph = NULL;
	AVFilterInOut *src_inout = NULL, *sink_inout = NULL;
	AVPacket *pkt = NULL, *pkt_out = NULL;
	AVFrame *frame = NULL, *frame_out = NULL;
	AVStream *stream = NULL, *stream_in = NULL;

	av_register_all();
	avcodec_register_all();
	avformat_network_init();
	avfilter_register_all();
	avformat_open_input(&fmt_ctx, in, nullptr, NULL);
	avformat_find_stream_info(fmt_ctx, NULL);
	audio_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	codec = avcodec_find_decoder(fmt_ctx->streams[audio_index]->codec->codec_id);
	avcodec_open2(fmt_ctx->streams[audio_index]->codec, codec, NULL);
	stream_in = fmt_ctx->streams[audio_index];

	AVOutputFormat *out_fmt = av_guess_format("adts", NULL, NULL);
	avformat_alloc_output_context2(&fmt_ctx_out, out_fmt, NULL, out);
	avio_open2(&(fmt_ctx_out->pb), out, AVIO_FLAG_WRITE, NULL, NULL);
	encodec = avcodec_find_encoder(out_fmt->audio_codec);
	encodec_ctx = avcodec_alloc_context3(encodec);
	encodec_ctx->codec = encodec;
	encodec_ctx->sample_fmt = encodec->sample_fmts[0];
	encodec_ctx->channel_layout = AV_CH_LAYOUT_STEREO;
	encodec_ctx->channels = av_get_channel_layout_nb_channels(encodec_ctx->channel_layout);
	encodec_ctx->sample_rate = 48000;
	encodec_ctx->codec_tag = 0;
	encodec_ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;
	encodec_ctx->frame_size = 1024;
	encodec_ctx->time_base = fmt_ctx->streams[audio_index]->codec->time_base;
	/* Allow the use of the experimental AAC encoder. */
	encodec_ctx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	avcodec_open2(encodec_ctx, encodec, NULL);
	stream = avformat_new_stream(fmt_ctx_out, encodec);
	stream->time_base = stream_in->time_base;
	fmt_ctx_out->streams[0]->codec = encodec_ctx;
	avformat_write_header(fmt_ctx_out, NULL);

	srcfilter = avfilter_get_by_name("abuffer");
	sinkfilter = avfilter_get_by_name("abuffersink");
	snprintf(args, sizeof(args), "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%x",
			fmt_ctx->streams[audio_index]->time_base.num, fmt_ctx->streams[audio_index]->time_base.den,
			fmt_ctx->streams[audio_index]->codec->sample_rate,
			av_get_sample_fmt_name(fmt_ctx->streams[audio_index]->codec->sample_fmt),
			fmt_ctx->streams[audio_index]->codec->channel_layout);
	printf("%s\n", args);
	filtergraph = avfilter_graph_alloc();
	//filtergraph->nb_threads = 1;
	ret = avfilter_graph_create_filter(&srcfilter_ctx, srcfilter, "in", args, NULL, filtergraph);
	if (ret < 0) {
		printf("avfilter_graph_create_filter in failed\n");
		return;
	}
	ret = avfilter_graph_create_filter(&sinkfilter_ctx, sinkfilter, "out", NULL, NULL, filtergraph);
	if (ret < 0) {
		printf("avfilter_graph_create_filter out failed\n");
		return;
	}
	enum AVSampleFormat sample_fmts[] = {encodec_ctx->sample_fmt, AV_SAMPLE_FMT_NONE};
	int64_t layouts[] = {encodec_ctx->channel_layout, -1};
	int rates[] = {encodec_ctx->sample_rate, -1};
	ret = av_opt_set_int_list(sinkfilter_ctx, "sample_fmts", sample_fmts, -1, AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		printf("Cannot set output sample_fmts\n");
		return;
	}
	ret = av_opt_set_int_list(sinkfilter_ctx, "channel_layouts", layouts, -1, AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		printf("Cannot set output channel_layouts\n");
		return;
	}
	ret = av_opt_set_int_list(sinkfilter_ctx, "sample_rates", rates, -1, AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		printf("Cannot set output sample_rates\n");
		return;
	}
	src_inout = avfilter_inout_alloc();
	sink_inout = avfilter_inout_alloc();
	src_inout->name = av_strdup("in");
	src_inout->filter_ctx = srcfilter_ctx;
	src_inout->pad_idx = 0;
	src_inout->next = NULL;
	sink_inout->name = av_strdup("out");
	sink_inout->filter_ctx = sinkfilter_ctx;
	sink_inout->pad_idx = 0;
	sink_inout->next = NULL;
	ret = avfilter_graph_parse_ptr(filtergraph,
			"aresample=48000,aformat=sample_fmts=s16:channel_layouts=stereo"
			//"anull"
			, &sink_inout, &src_inout, NULL);
	if(ret < 0){
		printf("avfilter_graph_parse_ptr failed \n");
		return;
	}
	ret = avfilter_graph_config(filtergraph, NULL);
	if(ret < 0){
			printf("avfilter_graph_config failed \n");
			return;
	}
	av_buffersink_set_frame_size(sinkfilter_ctx, 1024);

	pkt = av_packet_alloc();
	av_init_packet(pkt);
	pkt_out = av_packet_alloc();
	av_init_packet(pkt_out);
	frame = av_frame_alloc();
	frame_out = av_frame_alloc();

	while((ret = av_read_frame(fmt_ctx, pkt)) >= 0){
		if(audio_index != pkt->stream_index)
			continue;
		avcodec_decode_audio4(fmt_ctx->streams[audio_index]->codec, frame, &ret, pkt);
		if(ret){
			av_buffersrc_add_frame_flags(srcfilter_ctx, frame, AV_BUFFERSRC_FLAG_PUSH);
			av_buffersink_get_frame(sinkfilter_ctx, frame_out);
			avcodec_encode_audio2(encodec_ctx, pkt_out, frame_out, &ret);
			if(ret){
				pkt_out->stream_index = 0;
				pkt_out->dts = pkt_out->pts = pkt->pts;
				pkt_out->duration = pkt->duration;
				pkt_out->pos = pkt->pos;
				//pkt_out->dts = av_rescale_q(pkt_out->dts, stream->time_base, encodec_ctx->time_base);
				//pkt_out->pts = av_rescale_q(pkt_out->pts, stream->time_base, encodec_ctx->time_base);
				printf("pkt.dts = %ld, pkt_out.dts = %ld \n", pkt->dts, pkt_out->dts);
				av_interleaved_write_frame(fmt_ctx_out, pkt_out);
				av_packet_unref(pkt_out);
			}
		}
		av_packet_unref(pkt);
	}
	av_write_trailer(fmt_ctx_out);

	avfilter_inout_free(&src_inout);
	avfilter_inout_free(&sink_inout);
	avfilter_free(srcfilter_ctx);
	avfilter_free(sinkfilter_ctx);
	avcodec_close(fmt_ctx->streams[audio_index]->codec);
	avcodec_close(encodec_ctx);
	avformat_close_input(&fmt_ctx);
	avformat_close_input(&fmt_ctx_out);
	printf("end transcode\n");
}

