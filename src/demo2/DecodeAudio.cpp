#include "DecodeAudio.h"

void decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame,
                   FILE *outfile)
{
    int i, ch;
    int ret, data_size;

    /* send the packet with the compressed data to the decoder */
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error submitting the packet to the decoder\n");
        exit(1);
    }

    /* read all the output frames (in general there may be any number of them */
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }
        data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
        if (data_size < 0) {
            /* This should not occur, checking just for paranoia */
            fprintf(stderr, "Failed to calculate data size\n");
            exit(1);
        }
        for (i = 0; i < frame->nb_samples; i++)
            for (ch = 0; ch < dec_ctx->channels; ch++)
                fwrite(frame->data[ch] + data_size*i, 1, data_size, outfile);
    }
}

void decodeAudio(const char *inputFile, const char *outputFile){
	cout << "begin decodeaudio" << endl;

	AVPacket *avPacket;
	AVCodec *avCodec;
	AVCodecParserContext *avCodecParserContext;
	AVCodecContext *avCodecContext;
	FILE *inFile, *outFile;
	uint8_t inbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
	uint8_t *data;
	size_t data_size;
	AVFrame *decoded_frame = NULL;
	int ret, len;

	av_register_all();
	avPacket = av_packet_alloc();
	avCodec = avcodec_find_decoder(AV_CODEC_ID_AAC);
	if(!avCodec){
		fprintf(stderr, "Codec not found\n");
		exit(1);
	}
	avCodecParserContext = av_parser_init(avCodec->id);
	if(!avCodecParserContext){
		fprintf(stderr, "parser not found\n");
		exit(1);
	}
	avCodecContext = avcodec_alloc_context3(avCodec);
	if (!avCodecContext) {
		fprintf(stderr, "Could not allocate audio codec context\n");
		exit(1);
	}
	if(avcodec_open2(avCodecContext, avCodec, NULL) < 0){
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}

	inFile = fopen(inputFile, "rb");
	if (!inFile) {
		fprintf(stderr, "Could not open %s\n", inputFile);
		exit(1);
	}
	outFile = fopen(outputFile, "wb");
	if (!outFile) {
		fprintf(stderr, "Could not open %s\n", outputFile);
		exit(1);
	}
	data = inbuf;
	data_size = fread(inbuf, 1, AUDIO_INBUF_SIZE, inFile);

	while(data_size > 0){
		if(!decoded_frame){
			if(!(decoded_frame = av_frame_alloc())){
				fprintf(stderr, "could not allocate audio frame");
				exit(1);
			}
		}
		ret = av_parser_parse2(avCodecParserContext, avCodecContext, &avPacket->data, &avPacket->size,
				data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

		if(ret < 0){
			fprintf(stderr, "err while parsing");
			exit(1);
		}
		data += ret;
		data_size -= ret;

		if(avPacket->size){
			decode(avCodecContext, avPacket, decoded_frame, outFile);
		}

		if(data_size < AUDIO_REFILL_THRESH){
			printf("rest byte length = %d\n", data_size);
			//剩余字节挪到数组开头，然后再读入数据，将数组填充满
			memmove(inbuf, data, data_size);
			data = inbuf;
			len = fread(data + data_size, 1, AUDIO_INBUF_SIZE - data_size, inFile);
			if(len > 0){
				data_size += len;
			}
		}
	}

	avPacket->data = NULL;
	avPacket->size = 0;
	decode(avCodecContext, avPacket, decoded_frame, outFile);

	fclose(outFile);
	fclose(inFile);

	avcodec_free_context(&avCodecContext);
	av_parser_close(avCodecParserContext);
	av_frame_free(&decoded_frame);
	av_packet_free(&avPacket);
}
