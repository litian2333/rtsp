#include <iostream>
#include "./inc/InitConnect.h"
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
}
void callBackTest(unsigned char *buffer, const int &buffer_size,const struct timeval &receive_time,const std::time_t &localTime)
{
    MyBufferInfo info;
    info.buffer = std::string((char*)buffer,buffer_size);
    info.buffer_size = buffer_size;
    info.SecondTime=receive_time.tv_sec;
    info.MillisecondTime = receive_time.tv_usec;
    info.localTime = localTime;
    safe_queue.PushBack(info);
}
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
}
void queueThread()
{
    while (true)
    {
        MyBufferInfo info;

        if(safe_queue.PopFront(info,100))
        {
            auto l_start = std::chrono::high_resolution_clock::now();
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
                save_name ="./save/" + st.str() + ".bin";
                index++;
            }
            else
            {
                last_time = std::to_string(temp1);
                index = 0;
                std::stringstream st;
                st<<temp<<"_"<<index<<"-"<<info.localTime;
                save_name ="/data/jiaojie/RTSP_Laliu/python拉流代码测试/rtsplib_new_git/build/save/" + st.str() + ".bin";
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
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "请输入所有参数：" << std::endl;
#define test
#ifdef test
        std::string  path  = "rtsp://admin:wanji123@192.168.3.35:554/cam/realmonitor?channel=1&subtype=0";
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
    while (1)
    {
        std::cout<<"main"<<std::endl;
    }
    return 0;
}
