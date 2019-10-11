#include "Rescale.h"
void rescale(const char *in, const char *out){
	printf("begin rescale \n");

	AVFormatContext *fmt_ctx_in = NULL, *fmt_ctx_out = NULL;
	int ret, video_index, result;
	AVPacket *pkt = NULL, *pkt_out = NULL;
	AVCodec *encodec = NULL, *decodec = NULL;
	AVStream *stream = NULL;
	SwsContext *sws_ctx = NULL;
	int src_width, src_height;
	AVFrame *frame = NULL, *frame_out = NULL;
	AVCodecContext *codec_ctx = NULL;

	av_register_all();
	avcodec_register_all();
	avformat_open_input(&fmt_ctx_in, in, NULL, NULL);
	avformat_find_stream_info(fmt_ctx_in, NULL);
	video_index = av_find_best_stream(fmt_ctx_in, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, -1);

	avformat_alloc_output_context2(&fmt_ctx_out, NULL, NULL, out);
	avio_open2(&(fmt_ctx_out->pb), out, AVIO_FLAG_WRITE, NULL, NULL);
	encodec = avcodec_find_encoder(fmt_ctx_in->streams[video_index]->codec->codec_id);
	codec_ctx = avcodec_alloc_context3(encodec);
	stream = avformat_new_stream(fmt_ctx_out, encodec);
	avcodec_parameters_to_context(stream->codec, fmt_ctx_in->streams[video_index]->codecpar);
	stream->codec->width = 640;
	stream->codec->height = 480;
	stream->codec->pix_fmt = fmt_ctx_in->streams[video_index]->codec->pix_fmt;
	stream->codec->gop_size = 30;
	stream->codec->bit_rate = 20000;
	stream->codec->max_b_frames = fmt_ctx_in->streams[video_index]->codec->max_b_frames;
	stream->codec->time_base = fmt_ctx_in->streams[video_index]->codec->time_base;
	stream->codec->time_base.den = 24;
	stream->codec->framerate = fmt_ctx_in->streams[video_index]->codec->framerate;
	stream->time_base = fmt_ctx_in->streams[video_index]->time_base;
	if (encodec->id == AV_CODEC_ID_H264)
	        av_opt_set(stream->codec->priv_data, "preset", "slow", 0);
	if (fmt_ctx_out->oformat->flags & AVFMT_GLOBALHEADER)
		stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	avformat_write_header(fmt_ctx_out, NULL);

	decodec = avcodec_find_decoder(fmt_ctx_in->streams[video_index]->codec->codec_id);
	ret = avcodec_open2(fmt_ctx_in->streams[video_index]->codec, decodec, NULL);

	//av_opt_set(stream->codec->priv_data, "preset", "slow", 0);
	ret = avcodec_open2(stream->codec, encodec, NULL);

	src_width = fmt_ctx_in->streams[video_index]->codec->width;
	src_height = fmt_ctx_in->streams[video_index]->codec->height;
	sws_ctx = sws_getContext(src_width, src_height,
			fmt_ctx_in->streams[video_index]->codec->pix_fmt, stream->codec->width, stream->codec->height, stream->codec->pix_fmt,
			SWS_BILINEAR, NULL, NULL, NULL);

	pkt = av_packet_alloc();
	av_init_packet(pkt);
	pkt_out = av_packet_alloc();
	av_init_packet(pkt_out);
	frame = av_frame_alloc();
	frame_out = av_frame_alloc();
	while(1){
		ret = av_read_frame(fmt_ctx_in, pkt);
		if(ret < 0)
			break;
		if(pkt->stream_index != video_index)
			continue;
		pkt->stream_index = 0;
		avcodec_decode_video2(fmt_ctx_in->streams[video_index]->codec, frame, &result, pkt);
		if(result <= 0)
			continue;
		printf("pkt.pts = %d stream.ts = %d  frame.pts = %d codec.ts = %d \n", pkt->pts, fmt_ctx_in->streams[video_index]->time_base.den,
				frame->pts, fmt_ctx_in->streams[video_index]->codec->time_base.den);
		av_packet_unref(pkt);
		frame_out->width = stream->codec->width;
		frame_out->height = stream->codec->height;
		frame_out->format = stream->codec->pix_fmt;
		frame_out->pts = frame->pts;
		av_frame_get_buffer(frame_out, 16);
		sws_scale(sws_ctx, frame->data, frame->linesize, 0, src_height, frame_out->data, frame_out->linesize);
		avcodec_encode_video2(stream->codec, pkt_out, frame_out, &result);
		if(result < 0)
			continue;

		pkt_out->dts = av_rescale_q(frame->pkt_dts, fmt_ctx_in->streams[video_index]->time_base, stream->time_base);
		pkt_out->pts = av_rescale_q(frame->pkt_pts, fmt_ctx_in->streams[video_index]->time_base, stream->time_base);
		pkt_out->duration = av_rescale_q(frame->pkt_duration, fmt_ctx_in->streams[video_index]->time_base, stream->time_base);

		printf("pkt_out.pts = %d stream.ts = %d frame.pts = %d codec.ts = %d \n", pkt_out->pts, stream->time_base.den,
				frame_out->pts, stream->codec->time_base.den);

		av_interleaved_write_frame(fmt_ctx_out, pkt_out);
		av_packet_unref(pkt_out);
	}
	av_write_trailer(fmt_ctx_out);

	avcodec_close(fmt_ctx_in->streams[0]->codec);
	avformat_close_input(&fmt_ctx_in);
	avformat_free_context(fmt_ctx_out);

	printf("end rescale \n");
}
