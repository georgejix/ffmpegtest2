#include "VideoToPng.h"

#define WORD uint16_t
#define DWORD uint32_t
#define LONG int32_t
typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

void saveBMP(struct SwsContext *img_convert_ctx, AVFrame *frame, char *filename)
{
    //1 先进行转换,  YUV420=>RGB24:
    int w = frame->width;
    int h = frame->height;


    /*uint8_t *inbuf[4];
    uint8_t *outbuf[4];
    inbuf[0] = (uint8_t *)av_malloc()
    int inlinesize[4] = {w, w/2, w/2, 0};
    int outlinesize[4] = {3*w, 0, 0, 0};*/

    int numBytes=avpicture_get_size(AV_PIX_FMT_RGB24, w, h);
    uint8_t *buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
    AVFrame *pFrameRGB = av_frame_alloc();
    avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24, w, h);
    sws_scale(img_convert_ctx, frame->data, frame->linesize,
              0, h, pFrameRGB->data, pFrameRGB->linesize);

    //2 构造 BITMAPINFOHEADER
    BITMAPINFOHEADER header;
    header.biSize = sizeof(BITMAPINFOHEADER);


    header.biWidth = w;
    header.biHeight = h*(-1);
    header.biBitCount = 24;
    header.biCompression = 0;
    header.biSizeImage = 0;
    header.biClrImportant = 0;
    header.biClrUsed = 0;
    header.biXPelsPerMeter = 0;
    header.biYPelsPerMeter = 0;
    header.biPlanes = 1;

    //3 构造文件头
    BITMAPFILEHEADER bmpFileHeader = {0,};
    //HANDLE hFile = NULL;
    DWORD dwTotalWriten = 0;
    DWORD dwWriten;

    bmpFileHeader.bfType = 0x4d42; //'BM';
    bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) - 2 + numBytes;
    bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) - 2;

    FILE* pf = fopen(filename, "wb");
    //fwrite(&bmpFileHeader, 1, sizeof(BITMAPFILEHEADER), pf);
    fwrite(&(bmpFileHeader.bfType), 1, sizeof(bmpFileHeader.bfType), pf);
    fwrite(&(bmpFileHeader.bfSize), 1, sizeof(BITMAPFILEHEADER) - 4, pf);
    fwrite(&header, 1, sizeof(BITMAPINFOHEADER), pf);
    fwrite(pFrameRGB->data[0], 1, numBytes, pf);
    fclose(pf);


    //释放资源
    //av_free(buffer);
    av_freep(&pFrameRGB[0]);
    av_free(pFrameRGB);
}

void decode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt, SwsContext *sws_ctx, const char *outfilename, int *frame_count){
	int got;
	char buf[1024];
	avcodec_decode_video2(ctx, frame, &got, pkt);
	if(got){
		snprintf(buf, sizeof(buf), "%s-%d.bmp", outfilename, *frame_count);
		saveBMP(sws_ctx, frame, buf);
		(*frame_count)++;
	}
}

void videoToPng(const char *in, const char *out){
	printf("begin videotopng %s\n", in);

	AVFormatContext *fmt_ctx = NULL;
	AVCodec *codec = NULL;
	AVCodecContext *codec_ctx = NULL;
	int video_index;
	AVFrame *frame = NULL;
	AVPacket *pkt = NULL;
	SwsContext *sws_ctx = NULL;
	int frame_count, ret;

	av_register_all();
	ret = avformat_open_input(&fmt_ctx, in, NULL, NULL);
	if(ret < 0){
		printf("avformat_open_input failed");
		return;
	}
	avformat_find_stream_info(fmt_ctx, NULL);
	video_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
	if(video_index < 0){
		printf("av_find_best_stream failed");
		return;
	}
	codec_ctx = avcodec_alloc_context3(codec);
	avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[video_index]->codecpar);
	frame = av_frame_alloc();
	pkt = av_packet_alloc();
	av_init_packet(pkt);
	pkt->data = NULL;
	pkt->size = 0;

	avcodec_open2(codec_ctx, codec, NULL);
	sws_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
			codec_ctx->width, codec_ctx->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
	if(!sws_ctx){
		printf("sws_getContext failed");
		return;
	}
	while(av_read_frame(fmt_ctx, pkt) >= 0){
		printf("read one pkt \n");
		if(pkt->stream_index == video_index){
			decode(codec_ctx, frame, pkt, sws_ctx, out, &frame_count);
		}
		av_packet_unref(pkt);
	}
	pkt->data = NULL;
	pkt->size = 0;
	decode(codec_ctx, frame, pkt, sws_ctx, out, &frame_count);
	avformat_free_context(fmt_ctx);
	avcodec_free_context(&codec_ctx);
	av_frame_free(&frame);


}
