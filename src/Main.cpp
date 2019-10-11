//#include "Remuxing.h"
#include "demo2/DecodeAudio.h"
#include "demo1/EncodeAudio.h"
#include "demo2/TranscodeAudio.h"
#include "demo2/Resample.h"
#include <string>
#include "demo2/FilterAudio1.h"
#include "demo2/Test.h"
#include "demo1/EncodeH264.h"
#include "demo1/VideoToPng.h"
#include "demo3/SaveRtsp.h"
#include "demo3/RtspToRtsp.h"
#include "demo3/TranscodeAudio.h"
#include "demo3/ReadPtsDts.h"
#include "demo3/Resample.h"
#include "demo3/Rescale.h"

int main(int argc, char **argv){
	//remuxing();

	string decodeInputAudio = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/decodeAudio.aac";
	string decodeOutputAudio = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/decodeAudio_out.mp3";
	//decodeAudio(decodeInputAudio.c_str(), decodeOutputAudio.c_str());

	string encodeOutputFile = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/encodeAudio.mp2";
	//encodeAudio(encodeOutputFile.c_str());

	string transcodeAudioIn = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/decodeAudio.aac";
	string transcodeAudioOut = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/decodeAudio.mp3";
	//transcodeAudio(transcodeAudioIn.c_str(), transcodeAudioOut.c_str());

	string resampleAudioOut = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/resampleAudio.mp3";
	//resampleAudio(resampleAudioOut.c_str());

	string filterAudioIn = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/decodeAudio.aac";
	//filterAudio1(filterAudioIn.c_str());

	string testFile = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/audio.flv";
	//test(testFile.c_str());

	string encodeH264Out = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/encodeh264.h264";
	//encodeH264(encodeH264Out.c_str());

	string videoToPngIn = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/audio.flv";
	string videoToPngOut = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/bmp/h264pic";
	//videoToPng(videoToPngIn.c_str(), videoToPngOut.c_str());

	string saveRtspIn = "rtmp://127.0.0.1:1935/mytv/1";
	string saveRtspOut = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/rtsp.flv";
	//saveRtsp(saveRtspIn.c_str(), saveRtspOut.c_str());

	string rtspToRtspIn = "rtmp://127.0.0.1:1935/mytv/1";
	string rtspToRtspOut = "rtmp://127.0.0.1:1935/mytv/2";
	//rtspToRtsp(rtspToRtspIn.c_str(), rtspToRtspOut.c_str());

	string transcodeAudioIn2 = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/audio.flv";
	string transcodeAudioOut2 = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/transcode_out";
	//transcodeAudio2(transcodeAudioIn2.c_str(), transcodeAudioOut2.c_str());

	string readPtsDtsIn = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/audio.flv";
	//string readPtsDtsIn = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/transcode_out";
	//readPtsDts(readPtsDtsIn.c_str());

	string resampleIn = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/audio.flv";
	string resampleOut = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/resample.aac";
	resample(resampleIn.c_str(), resampleOut.c_str());

	string rescaleIn = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/audio.flv";
	string rescaleOut = "/home/georgejx/develop/studyfrom2019.5/ffmpeg/video/tmp1/rescale.h264";
	//rescale(rescaleIn.c_str(), rescaleOut.c_str());

	return 0;
}
