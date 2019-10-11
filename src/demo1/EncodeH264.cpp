#include "EncodeH264.h"

void encodeH264(const char *filename){
	printf("begin encodeh264");
	AVCodec *codec = NULL;
	AVCodecContext *codec_ctx = NULL;
	FILE *f = NULL;
	AVFrame *frame = NULL;
	int i, y, x, got_output, ret;
	AVPacket *pkt = NULL;
	uint8_t endcode[] = { 0, 0, 1, 0xb7 };

	av_register_all();
	codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	codec_ctx = avcodec_alloc_context3(codec);
	codec_ctx->bit_rate = 400000;
	codec_ctx->width = 352;
	codec_ctx->height = 288;
	codec_ctx->time_base = (AVRational){1, 25};
	codec_ctx->framerate = (AVRational){25, 1};
	codec_ctx->gop_size = 10;
	codec_ctx->max_b_frames = 1;
	codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	if(codec->id == AV_CODEC_ID_H264){
		av_opt_set(codec_ctx->priv_data, "preset", "slow", 0);
	}
	avcodec_open2(codec_ctx, codec, NULL);
	f = fopen(filename, "wb");
	frame = av_frame_alloc();
	frame->format = codec_ctx->pix_fmt;
	frame->width = codec_ctx->width;
	frame->height = codec_ctx->height;
	ret = av_frame_get_buffer(frame, 32);
	pkt = av_packet_alloc();
	for(i = 0; i < 25; i++){
		av_init_packet(pkt);
		pkt->data = NULL;
		pkt->size = 0;

		av_frame_make_writable(frame);
		for(y = 0; y < codec_ctx->height; y++){
			for(x = 0; x < codec_ctx->width; x++){
				frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
			}
		}
		for(y = 0; y < codec_ctx->height/2; y++){
			for(x = 0; x < codec_ctx->width/2; x++){
				frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
				frame->data[2][y * frame->linesize[2] + x] = 64 + y + i * 5;
			}
		}
		frame->pts = i;
		avcodec_encode_video2(codec_ctx, pkt, frame, &got_output);
		if(got_output){
			fwrite(pkt->data, 1, pkt->size, f);
			av_packet_unref(pkt);
		}
		/*avcodec_send_frame(codec_ctx, frame);
		ret = avcodec_receive_packet(codec_ctx, pkt);
		if(ret){
			fwrite(pkt->data, 1, pkt->size, f);
			av_packet_unref(pkt);
		}*/
	}
	for(got_output = 1; got_output; i++){
		avcodec_encode_video2(codec_ctx, pkt, NULL, &got_output);
		if(got_output){
			fwrite(pkt->data, 1, pkt->size, f);
			av_packet_unref(pkt);
		}
	}
	fwrite(endcode, 1, sizeof(endcode), f);
	fclose(f);
	avcodec_free_context(&codec_ctx);
	av_frame_free(&frame);
}
