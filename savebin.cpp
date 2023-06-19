#include <iostream>
#include "./inc/InitConnect.h"
//#include "./lib/include/opencv2/opencv.hpp"
//#include "H264Decode.h"
#include "CSafeDataDeque.h"
#include <thread>
#include <chrono>
#include <cmath>
#include <sstream>
#include <fstream>
typedef struct myBufferInfo
{
    std::string buffer;
    int buffer_size;
    long int SecondTime;
    unsigned int MillisecondTime;
    std::time_t localTime;
}MyBufferInfo;
CSafeDataDeque <MyBufferInfo>safe_queue;
static std::string last_time = "";
static int index =0;
//typedef void (*CallFunc)(unsigned char *buffer, const int &buffer_size,const unsigned int& SecondTime,const unsigned int&MillisecondTime,const std::time_t &localTime);
void openUrl(const std::string &url,const std::string &name,InitConnect *connect)
{

    connect->OpenUrl(url.c_str(),name.c_str());
//    connect->OpenUrl("rtsp://admin:wanji123@192.168.3.108:554/cam/realmonitor?channel=1&subtype=0","temp1");
//    std::cout<<"ok"<<std::endl;
}
void callBackTest(unsigned char *buffer, const int &buffer_size,const struct timeval &receive_time,const std::time_t &localTime)
{
//    std::cout<<"in callback size is"<<info_queue.size()<<std::endl;
    MyBufferInfo info;
//    info.buffer = buffer;
    info.buffer = std::string((char*)buffer,buffer_size);
    info.buffer_size = buffer_size;
    info.SecondTime=receive_time.tv_sec;
    info.MillisecondTime = receive_time.tv_usec;
    info.localTime = localTime;
    safe_queue.PushBack(info);
//    auto temp = SecondTime-2208988800;
//    double temp1 = MillisecondTime;
////    std::cout<<"ms is:"<<temp1<<std::endl;
//    temp1 = temp1/pow(2,32);
//    temp1=temp1*1000;
//    std::cout<<"receive time is:"<<temp<<"."<<int(temp1)<<"local time is:"<<localTime<<std::endl;
}
//void saveImgThread(cv::Mat img,const std::string &save_path)
//{
//    cv::imwrite(save_path,img);
//}
//void queueThread(H264Decode *decode)
std::string frameJudge(uint8_t *frame)
{

    int temp = 0;
    temp = frame[4] & 31;
    std::string type = "";
    switch (temp) {
        case 7:
        {
            type = "SPS";
        }break;
        case 8:
        {
            type = "PPS";
        }break;
        case 5:
        {
            type = "I";
        }break;
        case 1:
        {
            type = "P";
        }break;
        default:break;
    }
    return type;
    std::cout<<"frame "<<temp<<"TYPE:"<<type<<std::endl;
//
}
void queueThread()
{
    while (true)
    {
        MyBufferInfo info;
        if(safe_queue.PopFront(info,100))
        {
            auto l_start = std::chrono::high_resolution_clock::now();
//            cv::Mat mat;
//            bool succ = decode->decodeOneFrame((uint8_t*)info.buffer.c_str(),info.buffer_size,mat);
            auto type  = frameJudge((uint8_t*)info.buffer.c_str());
            if(type=="P")
                continue;
            auto temp = info.SecondTime;
            auto  temp1 = info.MillisecondTime;
            temp = temp + temp1;
            std::string save_name = "";
            if(std::to_string(temp1) == last_time)
            {
                std::stringstream st;
                st<<temp<<"_"<<index<<"-"<<info.localTime;
                std::cout<<"time focus:------:"<<st.str()<<std::endl;
                save_name ="./save/" + st.str() + ".bin";
                index++;
            }
            else
            {
//                std::cout<<"std::to_string(temp1)"<<std::to_string(temp1)<<std::endl;
                last_time = std::to_string(temp1);
                index = 0;
                std::stringstream st;
                st<<temp<<"_"<<index<<"-"<<info.localTime;
                std::cout<<"time focus:------:"<<st.str()<<std::endl;
                save_name ="./save/" + st.str() + ".bin";
            }

            std::ofstream file;
            file.open(save_name.c_str(), std::ios::out|std::ios::app );
            if (!file)
            {
                std::cout << "open error" << std::endl;
                return ;
            }
            else
            {
                file.write((char*)info.buffer.data(),info.buffer_size);
                file.flush();
                file.close();
                auto l_end = std::chrono::high_resolution_clock::now();
                uint64_t l_us = std::chrono::duration_cast<std::chrono::microseconds>(l_end-l_start).count();
                std::cout << "need time:" << l_us << "us" << std::endl;
            }
        }
    }
}
int main(int argc, char *argv[]){
//    H264Decode *decode = new H264Decode();
    if (argc < 2)
    {
        std::cout << "请输入所有参数：" << std::endl;
#define test
#ifdef test
        std::string  path  = "rtsp://admin:wanji123@10.100.16.1:554/cam/realmonitor?channel=1&subtype=0";
        auto th = std::thread(queueThread);
        th.detach();
        InitConnect  * connect = new InitConnect();
        connect->setCallBackFun(callBackTest);
        connect->OpenUrl(path.c_str(),"th1");
#endif
    }
    else
    {
        std::string  path  = std::string(argv[1]);
        auto th = std::thread(queueThread);
        th.detach();
        InitConnect  * connect = new InitConnect();
        connect->setCallBackFun(callBackTest);
        connect->OpenUrl(path.c_str(),"th1");
    }
//        auto th = std::thread(queueThread);
//        th.detach();
//        InitConnect  * connect = new InitConnect();
//        connect->setCallBackFun(callBackTest);
//        connect->OpenUrl("rtsp://admin:wanji123@192.168.3.58:554/cam/realmonitor?channel=1&subtype=0","th1");

//    auto  th = std::thread(openUrl,"rtsp://admin:wanji123@192.168.3.108:554/cam/realmonitor?channel=1&subtype=0","th1",connect);
//    th.join();
//    InitConnect  * connect1 = new InitConnect();
//    std::this_thread::sleep_for(std::chrono::milliseconds(3));
//    auto  th1 = std::thread(openUrl,"rtsp://admin:wanji123@192.168.3.58:554/cam/realmonitor?channel=1&subtype=0","th2",connect1);
//    th1.join();
    while (1)
    {
        std::cout<<"main"<<std::endl;
    }
    return 0;
}