#include <iostream>
#include "./inc/InitConnect.h"
#include "./lib/include/opencv2/opencv.hpp"
#include "H264Decode.h"
#include "CSafeDataDeque.h"
#include "CommonFunctions.h"
#include <thread>
#include <chrono>
#include <thread>
#include <cmath>
#include <queue>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
typedef struct myBufferInfo
{
    std::string buffer;
    int buffer_size;
    long int SecondTime;
    unsigned int MillisecondTime;
    std::time_t localTime;
}MyBufferInfo;
CSafeDataDeque <MyBufferInfo>safe_queue;
std::queue<MyBufferInfo> buffer_queue;
//typedef void (*CallFunc)(unsigned char *buffer, const int &buffer_size,const unsigned int& SecondTime,const unsigned int&MillisecondTime,const std::time_t &localTime);
void openUrl(const std::string &url,const std::string &name,InitConnect *connect)
{

    connect->OpenUrl(url.c_str(),name.c_str());
//    connect->OpenUrl("rtsp://admin:wanji123@192.168.3.108:554/cam/realmonitor?channel=1&subtype=0","temp1");
//    std::cout<<"ok"<<std::endl;
}
std::string frameJudge(uint8_t *frame)
{

    int temp = 0;
    //0001 1111 & 01100101 = 0000 0101 = 5 I帧
    temp = frame[4] & 31;
//    free(frame);
    std::string type = "";
    switch (temp) {
        case 7:
        {
            type = "SPS";
//            std::cout<<"frame "<<temp<<"sps"<<std::endl;
        }break;
        case 8:
        {
            type = "PPS";
//            std::cout<<"frame "<<temp<<"pps"<<std::endl;
        }break;
        case 5:
        {
            type = "I";
//            std::cout<<"frame "<<temp<<"i"<<std::endl;
        }break;
        case 1:
        {
            type = "P";
//            std::cout<<"frame "<<temp<<"p"<<std::endl;
        }break;
        default:break;
    }
    return type;
    std::cout<<"frame "<<temp<<"TYPE:"<<type<<std::endl;
//
}
void callBackTest(unsigned char *buffer, const int &buffer_size,const struct timeval &receive_time,const std::time_t &localTime)
{
//    std::cout<<"in callback size is"<<info_queue.size()<<std::endl;
    MyBufferInfo info;
//    info.buffer = buffer;
    std::cout << "info.buffer==" << std::endl;
    info.buffer = std::string((char*)buffer,buffer_size);
    std::cout << "info.buffer==" << info.buffer << std::endl;
    info.buffer_size = buffer_size;
    info.SecondTime=receive_time.tv_sec;
    info.MillisecondTime = receive_time.tv_usec;
    info.localTime = localTime;
//    buffer_queue.push(info);
//    std::cout<<"info "<<info.SecondTime+info.MillisecondTime<<std::endl;
//    frameJudge((uint8_t*)info.buffer.c_str());
    safe_queue.PushBack(info);
    free(buffer);
    buffer = nullptr;
//    auto temp = SecondTime-2208988800;
//    double temp1 = MillisecondTime;
//    std::cout<<"ms is:"<<temp1<<std::endl;
//    temp1 = temp1/pow(2,32);
//    temp1=temp1*1000;
//    std::cout<<"receive time is:"<<temp<<"."<<int(temp1)<<"local time is:"<<localTime<<std::endl;
}
void saveImgThread(cv::Mat img,const std::string &save_path)
{
    cv::imwrite(save_path,img);
}

void queueThread(H264Decode *decode)
{
//    while (true) {
//        if(buffer_queue.size()>0)
//        {
//            auto frameinfo =  buffer_queue.front();
//            std::cout<<"buffer size"<<frameinfo.buffer_size<<"local time"<<frameinfo.localTime<<std::endl;
//            buffer_queue.pop();
//        }
        MyBufferInfo info;
        if (safe_queue.PopFront(info, 100)) {
            std::cout << "info " << info.SecondTime + info.MillisecondTime << std::endl;

            frameJudge((uint8_t*)info.buffer.c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
//            auto l_start = std::chrono::high_resolution_clock::now();
//            cv::Mat mat;
//            bool succ = decode->decodeOneFrame((uint8_t*)info.buffer.c_str(),info.buffer_size,mat);
//            auto temp = info.SecondTime-2208988800;
//            double temp1 = info.MillisecondTime;
//            temp1 = temp1/pow(2,32);
//            temp1=temp1*1000;
//            std::stringstream st;
//            st<<temp<<"."<<int(temp1)<<"-"<<info.localTime;
//            std::string save_name ="./save/" + st.str() + ".jpg";
//            if(succ)
//            {
//                cv::cvtColor(mat, mat, CV_BGR2RGB);
//                auto img_th = std::thread(saveImgThread,mat,save_name);
//                img_th.detach();
////                cv::imwrite(save_name,mat);
//                auto l_end = std::chrono::high_resolution_clock::now();
//                uint64_t l_us = std::chrono::duration_cast<std::chrono::microseconds>(l_end-l_start).count();
//                std::cout << "need time:" << l_us << "us" << std::endl;
//            }

        }
//    } //zp
//        if(savf_queue.Size()>0)
//        {
//            MyBufferInfo info = savf_queue.Front();
////            std::cout<<"queue size is"<<savf_queue.Size()<<std::endl;
//            cv::Mat mat;
//            bool succ = decode->decodeOneFrame((uint8_t*)info.buffer.c_str(),info.buffer_size,mat);
//            auto temp = info.SecondTime-2208988800;
//            double temp1 = info.MillisecondTime;
//            temp1 = temp1/pow(2,32);
//            temp1=temp1*1000;
//            std::stringstream st;
//            st<<temp<<"."<<int(temp1)<<"-"<<info.localTime;
//            std::string save_name ="./save/" + st.str() + ".jpg";
//            if(succ)
//            {
//                cv::cvtColor(mat, mat, CV_BGR2RGB);
//                cv::imwrite(save_name,mat);
//            }
//            savf_queue.PopFront(info);
//        } else
//        {
//            continue;
//        }
//    }
}
void watchThread()
{
    while(true)
    {
        pid_t l_nPid = getpid();
//        pid_t l_nPid = 7829;

        double l_fdCpu = get_proc_cpu(l_nPid);
        int32_t l_niMem = get_proc_mem(l_nPid);
        int32_t l_nithNum = get_proc_thNum(l_nPid);
        std::cout << "PID : " << l_nPid << ", cpu: " << l_fdCpu << " %, mem: " << l_niMem / 1024.0
                      << " Mb, thNum : " << l_nithNum << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
int main(){
    H264Decode *decode = new H264Decode();
//    auto th = std::thread(queueThread,decode);
//    th.detach();
//    auto wat_th = std::thread(watchThread);
//    wat_th.detach();
//    while (1)
//    {
//
//    }
    for(int i=0;i<1;i++){
        pid_t fpid=fork();
        if(fpid==0)
        {
            printf("%d child  %4d %4d %4d \n",i,getppid(),getpid(),fpid);
//            auto th = std::thread(queueThread,decode);
//            th.detach();
//            InitConnect  * connect = new InitConnect();
//            connect->setCallBackFun(callBackTest);
//            connect->OpenUrl("rtsp://admin:wanji123@192.168.3.108:554/cam/realmonitor?channel=1&subtype=0","th1");
        }
        else
        {
            printf("%d parent %4d %4d %4d \n",i,getppid(),getpid(),fpid);
            auto th = std::thread(queueThread,decode);
            th.detach();
            auto wat_th = std::thread(watchThread);
            wat_th.detach();
            InitConnect  * connect = new InitConnect();
            connect->setCallBackFun(callBackTest);
            connect->OpenUrl("rtsp://admin:wanji123@10.100.16.1:554/cam/realmonitor?channel=1&subtype=0","th1");
        }
    }
//    while (1)
//    {
//
//    }
//    pid_t fpid; //fpid表示fork函数返回的值
//    fpid=fork();
//    if (fpid < 0)
//        printf("error in fork!");
//    else if (fpid == 0) {
////        auto th = std::thread(queueThread,decode);
////        th.detach();
//        printf("i am the child process, my process id is %d/n",getpid());
////        InitConnect  * connect = new InitConnect();
////        connect->setCallBackFun(callBackTest);
////        connect->OpenUrl("rtsp://admin:wanji123@192.168.3.108:554/cam/realmonitor?channel=1&subtype=0","th1");
//    }
//    else {
////        auto th = std::thread(queueThread,decode);
////        th.detach();
//        printf("i am the parent process, my process id is %d/n",getpid());
////        InitConnect  * connect = new InitConnect();
////        connect->setCallBackFun(callBackTest);
////        connect->OpenUrl("rtsp://admin:wanji123@192.168.3.58:554/cam/realmonitor?channel=1&subtype=0","th2");
//    }

//    auto  th = std::thread(openUrl,"rtsp://admin:wanji123@192.168.3.108:554/cam/realmonitor?channel=1&subtype=0","th1",connect);
//    th.join();
//    InitConnect  * connect1 = new InitConnect();
//    std::this_thread::sleep_for(std::chrono::milliseconds(3));
//    auto  th1 = std::thread(openUrl,"rtsp://admin:wanji123@192.168.3.58:554/cam/realmonitor?channel=1&subtype=0","th2",connect1);
//    th1.join();
//    while (1)
//    {
//        std::cout<<"main"<<std::endl;
//    }
    return 0;
}