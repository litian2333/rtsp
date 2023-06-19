//
// Created by wanji on 2022/7/22.
//

#ifndef MYPROJECT_H264DECODE_H
#define MYPROJECT_H264DECODE_H
extern "C"
{
#include "../lib/include/ffmpeg/libavformat/avformat.h"
#include "../lib/include/ffmpeg/libavcodec/avcodec.h"
#include "../lib/include/ffmpeg/libavutil/imgutils.h"
#include "../lib/include/ffmpeg/libavutil/pixfmt.h"
#include "../lib/include/ffmpeg/libswscale/swscale.h"
}
#if !defined (MIN)
#define MIN(A,B) ( (A) < (B) ? (A) : (B) )
#endif
namespace cv
{
    class Mat;
}


typedef struct YUVChannelDef
{
    unsigned int    length;
    unsigned char*  dataBuffer;//视频数据的YUV buffer

}YUVChannel;

typedef struct YUVFrameDef
{
    unsigned int    width;
    unsigned int    height;
    YUVChannel      luma; // Y分量
    YUVChannel      chromaB;// U 分量
    YUVChannel      chromaR;// V 分量
    long long       pts;

}YUVData_Frame;

class H264Decode {
public:
    H264Decode();
    ~H264Decode();
private:
    void initDecoder();
    void copyDecodedFrame(uint8_t* src, uint8_t* dist, int linesize, int width,int height);
public:
    bool decodeOneFrame(uint8_t *H264Buffer, int H264length, cv::Mat &mat_frame);
private:
    AVCodec *codec;
    AVCodecParserContext *parser;
    AVCodecContext *context= NULL;
    AVFrame *frame;
    uint8_t *data;
    size_t   data_size;
    int ret;
    int eof;
    AVPacket *pkt;
};


#endif //MYPROJECT_H264DECODE_H
