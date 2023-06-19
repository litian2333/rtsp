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
#include "CommonFunctions.h"
#include <fstream>
#define VMRSS_LINE 22
#define LOGINFO 0X001L
#define LOGWARRING 0x002L
#define LOGERROR 0x0004L
#define LOGAPPSTATUS 0x0008L
#define LOG(INFO,TYPE,...) { \
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());\
    auto current_time = std::put_time(std::localtime(&t), "%Y-%m-%d %X") ;\
    std::ofstream stream("./Log/info_log.csv",std::ios::app); \
    if (stream.is_open())\
    {\
        std::string type = "OTHERS:";\
        switch (TYPE) {       \
        case LOGINFO:        \
        {type = "INFO:"; }break;            \
        case LOGWARRING:     \
        {type = "WARRING:";}break;             \
        case LOGERROR:       \
        {type = "ERROR:";}break;             \
        case LOGAPPSTATUS:                     \
        {type = "APPSTATUS:";}break;                     \
        default:break;       \
        };\
        stream<<current_time<<":";\
        stream<<"-"<< type<<"-";\
        stream<<INFO<<std::endl;\
        stream.close();\
    }\
}
typedef struct bufferInfo
{
    std::string buffer;
    std::string frameType;
    int bufferSize;
    long int SecondTime;
    unsigned int MillisecondTime;
    std::time_t localTime;
}BufferInfo;
struct PyBufferInfo
{
    PyBufferInfo()
    {
        clear();
    }
    ~PyBufferInfo()
    {
        clear();
    }
    void clear()
    {
        std::string temp = "";
        frameType = "";
        buffer = py::bytes(temp);
        bufferSize = 0;
        SecondTime = 0;
        MillisecondTime = 0;
        localTime = 0;
    }
    py::bytes getBuffer(){return buffer;}
    const long int &getSecondTime(){return SecondTime;}
    const unsigned int &getMillisecondTime(){return MillisecondTime;}
    const std::time_t &getLocalTime(){return localTime;}
    const int & getBufferSize(){return bufferSize;}
    const std::string &getFrameType(){return frameType;}
    void setBuffer(py::bytes im_buffer){buffer = im_buffer;}
    void setSecondTime(const long int &second){SecondTime = second;}
    void setMilliSecondTime(const unsigned  int &milli_second){MillisecondTime = milli_second;}
    py::bytes buffer;
    int bufferSize;
    long int SecondTime;
    unsigned int MillisecondTime;
    std::time_t localTime;
    std::string frameType;
};

double getNowMem()
{
    pid_t l_nPid = getpid();
    int32_t l_niMem = get_proc_mem(l_nPid);
    std::cout<<"use mem is"<<l_niMem/1024.0<<std::endl;
    return l_niMem/1024.0;
}
std::string getNowCpu()
{
    pid_t l_nPid = getpid();
    double l_fdCpu = get_proc_cpu(l_nPid);
    std::string temp = std::to_string(l_fdCpu);
    return (temp+"%");
}
class PyRtsp;
std::function<void()> mainFun = nullptr;
class PyRtsp
{
public:
    PyRtsp()
    {
        MyMakeDirs("./Log");
        connect = new InitConnect();
        rtsp_ptr = this;
        app_run_ = true;
    }
    ~PyRtsp()
    {
        std::cout<<"pyrtsplib: pyrtsplib "<<version()<<"quit"<<std::endl;
        std::string str = "pyrtsplib "+version()+"quit";
        LOG(str,LOGERROR);
        if(get_thread_!= nullptr&&get_thread_->joinable())
        {
            get_thread_->join();
            delete get_thread_;
            get_thread_ = nullptr;
            std::cout<<"pyrtsplib: pyrtsplib "<<version()<<"all thread quit"<<std::endl;
        }
    }

    static void CallBack(unsigned char *buffer, const int &buffer_size,const struct timeval &receive_time,const std::time_t &localTime)
    {
//        py::gil_scoped_release release;     // 释放GIL锁
        if(rtsp_ptr->py_fun== nullptr)
        {
            BufferInfo info;
            info.buffer = std::string((char*)buffer,buffer_size);
            free(buffer);
            buffer = nullptr;
            std::string frame_type = rtsp_ptr->frameJudge((uint8_t*)info.buffer.c_str());
            if(rtsp_ptr->only_frame_i_&&frame_type=="P")
            {
                return;
            }
            info.bufferSize = buffer_size;        info.SecondTime=receive_time.tv_sec;
            info.MillisecondTime = receive_time.tv_usec;
            info.localTime = localTime;
            info.frameType = frame_type;
            rtsp_ptr->safe_queue_.PushBack(info);
            std::string push_info = "rtsp data push success";
            LOG(push_info,LOGINFO);
        }
        else
        {
            std::string s = std::string((char*)buffer,buffer_size);
            std::string frame_type = rtsp_ptr->frameJudge((uint8_t*)s.c_str());
            if(rtsp_ptr->only_frame_i_&&frame_type=="P")
            {
                return;
            }
            free(buffer);
            buffer = nullptr;
            rtsp_ptr->py_fun(py::bytes(s),buffer_size,receive_time.tv_sec,receive_time.tv_usec,localTime);
        }
    }
    void threadOpenUrl(const std::string &url,const std::string &camera)
    {
        std::cout<<"pyrtsplib: open thread id is"<<std::this_thread::get_id()<<std::endl;
        std::string open_info = "pyrtsplib: open " + url + "open success";
        LOG(open_info,LOGINFO);
        connect->setCallBackFun(CallBack);
        connect->OpenUrl(url.c_str(),camera.c_str());
    }
    /**
     *
     * @param url rtsp拉流地址
     * @param camera 建立相机连接的一个标志
     */
    void OpenUrl(const std::string &url,const std::string &camera)
    {
        std::cout<<"pyrtsplib: main thread id is"<<std::this_thread::get_id()<<std::endl;
        auto th = new std::thread(&PyRtsp::threadOpenUrl,this,url,camera);
        get_thread_ = th;
        th->detach();
    }
    /**
     *
     * @param fun
     */
    void registerPYCallBack(const std::function<void(py::bytes,const int &buffer_size,const long int& SecondTime,const unsigned int&MillisecondTime,const long int  &localTime)> &fun)
    {
        py_fun = fun;
//        connect->setCallBackFun(CallBack);
    }
    /**
     *
     * @param frame 带0001标志位的h264码流
     * @return 帧类型：I、P、SPS、PPS
     */
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
    /**
     *
     * @return PyBufferInfo类型
     */
    PyBufferInfo getQueueInfo()
    {
        BufferInfo info;
        if(safe_queue_.PopFront(info,100))
        {
            std::cout<<"get pop success"<<std::endl;
            PyBufferInfo py_info;
            py_info.buffer = py::bytes(info.buffer);
            py_info.bufferSize = info.bufferSize;
            py_info.SecondTime = info.SecondTime;
            py_info.MillisecondTime = info.MillisecondTime;
            py_info.localTime = info.localTime;
            py_info.frameType = info.frameType;
            return py_info;
        }
        else
        {
//            std::cout<<"now none"<<std::endl;
            PyBufferInfo py_info;
            return py_info;
        }
    }
    PyBufferInfo getPyQueueInfo()
    {
        PyBufferInfo info;
        if(py_queue_.PopFront(info,100))
        {
            std::cout<<" py_queue get pop success"<<std::endl;
            return info;
        }
        else
        {
            PyBufferInfo py_info;
            return py_info;
        }
    }
    void setPyQueueInfo(PyBufferInfo info)
    {
        py_queue_.PushBack(info);
        std::cout<<" py_queue set push success"<<std::endl;
    }
    /**
     *
     * @return  当前rtsp拉流库版本号
     */
    std::string version()
    {
        return "v1.1.1_221122";
    }
    /**
     *
     * @param use true:only use I Frame false :All Frame
     */
    void onlyUseIFrame(const bool& use)
    {
        only_frame_i_ = use;
    }
    /**
     *
     * @param diff int类型时间累积值
     */
    void setCameraTimeDiff(const int &diff)
    {
        if(connect)
        {
            connect->setTimeDiff(diff);
        }
    }
    /**
     * 关闭拉流连接，外部调用程序退出的时候调用该接口
     */
    void CloseConnect()
    {
        connect->CloseCamera();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if(get_thread_->joinable())
        {
            get_thread_->join();
            delete get_thread_;
            get_thread_ = nullptr;
        }
        delete connect;
        connect = nullptr;
        app_run_ = false;
        std::cout<<"pyrtsplib: "<<version()<<"close camera"<<std::endl;
    }
private:
    InitConnect *connect = nullptr;
    bool only_frame_i_ = false;
    bool app_run_ = false;
    std::thread * get_thread_ = nullptr;
    CSafeDataDeque <BufferInfo>safe_queue_;
    std::function<void(py::bytes,const int &buffer_size,const long int& SecondTime,const unsigned int&MillisecondTime,const long int  &localTime)> py_fun = nullptr;
//    CallFunc func;
    CSafeDataDeque<PyBufferInfo>py_queue_;
    static PyRtsp *rtsp_ptr;
};
PyRtsp *PyRtsp::rtsp_ptr = nullptr;
PYBIND11_MODULE(Rtsp, m) {
    py::class_<PyRtsp>(m, "PyRtsp")
            .def(py::init())
            .def("registerPYCallBack", &PyRtsp::registerPYCallBack)
            .def("OpenUrl", &PyRtsp::OpenUrl)
            .def("setCameraTimeDiff",&PyRtsp::setCameraTimeDiff)
            .def("version",&PyRtsp::version)
            .def("onlyUseIFrame",&PyRtsp::onlyUseIFrame)
            .def("getQueueInfo",&PyRtsp::getQueueInfo)
            .def("getPyQueueInfo",&PyRtsp::getPyQueueInfo)
            .def("setPyQueueInfo",&PyRtsp::setPyQueueInfo)
            .def("CloseConnect", &PyRtsp::CloseConnect);
    py::class_<PyBufferInfo>(m, "PyBufferInfo")
            .def(py::init())
            .def("getBuffer",&PyBufferInfo::getBuffer)
            .def("getSecondTime",&PyBufferInfo::getSecondTime)
            .def("getMillisecondTime",&PyBufferInfo::getMillisecondTime)
            .def("setBuffer",&PyBufferInfo::setBuffer)
            .def("setSecondTime",&PyBufferInfo::setSecondTime)
            .def("setMillisecondTime",&PyBufferInfo::setMilliSecondTime)
            .def("getLocalTime",&PyBufferInfo::getLocalTime)
            .def("getFrameType",&PyBufferInfo::getFrameType)
            .def("getBufferSize",&PyBufferInfo::getBufferSize);
    m.def("getNowMem",getNowMem);
    m.def("getNowCpu",getNowCpu);


}
//,py::call_guard<py::gil_scoped_release>()
