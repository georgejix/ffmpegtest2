#include "Resample.h"
void make_dsi( unsigned int sampling_frequency_index, unsigned int channel_configuration, unsigned   char* dsi )
 {
  unsigned int object_type = 2; // AAC LC by default
  dsi[0] = (object_type<<3) | (sampling_frequency_index>>1);
  dsi[1] = ((sampling_frequency_index&1)<<7) | (channel_configuration<<3);
 };
 int get_sr_index(unsigned int sampling_frequency)
 {
  switch (sampling_frequency) {
   case 96000: return 0;
   case 88200: return 1;
   case 64000: return 2;
   case 48000: return 3;
   case 44100: return 4;
   case 32000: return 5;
   case 24000: return 6;
   case 22050: return 7;
   case 16000: return 8;
   case 12000: return 9;
   case 11025: return 10;
   case 8000:  return 11;
   case 7350:  return 12;
   default:    return 0;
  }
};
void resample(const char *in, const char *out){
	printf("begin resample \n");

	AVFormatContext *fmt_ctx_in = NULL, *fmt_ctx_out = NULL;
		int ret, audio_index, result, total_duration;
		AVPacket *pkt = NULL, *pkt_out = NULL;
		AVCodec *encodec = NULL, *decodec = NULL;
		AVStream *stream = NULL;
		AVFrame *frame = NULL, *frame_out = NULL;
		AVCodecContext *codec_ctx = NULL;
		SwrContext *swr_ctx = NULL;

		av_register_all();
		avcodec_register_all();
		avformat_open_input(&fmt_ctx_in, in, NULL, NULL);
		avformat_find_stream_info(fmt_ctx_in, NULL);
		audio_index = av_find_best_stream(fmt_ctx_in, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, -1);

		avformat_alloc_output_context2(&fmt_ctx_out, NULL, NULL, out);
		avio_open2(&(fmt_ctx_out->pb), out, AVIO_FLAG_WRITE, NULL, NULL);
		encodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
		stream = avformat_new_stream(fmt_ctx_out, encodec);
		codec_ctx = avcodec_alloc_context3(encodec);
		//avcodec_parameters_to_context(codec_ctx, fmt_ctx_in->streams[audio_index]->codecpar);

		codec_ctx->bit_rate = 40000;
		codec_ctx->sample_fmt = encodec->sample_fmts[0];
		codec_ctx->sample_rate = 48000;
		codec_ctx->channel_layout = AV_CH_LAYOUT_MONO;
		codec_ctx->channels = av_get_channel_layout_nb_channels(codec_ctx->channel_layout);
		codec_ctx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
		unsigned char dsi[2];
		make_dsi( (unsigned int)get_sr_index( (unsigned int)codec_ctx->sample_rate ), (unsigned int)codec_ctx->channels, dsi);
		codec_ctx->extradata = (uint8_t*)dsi;
		codec_ctx->extradata_size = 2;
		stream->codec = codec_ctx;
		avcodec_parameters_from_context(stream->codecpar, codec_ctx);
		stream->time_base = {1, 1000};
		if (fmt_ctx_out->oformat->flags & AVFMT_GLOBALHEADER)
			fmt_ctx_out->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

		avformat_write_header(fmt_ctx_out, NULL);
		decodec = avcodec_find_decoder(fmt_ctx_in->streams[audio_index]->codec->codec_id);
		ret = avcodec_open2(fmt_ctx_in->streams[audio_index]->codec, decodec, NULL);
		ret = avcodec_open2(stream->codec, encodec, NULL);

		swr_ctx = swr_alloc();
		av_opt_set_int(swr_ctx, "in_channel_layout", fmt_ctx_in->streams[audio_index]->codec->channel_layout, 0);
		av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", fmt_ctx_in->streams[audio_index]->codec->sample_fmt, 0);
		av_opt_set_int(swr_ctx, "in_sample_rate", fmt_ctx_in->streams[audio_index]->codec->sample_rate, 0);

		av_opt_set_int(swr_ctx, "out_channel_layout", stream->codec->channel_layout, 0);
		av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", stream->codec->sample_fmt, 0);
		av_opt_set_int(swr_ctx, "out_sample_rate", stream->codec->sample_rate, 0);
		swr_init(swr_ctx);

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
			if(pkt->stream_index != audio_index)
				continue;

			pkt->stream_index = 0;
			avcodec_decode_audio4(fmt_ctx_in->streams[audio_index]->codec, frame, &result, pkt);
			if(result <= 0)
				continue;
			frame_out->nb_samples = av_rescale_rnd(frame->nb_samples, stream->codec->sample_rate, frame->sample_rate, AV_ROUND_UP);
			frame_out->sample_rate = stream->codec->sample_rate;
			frame_out->channel_layout = stream->codec->channel_layout;
			frame_out->format = stream->codec->sample_fmt;
			frame_out->channels = stream->codec->channels;
			av_frame_get_buffer(frame_out, 2);
			swr_convert(swr_ctx, frame_out->data, frame_out->nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
			avcodec_encode_audio2(stream->codec, pkt_out, frame_out, &result);
			if(result < 0)
				continue;

			pkt_out->dts = av_rescale_q(frame->pkt_dts, fmt_ctx_in->streams[audio_index]->time_base, stream->time_base);
			pkt_out->pts = av_rescale_q(frame->pkt_pts, fmt_ctx_in->streams[audio_index]->time_base, stream->time_base);
			pkt_out->duration = av_rescale_q(frame->pkt_duration, fmt_ctx_in->streams[audio_index]->time_base, stream->time_base);
			total_duration += pkt_out->duration;

			printf("pkt_out.pts = %d stream.ts = %d frame.pts = %d codec.ts = %d pkt_out.duration = %d pkt_out.dts = %d \n", pkt_out->pts, stream->time_base.den,
					frame_out->pts, stream->codec->time_base.den, pkt_out->duration, pkt_out->dts);
			printf("frame.pts = %d codec.ts = %d pkt.pts = %d stream.ts = %d \n", frame->pts, fmt_ctx_in->streams[audio_index]->codec->time_base.den,
					pkt->pts, fmt_ctx_in->streams[audio_index]->time_base.den);

			av_interleaved_write_frame(fmt_ctx_out, pkt_out);
			av_packet_unref(pkt);
			av_packet_unref(pkt_out);
		}
		pkt_out->data = NULL;
		pkt_out->size = 0;
		av_interleaved_write_frame(fmt_ctx_out, pkt_out);
		av_write_trailer(fmt_ctx_out);
		printf("%d \n", total_duration);

		avcodec_close(fmt_ctx_in->streams[0]->codec);
		avformat_close_input(&fmt_ctx_in);
		avformat_free_context(fmt_ctx_out);

	printf("end resample \n");
}
