#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include "./inc/InitConnect.h"
namespace py = pybind11;
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <arpa/inet.h>
#include "CSafeDataDeque.h"
#define VMRSS_LINE 22
typedef struct bufferInfo
{
    std::string buffer;
    int buffer_size;
    long int SecondTime;
    unsigned int MillisecondTime;
    std::time_t localTime;
}BufferInfo;
inline unsigned int get_proc_mem(unsigned int pid)
{

    char file_name[64] = {0};
    FILE *fd;
    char line_buff[512] = {0};
    sprintf(file_name, "/proc/%d/status", pid);

    fd = fopen(file_name, "r");
    if (NULL == fd)
    {
        //printf("没打开文件\n");
        return 0;
    }

    char name[64];
    int vmrss;
    for (int i = 0; i < VMRSS_LINE - 1; i++)
    {
        fgets(line_buff, sizeof(line_buff), fd);
    }

    fgets(line_buff, sizeof(line_buff), fd);
    sscanf(line_buff, "%s %d", name, &vmrss);
    fclose(fd);

    return int(vmrss);
}
class PyRtsp;
class PyRtsp
{
public:
    PyRtsp()
    {
        connect = new InitConnect();
        rtsp_ptr = this;
        app_run_ = true;
        pop_thread_ = new std::thread(&PyRtsp::threadPopData,this);
//        func = this->CallBack()
    }
    static void CallBack(unsigned char *buffer, const int &buffer_size,const struct timeval &receive_time,const std::time_t &localTime)
    {
        BufferInfo info;
        info.buffer = std::string((char*)buffer,buffer_size);
        info.buffer_size = buffer_size;
        info.SecondTime=receive_time.tv_sec;
        info.MillisecondTime = receive_time.tv_usec;
        info.localTime = localTime;
        rtsp_ptr->safe_queue_.PushBack(info);

        free(buffer);
        buffer = nullptr;
//        std::string s = std::string((char*)buffer,buffer_size);
//        free(buffer);
//        buffer = nullptr;
//        pid_t l_nPid = getpid();
//        int32_t l_niMem = get_proc_mem(l_nPid);
//        std::cout<<"c++ address is:"<<rtsp_ptr<<"SDK use mem is"<<l_niMem/1024.0<<std::endl;
////        std::cout<<"call back c++"<<std::endl;
//        rtsp_ptr->py_fun(py::bytes(s),buffer_size,receive_time.tv_sec,receive_time.tv_usec,localTime);
    }
    void OpenUrl(const std::string &url,const std::string &camera)
    {
        connect->OpenUrl(url.c_str(),camera.c_str());
    }
    void registerPYCallBack(const std::function<void(py::bytes,const int &buffer_size,const long int& SecondTime,const unsigned int&MillisecondTime,const long int  &localTime)> &fun)
    {
        py_fun = fun;
        connect->setCallBackFun(CallBack);
    }
    void threadPopData()
    {
        while (app_run_)
        {
            BufferInfo info;
            if(safe_queue_.PopFront(info,100))
            {
                std::cout<<"run run"<<std::endl;
                std::string frame_type = frameJudge((uint8_t*)info.buffer.c_str());
                if(only_frame_i_&&frame_type=="P")
                {
                    continue;
                }
                rtsp_ptr->py_fun(py::bytes(info.buffer),info.buffer_size,info.SecondTime,info.MillisecondTime,info.localTime);
//                py_fun(py::bytes(info.buffer),info.buffer_size,info.SecondTime,info.MillisecondTime,info.localTime);
            } else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }
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
    }
    void onlyUseIFrame(const bool& use)
    {
        only_frame_i_ = use;
    }
    void CloseConnect()
    {
        delete connect;
        connect = nullptr;
        app_run_ = false;
        if(pop_thread_->joinable())
        {
            pop_thread_->join();
            delete pop_thread_;
            pop_thread_ = nullptr;
        }
    }
private:
    InitConnect *connect = nullptr;
    bool only_frame_i_ = false;
    bool app_run_ = false;
    std::thread * pop_thread_ = nullptr;
    CSafeDataDeque <BufferInfo>safe_queue_;
    std::function<void(py::bytes,const int &buffer_size,const long int& SecondTime,const unsigned int&MillisecondTime,const long int  &localTime)> py_fun = nullptr;
//    CallFunc func;
    static PyRtsp *rtsp_ptr;
};
PyRtsp *PyRtsp::rtsp_ptr = nullptr;
PYBIND11_MODULE(Rtsp, m) {
    py::class_<PyRtsp>(m, "PyRtsp")
            .def(py::init())
            .def("registerPYCallBack", &PyRtsp::registerPYCallBack)
            .def("OpenUrl", &PyRtsp::OpenUrl)
            .def("onlyUseIFrame",&PyRtsp::onlyUseIFrame)
            .def("CloseConnect", &PyRtsp::CloseConnect);
}
